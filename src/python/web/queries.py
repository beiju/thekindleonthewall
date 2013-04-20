def query_forecast():
    forecast.get_forecast()
    data['forecast'] = {
        'temp_c': forecast.current['temperature'],
        'temp_f': 32 + 9 / 5 * forecast.current['temperature'],
        'wind_speed': forecast.current['windSpeed'],
        'wind_dir': forecast.current['windBearing'],
        
        'current': forecast.nexthour['summary'],
        'next': forecast.hourly['summary'],
        'later': forecast.daily['summary']
    }
    cache['forecast_upd'] = time.time()
    
def query_google():
    access_token = session.get('access_token')
    if access_token is None:
        return redirect(url_for('login'))
    
    access_token = access_token[0]
    from urllib2 import Request, urlopen, URLError
    
    headers = {'Authorization': 'OAuth '+access_token}
    req = Request('https://www.googleapis.com/auth/calendar',
                  None, headers)
    try:
        res = urlopen(req)
    except URLError, e:
        if e.code == 401:
            # Unauthorized - bad token
            session.pop('access_token', None)
            return redirect(url_for('login'))
        res = urlopen(req)