#!/usr/bin/python3
import os
import sys

print("HTTP/1.1 200 OK\r\n", end='')
print("Content-type: text/plain\r\n\r\n", end='')

request_method = os.environ.get('REQUEST_METHOD', '')
upload_path = os.environ.get('X_UPLOAD_PATH', '')
if request_method == "GET":
    print("GOTTA HOME!", end='')
elif request_method == "POST":
    with open(upload_path + "/" + "new_file", 'w') as text_file:
	    text_file.write("this is open!")
    print("upload path", upload_path)
    print("GOTTA POST!", end='')
