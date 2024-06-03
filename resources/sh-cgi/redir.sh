#!/bin/sh

# queryの `path` に対応するファイルにリダイレクトする

if [ -z "$QUERY_STRING" ]; then
	echo "Status: 400 Bad Request"
	echo "Content-Type: text/plain"
	echo
	echo "Bad Request"
	exit 1
fi

path=$(echo "$QUERY_STRING" | sed -n 's/^path=\([^&]*\).*/\1/p')

echo "Location: $path"
echo
