#!/bin/sh

TARGET="http://localhost:80"

TARGET_ROOT="42_tester_dir"
TESTER_NAME="tester"
CGI_TESTER_NAME="cgi_tester"

YOUPI_BANENE="YoupiBanane"
DOCUMENT_ROOT="DocumentRoot"
PUT_TEST="PutTest"
POST_BODY="PostBody"

cd $TARGET_ROOT

if [ ! -f $TESTER_NAME ]; then
	echo "Please put \`$TESTER_NAME\` in \`$TARGET_ROOT\`" >&2
	exit 1
fi

if [ ! -f $CGI_TESTER_NAME ]; then
	echo "Please put \`$CGI_TESTER_NAME\` in \`$TARGET_ROOT\`" >&2
	exit 1
fi

chmod u+x $TESTER_NAME
chmod u+x $CGI_TESTER_NAME

# フォルダが存在しない場合も成功する
rm -rf $YOUPI_BANENE $DOCUMENT_ROOT $PUT_TEST $POST_BODY

mkdir $YOUPI_BANENE
mkdir $DOCUMENT_ROOT
mkdir $PUT_TEST
mkdir $POST_BODY

echo "youpi.bad_extension" > $YOUPI_BANENE/youpi.bad_extension
echo "youpi.bla" > $YOUPI_BANENE/youpi.bla
mkdir $YOUPI_BANENE/nop
echo "youpi.bad_extension" > $YOUPI_BANENE/nop/youpi.bad_extension
echo "other.pouic" > $YOUPI_BANENE/nop/other.pouic
mkdir $YOUPI_BANENE/Yeah
echo "not_happy.bad_extension" > $YOUPI_BANENE/Yeah/not_happy.bad_extension

echo "sample.txt" > $DOCUMENT_ROOT/sample.txt

./tester $TARGET
