# Base image
FROM alpine:3.16

# Install necessary packages
RUN apk add --no-cache \
    dumb-init \
    php-fpm

# Create directories for php files
RUN mkdir -p /var/www/html/php /var/www/html/php/.media

# Copy index.php file
COPY ./asset/index.php /var/www/html/php/

# Copy php-fpm configuration file
COPY ./conf/www.conf /etc/php8/php-fpm.d/

RUN chmod +x /usr/bin/dumb-init
COPY ./tools/entrypoint.sh /entrypoint.sh

# Expose port 9000 for php-fpm
EXPOSE 9000

# Start dumb-init for PID 1
ENTRYPOINT ["/usr/bin/dumb-init", "--"]

# Start php-fpm in the foreground
CMD ["/bin/sh", "entrypoint.sh"]
