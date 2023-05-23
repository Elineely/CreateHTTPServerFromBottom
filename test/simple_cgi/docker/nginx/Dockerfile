# Base image
FROM alpine:3.16

# Install nginx, dumb-init, openssl
RUN apk add --no-cache nginx dumb-init

# Copy Nginx configuration file
COPY conf/my.conf /etc/nginx/http.d

# Expose port 80
EXPOSE 80

# Start dumb-init for PID 1
ENTRYPOINT ["/usr/bin/dumb-init", "--"]

# Start Nginx in the foreground
CMD ["nginx", "-g", "daemon off;"]
