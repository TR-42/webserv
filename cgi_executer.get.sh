#!/bin/sh

NUM1=10
NUM2=20

# This script is used to execute the cgi script
export GATEWAY_INTERFACE="CGI/1.1"
export PATH_INFO="$(pwd)/resources/php-cgi/inline.php"
export PATH_TRANSLATED="$(pwd)/resources/php-cgi/inline.php"
export QUERY_STRING="num1=$NUM1&num2=$NUM2"
export REQUEST_METHOD="GET"
export SCRIPT_NAME="$(pwd)/resources/php-cgi/inline.php"
export REMOTE_ADDR="127.0.0.1"
export REMOTE_PORT="12345"
export SERVER_NAME="localhost"
export SERVER_PORT="80"
export SERVER_PROTOCOL="HTTP/1.1"
export SERVER_SOFTWARE="webserv 0.0.1"

export REDIRECT_STATUS=1

echo "executing: php-cgi $SCRIPT_NAME" >&2

# Execute the cgi script
php-cgi $SCRIPT_NAME
