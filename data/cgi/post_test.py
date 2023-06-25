#!/usr/bin/python
import cgi
import os
import sys

import cgitb
cgitb.enable()

upload_dir = './data/upload/'

form = cgi.FieldStorage()

if 'image' in form:
    image = form['image']

    if image.filename:
        # Create a unique filename for the image
        save_path = os.path.join(upload_dir, image.filename)
        with open(save_path, 'wb') as a_file:
            a_file.write(image.file.read())

        print('Content-Type: text/html\r')
        print('Status: 200\r\n\r')
        print('<html><body>')
        print('<h2>Image saved successfully!</h2>')
        print('</body></html>')
    else:
        print('Content-Type: text/html\r')
        print('Status: 400\r\n\r')
        print('<html><body>')
        print('<h2>There is no image file!</h2>')
        print('</body></html>')
        
else:
    print('Content-Type: text/html\r')
    print('Status: 500\r\n\r')
    print('<html><body>')
    print('<h2>No image file found in the form data!</h2>')
    print('</body></html>')