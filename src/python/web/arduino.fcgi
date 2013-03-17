#!/usr/bin/python
from flup.server.fcgi import WSGIServer
from room_controller.web import app

if __name__ == '__main__':
    WSGIServer(app, bindAddress=('127.0.0.1', '9000')).run()
