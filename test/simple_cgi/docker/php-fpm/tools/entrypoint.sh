#!/bin/sh

# Set ownership of WordPress files to www-data
adduser -D -S -G www-data www-data
chown -R www-data:www-data /var/www/html/php

# Execute PHP FastCGI in foreground
php-fpm8 -F
