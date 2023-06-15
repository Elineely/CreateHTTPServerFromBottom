#!/usr/bin/python3
import os
import sys
import time

print("HTTP/1.1 200 OK\r\n", end='')
print("Status: 200\r\n", end='')
print("Content-Type: text/plain\r\n\r\n", end='')

request_method = os.environ.get('REQUEST_METHOD', '')
upload_path = os.environ.get('X_UPLOAD_PATH', '')
if request_method == "GET":
    print("GOTTA HOME!", end='')
elif request_method == "POST":
    print("GOTTA POST!", end='')
