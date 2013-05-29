from flask import Flask, request, g, render_template, redirect, url_for, session, jsonify, json
from apscheduler.scheduler import Scheduler
from flask_oauth import OAuth
from forecastio import Forecast
import room_controller.arduino
import time
import pytz
import datetime
from dateutil.parser import parse
from settings import *
import logging

app = Flask(__name__)
logging.basicConfig() # used by apscheduler
sched = Scheduler()
sched.start()
app.secret_key = GOOGLE_SECRET_KEY

forecast = Forecast(FORECAST_API, FORECAST_UNITS, FORECAST_LAT, FORECAST_LONG)

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

the_arduino = room_controller.arduino.Arduino(SIGNATURE, FILENAME, BAUD, TIMEOUT)
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
    return render_template('index.html')
    
@app.route('/update')
def update():   
    lastUpdate = float(request.args.get('lastUpdate') or 0)
    data = dict()
    
    if 'forecast_upd' not in cache:
        forecast_upd()
    if lastUpdate < cache['forecast_upd']: 
        data['forecast'] = cache['forecast']
    
    # Needs special treatment because calling local_info_upd() does not garuantee an update
    if 'local_info_upd' not in cache and 'local_info_upd_requested' not in cache:
        local_info_upd()
        cache['local_info_upd_requested'] = True
    if 'local_info_upd' in cache and lastUpdate < cache['local_info_upd']:
        data['local_info'] = cache['local_info']
    
    if 'calendar_upd' not in cache:
        if not calendar_upd(): # calendar_upd() sets cache[]
            data['calendar'] = {
                'error': "Google Calendar Not Authorized",
                'err_code': "not_authorized",
                'err_note': "Visit /login to authorize"
            }
            print "Unauthorized request for calendar"
    if ('calendar' not in data or 'error' not in data['calendar']) and lastUpdate < cache['calendar_upd']: 
        data['calendar'] = cache['calendar']
    
    return jsonify(data)

@app.route('/run', methods=['POST'])
def run_command():
    command = request.form.get('command', '')
    print 'command', command
    the_arduino.send_raw_command(command)
    print 'command', command
    return redirect(url_for('run'))
    
@app.route('/login')
def login():
    callback=url_for('authorized', _external=True)
    return google.authorize(callback=callback)

    
@app.route(GOOGLE_REDIRECT_URI)
@google.authorized_handler
def authorized(response):
    access_token = response['access_token']
    session['access_token'] = access_token, ''
    return redirect(url_for('update'))


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

@sched.interval_schedule(minutes=5)
def forecast_upd():
    print "Updating forecast"
    forecast.get_forecast()
    data = dict()
    
    if 'current' in forecast.forecast: 
        data['temp_c'] = forecast.current['temperature']
        data['temp_f'] = 32 + 9 / 5 * forecast.current['temperature']
        data['wind_speed'] = forecast.current['windSpeed']
        data['wind_dir'] = forecast.current['windBearing']
    if 'minutely' in forecast.forecast: data['current'] = forecast.nexthour['summary']
    if 'hourly' in forecast.forecast: data['next'] = forecast.hourly['summary']
    if 'daily' in forecast.forecast: data['later'] = forecast.daily['summary']
    
    cache['forecast'] = data
    cache['forecast_upd'] = time.time()

@sched.interval_schedule(seconds=1)
def refresh_arduino():
    if the_arduino.connected:
        the_arduino.refresh()
    else:
        the_arduino.open()
        
        if the_arduino.connected:
            print "Reconnected arduino"
        else:
            print "Failed to reconnect arduino"


@sched.interval_schedule(minutes=1)
def local_info_upd():
    the_arduino.send_command('send_status', 'lights')
    # the_arduino.send_command('send_status', 'temperature') # When there's a temp sensor on the arduino, enable this

@sched.interval_schedule(minutes=5)
def calendar_upd():
    caldata = dict()
    
    try:
        access_token = session.get('access_token')
        if access_token is None:
            return False
    
        access_token = access_token[0]
        
        caldata['daka_hours'] = query_gcal(access_token, '0cto0462lqrpt673m51bf1ucuk%40group.calendar.google.com')
        if caldata['daka_hours'] == True: return False
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
    
        print 'updating calendar'
        cache['calendar'] = caldata
        cache['calendar_upd'] = time.time()
        return True
    except RuntimeError:
        with app.test_request_context('/update'):
            calendar_upd()

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
                    'start_time': str(startTime),
                    'start_time_str': startTime.strftime('%I:%M %p'),
                    'end_time': str(endTime),
                    'end_time_str': endTime.strftime('%I:%M %p'),
                    'duration': time_btwn(startTime, endTime),
                    'remaining_time': time_btwn(now, endTime),
                    'event_name': item['summary']
                }
                if 'location' in item:
                    data['current']['event_loc'] = item['location']

        if 'next' not in data:
            if startTime > now: # The first event for which startTime is after now is 'next', since events are ordered by startTime
                data['next'] = {
                    'start_time': str(startTime),
                    'start_time_str': startTime.strftime('%I:%M %p'),
                    'end_time': str(endTime),
                    'end_time_str': endTime.strftime('%I:%M %p'),
                    'duration': time_btwn(startTime, endTime),
                    'time_until': time_btwn(startTime, now),
                    'event_name': item['summary'],
                    'continuation': 'current' in data and (abs(startTime - parse(data['current']['end_time'])) < datetime.timedelta(minutes=5))
                }
                if 'location' in item:
                    data['next']['event_loc'] = item['location']
                
                
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

if __name__ == "__main__":
    app.run('0.0.0.0', 5000, debug=True, use_reloader=False) # Reloader doesn't play nice with apscheduler