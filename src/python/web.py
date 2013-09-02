from flask import Flask, request, g, render_template, redirect, url_for, session, jsonify, json
from apscheduler.scheduler import Scheduler
from flask_oauth import OAuth
import requests
import arduino
import time
import pytz
import datetime
from dateutil.parser import parse
from settings import *
import logging
import threading

app = Flask(__name__, static_url_path='')
logging.basicConfig() # used by apscheduler
sched = Scheduler()
sched.start()
app.secret_key = GOOGLE_SECRET_KEY

oauth = OAuth()
google = oauth.remote_app('google',
                          base_url='https://www.google.com/accounts/',
                          authorize_url='https://accounts.google.com/o/oauth2/auth',
                          request_token_url=None,
                          request_token_params={'scope': 'https://www.googleapis.com/auth/calendar',
                                                'response_type': 'code'},
                          access_token_url='https://accounts.google.com/o/oauth2/token',
                          access_token_method='POST',
                          access_token_params={'grant_type': 'authorization_code'},
                          consumer_key=GOOGLE_CLIENT_ID,
                          consumer_secret=GOOGLE_CLIENT_SECRET)

the_arduino = arduino.Arduino(SIGNATURE, FILENAME, BAUD, TIMEOUT)
cache = dict()
print "Flask imported"
if the_arduino.connected:
    print "Starting up with arduino connected"
else:
    print "Starting up without arduino connected"
    
##########
# Routes #
##########

@app.route('/')
def index():
    print 'connected', the_arduino.connected #!
    return app.send_static_file('index.html')
    
@app.route('/update')
def update():
    lastUpdate = float(request.args.get('lastUpdate') or 0)
    data = dict()
    
    sources = [
        ('weather', forecast_upd),
        ('local_info', local_info_upd),
        ('calendar', calendar_upd)
    ]
    
    for item in sources:
        data[ item[0] ] = retrieve_cached(item[0], lastUpdate, item[1])
        if data[ item[0] ] is None: del data[ item[0] ]
        
    return jsonify(data)

@app.route('/run', methods=['POST'])
def run_command():
    command = request.form.get('command', '')
    print 'command', command
    the_arduino.send_raw_command(command)
    print 'command', command
    return redirect(url_for('run'))

@app.route('/clear')
def clearCache():
    cache.clear()
    return redirect(url_for('index'))    
    
@app.route('/login')
def login():
    if cache and 'calendar' in cache and 'data' in cache['calendar']:
        cache['calendar']['data']['err_code'] = "pending_authorization" # Avoid endless redirects
    callback=url_for('authorized', _external=True)
    return google.authorize(callback=callback)

@app.route(GOOGLE_REDIRECT_URI)
@google.authorized_handler
def authorized(response):
    access_token = response['access_token']
    session['access_token'] = access_token, ''
    print access_token
    if 'calendar' in cache: 
        del cache['calendar'] # Update the calendar the next time it's requested
    return redirect(url_for('index'))

@google.tokengetter
def get_access_token():
    return session.get('access_token')
    
####################
# Arduino Commands #
####################

@the_arduino.command('lights_status')
def upd_lights_status(args):
    if 'local_info' not in cache:
        cache['local_info'] = dict()
    cache['local_info']['light1'] = (int(args[0]) & 0b001 == 0b001)
    cache['local_info']['light2'] = (int(args[0]) & 0b010 == 0b010)
    cache['local_info_upd'] = time.time()
    print 'lights updated '
    
###########
# Updates #
###########

@sched.interval_schedule(seconds=1)
def refresh_arduino():
    if the_arduino.connected:
        the_arduino.refresh()
    else:
        the_arduino.open()
        
        if the_arduino.connected:
            print "Reconnected arduino"

@sched.interval_schedule(minutes=5)
def forecast_upd():
    print "Updating forecast"
    
    forecast = requests.get('https://api.forecast.io/forecast/'+FORECAST_API+'/'+FORECAST_LAT+','+FORECAST_LONG, params = {
        'units': FORECAST_UNITS,
        'exclude': 'flags'
    })
    
    data = forecast.json()
    
    for key in ['minutely', 'hourly', 'daily']:
        if key in data and 'data' in data[key]: del data[key]['data']
    
    # forecast.load_forecast(FORECAST_LAT, FORECAST_LONG, units=FORECAST_UNITS)
# 
#     currently = forecast.get_currently()
#     minutely = forecast.get_minutely()
#     hourly = forecast.get_hourly()
#     daily = forecast.get_daily()
#     data = {
#         'current': {
#             'icon': currently.icon,
#             'description': currently.summary,
#             'temperature': {
#                 'c': currently.temperature,
#                 'f': currently.temperature * 9 / 5 + 32
#             },
#             'wind': {
#                 'speed': currently.windspeed,
#                 'angle': currently.windbaring # Typo in the library
#             }
#         },
#         'next_hr': {
#             'icon': minutely.icon,
#             'description': minutely.summary
#         },
#         'tomorrow': {
#             'icon': hourly.icon,
#             'description': hourly.summary
#         },
#         'this_week': {
#             'icon': daily.icon,
#             'description': daily.summary
#         }
#     }

    if 'weather' not in cache: cache['weather'] = dict()
    cache['weather']['data'] = data
    cache['weather']['last_update'] = time.time()

@sched.interval_schedule(minutes=1)
def local_info_upd():
    print "Updating Local Info"
    the_arduino.send_command('send_status', 'lights')
    # the_arduino.send_command('send_status', 'temperature') # When there's a temp sensor on the arduino, enable this

@sched.interval_schedule(minutes=5)
def calendar_upd(access_token=False):
    print "Updating Calendar"
    caldata = dict()
    
    try:
        access_token = access_token or session.get('access_token')
        if access_token is None:
            fill_calendar_cache({
                'error': "Google Calendar Not Authorized",
                'err_code': "not_authorized",
                'err_note': "Visit /login to authorize"
            })
            return False
        access_token = access_token[0]
        
        caldata['daka_hours'] = query_gcal(access_token, '0cto0462lqrpt673m51bf1ucuk%40group.calendar.google.com')
        if caldata['daka_hours'] == True: 
            return False
        caldata['spoon_hours'] = query_gcal(access_token, 'ieqe1kvtb6narapqoafv59umog%40group.calendar.google.com')

        caldata['will'] = query_gcals(access_token, '488or1ai5vadl5psti3iq8ipgs%40group.calendar.google.com', # work
            'beiju.i.am%40gmail.com', # personal
            't7ijq9al3asosqh1jnp93hvgdk%40group.calendar.google.com') # class
        caldata['ian'] = query_gcals(access_token, 'sodbfdhm4q7api4qvf5h5k7rlg%40group.calendar.google.com', # social
            '36gqite9pam369c6mknuttgsqg%40group.calendar.google.com', # work
            'achgl7e3m1pokdo8o1uqis70fk%40group.calendar.google.com', # ACM
            'jnqo9lo8efm5ogj78pr176qstg%40group.calendar.google.com', # WPI Extracurricular
            'a82i41iavlvd37d9fnrofklrms%40group.calendar.google.com', # WPI Schoolwork
            'ianonavy%40gmail.com')
    
        fill_calendar_cache(caldata)
    except RuntimeError:
        with app.test_request_context('/update'):
            calendar_upd()
            
####################
# Helper Functions #
####################

def retrieve_cached(name, since, getter):
    if name not in cache: cache[name] = dict()

    if 'data' not in cache[name]:
        if 'last_request' not in cache[name] or cache[name]['last_request'] + RETRY_REQUEST_TIMEOUT < time.time():
            # This is for google calendar, which tries its hardest to make my code break
            if name is 'calendar':
                args = [session.get('access_token')]
            else:
                args = []
            # Force the function to run asynchronously
            thr = threading.Thread(target=getter, args=args)
            thr.start()
            cache[name]['last_request'] = time.time()

    if 'data' in cache[name] and ('last_update' not in cache[name] or cache[name]['last_update'] > since):
        return cache[name]['data']

    return None

def query_gcals(access_token, *calIDs):
    data = dict() 
    for calID in calIDs:
        currdata = query_gcal(access_token, calID)
        if currdata == False:
            continue
        if 'current' in currdata and currdata['current'] and 'current' not in data:
            data['current'] = currdata['current']

        if 'next' in currdata and currdata['next'] and ('next' not in data or parse(currdata['next']['start_time']) < parse(data['next']['start_time'])):
            data['next'] = currdata['next']

            if 'current' in data and 'end_time' in data['current'] and 'start_time' in data['next'] and \
                    abs(parse(data['current']['end_time']) - parse(data['next']['start_time'])) < time.timedelta(minutes=5):
                data['next']['continuation'] = True
                return data; # at this point the data won't ever change
            else:
                data['next']['continuation'] = False
    return data
    
def query_gcal(access_token, calID):
    from urllib2 import Request, urlopen, URLError

    url = 'https://www.googleapis.com/calendar/v3/calendars/'
    url+= calID+'/events'
    url+= '?maxResults=7'
    url+= '&orderBy=startTime'
    url+= '&singleEvents=true'
    url+= '&timeMin='+datetime.date.today().isoformat()+'T00:00:00Z'
    url+= '&key='+GOOGLE_SECRET_KEY
    
    req = Request(url, None, {'Authorization': 'OAuth '+access_token})
    try:
        res = urlopen(req)
    except URLError, e:
        if e.code == 401:
            # Unauthorized - bad token
            session.pop('access_token', None)
            return True
        if e.code == 403: 
            return {
                'error': "403 Forbidden", 
                'err_code': "api_403",
                'err_note': "This error is often caused by sending an API request from an IP address not included in https://code.google.com/apis/console",
                'url': url
            }
        res = urlopen(req) 
    
    try:
        items = json.loads(res.read())['items']
    except KeyError, e:
        return False

    data = dict()
    now = datetime.datetime.now(pytz.utc)
    for item in items:
        if 'dateTime' not in item['start']: #all-day event
            startTime = pytz.utc.localize(parse(item['start']['date']))
            endTime = pytz.utc.localize(parse(item['end']['date'])+datetime.timedelta(days=1,seconds=-1))
        else:
            startTime = parse(item['start']['dateTime'])
            endTime = parse(item['end']['dateTime'])
        
        if 'current' not in data: # Look for the current event
            if startTime < now and endTime > now:
                data['current'] = {
                    'exists': True,
                    'start_time': startTime.isoformat(),
                    'end_time': endTime.isoformat(),
                    'duration': time_btwn(startTime, endTime),
                    'remaining_time': time_btwn(now, endTime),
                }
                if 'location' in item:
                    data['current']['event_loc'] = item['location']
                if 'summary' in item:
                    data['current']['event_name'] = item['summary']

        if 'next' not in data:
            if startTime > now: # The first event for which startTime is after now is 'next', since events are ordered by startTime
                data['next'] = {
                    'exists': True,
                    'start_time': startTime.isoformat(),
                    'end_time': endTime.isoformat(),
                    'duration': time_btwn(startTime, endTime),
                    'time_until': time_btwn(startTime, now),
                    'continuation': 'current' in data and (abs(startTime - parse(data['current']['end_time'])) < datetime.timedelta(minutes=5))
                }
                if 'location' in item:
                    data['next']['event_loc'] = item['location']
                if 'summary' in item:
                    data['next']['event_name'] = item['summary']
                
                
    if 'current' not in data:
        data['current'] = {} # Do this to clear cached result
     
    return data

def time_btwn(datetime1, datetime2):
    hours, remainder = divmod((datetime2 - datetime1).seconds, 3600)
    minutes, seconds = divmod(remainder, 60)
    strval = ''
    if hours == 1:
        strval += '1 hour '
    elif hours != 0:
        strval += str(hours)+' hours '
    if minutes == 1:
        strval += '1 minute '
    elif minutes != 0: 
        strval += str(minutes)+' minutes '
    return strval.strip()
    
def fill_calendar_cache(data):    
    cache['calendar']['data'] = data
    cache['calendar']['last_update'] = time.time()

########
# Init #
########

if __name__ == "__main__":
    app.run('0.0.0.0', 5000, debug=True, use_reloader=False) # Reloader doesn't play nice with apscheduler
