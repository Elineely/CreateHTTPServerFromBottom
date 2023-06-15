#!/usr/bin/python3
import cgi
import os
import sys

# Set the directory to store the uploaded image
upload_dir = './data/upload/'

# Create an instance of FieldStorage
form = cgi.FieldStorage()

# Get the image file from the form data
if 'image' in form:
    image = form['image']

    if image.value:
        # Create a unique value for the image
        target_path = os.path.join(upload_dir, image.value)
        print(target_path, file=sys.stderr)
        # Save the image to the upload directory
        image_data = sys.stdin.buffer.readline()
        # print(image_data, file=sys.stderr)
        with open(target_path, 'wb') as file:
            file.write(image_data)
        print('Content-Type:text/html\r\n', end='')
        print('Status: 200\r\n\r\n', end='')
        print('<html><body>', end='')
        print('<h2>Image saved successfully!</h2>', end='')
        print('</body></html>', end='')
    else:
        print('Content-Type:text/html\r\n', end='')
        print('Status: 200\r\n\r\n', end='')
        print('<html><body>', end='')
        print('<h2>No image file selected!</h2>', end='')
        print('</body></html>', end='')
else:
    print('Content-Type:text/html\r\n', end='')
    print('Status: 200\r\n\r\n', end='')
    print('<html><body>', end='')
    print('<h2>No image file found in the form data!</h2>', end='')
    print('</body></html>', end='')