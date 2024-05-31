#!/bin/sh

echo "Content-type: text/plain"
echo "Date: Wed, 22 May 2024 12:34:56 GMT"
echo ""
echo "Content-Type: $CONTENT_TYPE"
echo "Content-Length: $CONTENT_LENGTH"
echo "Gateway Interface: $GATEWAY_INTERFACE"
echo "Path Info: $PATH_INFO"
echo "Path Translated: $PATH_TRANSLATED"
echo "Query String: $QUERY_STRING"
echo "Request Method: $REQUEST_METHOD"
echo "Script Name: $SCRIPT_NAME"
echo "Remote Address: $REMOTE_ADDR"
echo "Remote Port: $REMOTE_PORT"
echo "Server Name: $SERVER_NAME"
echo "Server Port: $SERVER_PORT"
echo "Server Protocol: $SERVER_PROTOCOL"
echo "Server Software: $SERVER_SOFTWARE"

echo "Hello, World!"

echo

env | grep -i 'HTTP_' | sort

echo

cat
