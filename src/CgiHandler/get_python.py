#!/usr/bin/python
#/opt/homebrew/bin/python3

# keep in mind that the CGI module in Python has been removed in Python 3,
# so you may need to use an alternative approach for handling CGI requests.

import cgi
import os
import sys
import cgitb
cgitb.enable()
# # import cgitb; cgitb.enable(display=0, logdir="./log")

# Create a new CGI object
cgi_obj = cgi.FieldStorage()

from datetime import datetime

now = datetime.now()
default_header = "Content-Type: text/html\r\n\r"
default_content = "<html><head>\n" + \
               "<title>cute CGI output</title>\n" + \
               "</head><body>\n" + \
               "<h1>This is my so cute CGI output</h1>\n" + \
               "<p>Bang-gle</p>\n" + \
               "<p>" + now.strftime('%Y-%m-%d %H:%M:%S') + "</p>\n" + \
               "</body></html>"

# check path_info value
path_info = os.getenv('PATH_INFO')
if (path_info == None or path_info == ""):
    path_info = "index.html"
elif not (os.path.isfile(path_info)):
    path_info = "index.html"
elif not (os.path.exists(path_info)):
    path_info = "index.html"

# make response message
if (os.path.splitext(path_info)[1] == "html" or os.path.splitext(path_info)[1] == "txt"):
    print("Content-Type: text/html\r\n\r")
    with open(path_info, 'r') as text_file:
        text_content = text_file.read()
        sys.stdout.write(text_content)
elif (os.path.splitext(path_info)[1] == "jpg" or os.path.splitext(path_info)[1] == "jpeg"):
    print("Content-Type: image/jpeg\r\n\r")
    with open(path_info, 'rb') as image_file:
        image_content = image_file.read()
        sys.stdout.write(image_content)
else:
    print(default_header)
    print(default_content)
    # or return error
