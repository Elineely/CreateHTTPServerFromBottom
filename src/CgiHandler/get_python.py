#!/opt/homebrew/bin/python3
#/usr/bin/python

# keep in mind that the CGI module in Python has been removed in Python 3,
# so you may need to use an alternative approach for handling CGI requests.

import cgi
import os
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
             "</body></html>\n"

#*******************************************#
# only for '.py' file that we've already set
#*******************************************#
print('\n *** CASE OF DEFAULT *** \n')

print(default_header)
print(default_content)

#**************************************************************#
# case of taking text file 'PATH_INFO' env (maybe for cgi tester?)
#**************************************************************#
print('\n *** CASE OF TEXT PATH_INFO *** \n')

print("Content-Type: text/html\r\n\r")
print(os.getenv('PATH_INFO'))
path_info_t = os.getenv('PATH_INFO')
# with -> no need to use close() function
if os.path.isfile(path_info_t):
    with open(path_info_t, 'r') as text_file:
        text_content = text_file.read()
        print(text_content)
else:
    print(default_header)
    print(default_content)

#**************************************************************#
# case of taking binary file 'PATH_INFO' env (maybe for cgi tester?)
#**************************************************************#
print('\n *** CASE OF BINARY PATH_INFO *** \n')

# pip install pillow
# from PIL import Image
# from PIL import ImageDraw

print("Content-Type: image/jpeg\r\n\r")
path_info_i =  os.getenv('PATH_INFO')
if os.path.isfile(path_info_i):
    with open(path_info_i, 'rb') as image_file:
        image_content = image_file.read()
        print(image_content)     # b'~~~\n'
        # for b in image_content:  # 97 97 97 10
        #     print(b)
        # byte = image_file.read(1) # byte by byte ..
        # while byte != b"":
        #     print(byte, end="")
        #     byte = image_file.read(1)
# else:
#     print(default_header)
#     print(default_content)

# img = Image.open(path_info_i)
# draw = ImageDraw.Draw(img)
# display(img)
