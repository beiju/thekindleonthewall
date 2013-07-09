#!/usr/bin/env python

"""Settings file for the Arduino server."""

# Serial configuration
SIGNATURE = 'Dog'
FILENAME = '/dev/tty.usbmodem1711'
BAUD = 115200
TIMEOUT = 1  # seconds

# Forecast API connection info
FORECAST_API = '79110f2572af93fe93997203c876e83a' 
FORECAST_UNITS = 'uk' 
FORECAST_LAT = 34.1511
FORECAST_LONG = -118.4443
FORECAST_INTERVAL = 5*60

# Google API connection
GOOGLE_CLIENT_ID = '1068849907100-or72tfuoovok2ircqstcvpc7bd4i18sh.apps.googleusercontent.com'
GOOGLE_CLIENT_SECRET = 'Nxv9OacMZK8LZQrT5y_V7RCU'
GOOGLE_REDIRECT_URI = '/authorized'  # one of the Redirect URIs from Google APIs console
GOOGLE_SECRET_KEY = 'AIzaSyAcpSlpgyTE2LbW8-RCmMvg3NYXWbPJK_s'
CALENDAR_INTERVAL = 5*60
