#!/usr/bin/perl

# use strict;
# use warnings;

# Read the request method
my $request_method = $ENV{'REQUEST_METHOD'};
print "HTTP/1.1 200 OK\r\n";
print "Status: 200\r\n";

# Handle GET request
if ($request_method eq 'GET') {
    print "Content-Type: text/plain\r\n\r\n";
    print "This is a GET request.";
}

# Handle POST request
elsif ($request_method eq 'POST') {
    print "Content-Type: text/plain\r\n\r\n";
    print "This is a POST request.";
}
