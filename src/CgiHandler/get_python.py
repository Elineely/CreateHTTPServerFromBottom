#!/usr/bin/python

# keep in mind that the CGI module in Python has been removed in Python 3,
# so you may need to use an alternative approach for handling CGI requests.

import cgi
import os
import re
import cgitb
cgitb.enable()

# Create a new CGI object
cgi_obj = cgi.FieldStorage()

# Get the values of the two numbers to add from the query string
# if query string case
# print(os.environ['QUERY_STRING'])
aaa.py
# else if post put case
num1 = cgi_obj.getvalue('num1')
num2 = cgi_obj.getvalue('num2')

# print("num1:", num1)
# print("num2:", num2)

# Convert the numbers to integers
if num1 is None or num2 is None:
    if 'QUERY_STRING' in os.environ:
        query = os.environ['QUERY_STRING']
        query_list = query.split('&')
        for item in query_list:
            var, val = item.split('=')
            val = val.replace("'", "")
            val = val.replace('+', ' ')
            val = re.sub(r'%(\w\w)', lambda m: chr(int(m.group(1), 16)), val)
            if var == "num1":
                if val != "":
                    num1 = int(val)
            elif var == "num2":
                if val != "":
                    num2 = int(val)

        if num1 is None or num2 is None:
            print("Content-Type: text/html")
            print()
            print("<html><head><title>Addition Result</title></head><body>")
            print("<center><h1>Addition Result</h1></center>")
            print("<p>num1 or num2 does not exist.</p>")
            print("</body></html>")
            exit()
        else:
            sum = num1 + num2

            print("Content-Type: text/html")
            print()
            print("<html><head><title>Addition Result</title></head><body>")
            print("<center><h1>Addition Result</h1></center>")
            print("<p>The sum of {} and {} is {}.</p>".format(num1, num2, sum))
            print("</body></html>")
            exit()
    else:
        print("Content-Type: text/html")
        print()
        print("<html><head><title>Addition Result</title></head><body>")
        print("<center><h1>Addition Result</h1></center>")
        print("<p>num1 and num2 does not exist.</p>")
        print("</body></html>")
        exit()
else:
    num1 = int(num1)
    num2 = int(num2)
    # Add the numbers
    sum = num1 + num2

    # Print the HTML response
    print("Content-Type: text/html")
    print()
    print("<html><head><title>Addition Result</title></head><body>")
    print("<center><h1>Addition Result</h1></center>")
    print("<p>The sum of {} and {} is {}.</p>".format(num1, num2, sum))
    print("</body></html>")