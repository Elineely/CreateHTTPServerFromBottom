#!/usr/bin/python
#/opt/homebrew/bin/python3

import cgi
import os
import cgitb
cgitb.enable()
# # import cgitb; cgitb.enable(display=0, logdir="./log")

form = cgi.FieldStorage()

from datetime import datetime
now = datetime.now()
default_header = "Content-Type: text/html\r\n\r"
default_content = "<html><head>\n" + \
               "<title>cute CGI posting</title>\n" + \
               "</head><body>\n" + \
              "<h1>Post Success!</h1>\n" + \
              "<p>with kkam-bo</p>\n" + \
              "<p>" + now.strftime('%Y-%m-%d %H:%M:%S') + "</p>\n" + \
             "</body></html>\n"


# if (len(text_content) > maxlen): # len(text_content.encode('uft-8))
	# return error_page

content_length = os.getenv('CONTENT_LENGTH=')
content_type = os.getenv('CONTENT_TYPE=')
if (content_length == None | content_length == "" | content_length == "0"):
    print(default_header)
    print(default_content)
    exit()
    # return error page?
if not (content_type == "text/html" | content_type == "image/jpeg"):
    print(default_header)
    print(default_content)
    exit()
    # return error page?

# set and make upload_path
if os.getenv('X_UPLOAD_PATH'):
    upload_path = os.getenv('X_UPLOAD_PATH')
else:
    upload_path = ""

if os.path.exists(upload_path):
	upload_path += "/"
else:
	upload_path = "./tmp_d/"

if not os.path.exists(upload_path):
	os.mkdir(upload_path)

# set path_info file
if os.getenv('PATH_INFO'):
    new_file = os.getenv('PATH_INFO')
else:
    new_file = "tmp.txt"
new_file = upload_path + new_file

# make path_info file
import sys
text_content = sys.stdin.readline()

with open(new_file, 'w') as text_file:
	text_file.write(text_content)

# which header needed
if (content_type == "text/html"):
    print("Content-Type: text/html\r\n\r")
else if (content_type == "image/jpeg"):
    print("Content-Type: image/jpeg\r\n\r")
else:
    print(default_header)

with open(new_file, 'r') as text_file:
    test = text_file.read()
    sys.stdout.write(test)


# /////////////////////////////////////
# if os.path.exists("exam.txt"):
#   os.remove("exam.txt")
# else:
#   print("not exist")

# example
# if "name" not in form or "addr" not in form:
#     print("<H1>Error</H1>")
#     print("Please fill in the name and addr fields.")

# print("<p>name:", form["name"].value)
# print("<p>addr:", form["addr"].value)

# value = form.getlist("username")
# usernames = ",".join(value)

# fileitem = form["userfile"]
# if fileitem.file:
#     # It's an uploaded file; count lines
#     linecount = 0
#     while True:
#         line = fileitem.file.readline()
#         if not line: break
#         linecount = linecount + 1
