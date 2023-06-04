#!/opt/homebrew/bin/python3
#/usr/bin/python

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


text_content = input()
# if (len(text_content) > maxlen): # len(text_content.encode('uft-8))
	# return error_page

upload_path = os.getenv('X_UPLOAD_PATH')

if os.path.exists(upload_path):
	upload_path += "/"
else:
	upload_path = "./tmp_d/"

if not os.path.exists(upload_path):
	os.mkdir(upload_path)

new_file = os.getenv('PATH_INFO')
new_file = upload_path + new_file

with open(new_file) as text_file: # saved path where?
	text_file.write(text_content)

print(default_header)
print(default_content)
# or return text_file?



# /////////////////////////////////////////////

# save_path = os.getenv("SAVED_PATH")

# if save_path is None:
#     save_path = "./tmp/"
# else:
#     save_path += "/"

# print("Content-Type: text/html\r")

# if not os.path.exists(save_path):
#     os.mkdir(save_path)

# # Parse the form data
# form = cgi.FieldStorage()
# # Extract the uploaded files
# files = {}
# for field in form.keys():
#     # Check if the field is a file field
#     if isinstance(form[field], cgi.FieldStorage) and form[field].filename:
#         # Save the file to the specified location
#         with open(save_path + form[field].filename, 'wb') as f:
#             f.write(form[field].file.read())
#         files[field] = form[field].filename

# if len(files) > 0:
#     message = "file uploaded success"
# else:
#     message = "file uploaded failed"

# # Print the uploaded files
# response = "<html><body><center>"
# response += "<h1>{}<h1>".format(message)
# for field, filename in files.items():
#     response += "<p>{}: {}</p>\n".format(field, filename)
# response += "</center></body></html>"

# print("\r")
# print(response)

# exit()


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
