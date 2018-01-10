FROM ubuntu:16.04

RUN  useradd -ms /bin/bash ubuntu \
    && apt-get -qq update \
    # && apt-get -qq install libcurl4-openssl-dev \
    # && apt-get -qq install curl \
    && apt-get -qq install nginx \
    # && apt-get -qq install python3 \
    && apt-get -qq install php7.0-fpm \
    && apt-get -qq install php-xml \
    && apt-get -qq install vim \
    && apt-get -qq install php-pear \
    && mkdir -p /home/ubuntu/phpsession \
    && mkdir -p /home/ubuntu/php \
    && mkdir -p /var/run/php \
    && sed -i 's/www-data/ubuntu/g' /etc/nginx/nginx.conf

# COPY roles/gateway/files/config/kalayserver.pem /etc/kalay/certs/ 
COPY php/php.ini /etc/php/7.0/fpm/php.ini
COPY php/www.conf /etc/php/7.0/fpm/pool.d/www.conf
COPY default /etc/nginx/sites-available/default

COPY php/info.php /home/ubuntu/php/info.php

EXPOSE 8080
# CMD service php7.0-fpm restart \
#     && nginx -g 'daemon off;'
# USER ubuntu