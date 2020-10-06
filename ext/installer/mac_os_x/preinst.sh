#!/bin/bash

export PATH=/bin:/usr/bin:/sbin:/usr/sbin

echo 'Stopping any running OxFold BigFileToll ...'
pkill bigfiletest

sleep 1

echo 'Stopping any running oxfold-webtool ...'
pkill oxfold-webtool

if [ -d "/Library/Application Support/OxFold/BigFileTool" ]; then
	cd "/Library/Application Support/OxFold/BigFileTool"
	if [ -f "oxfold-webtool" ]; then
		rm oxfold-webtool *.sh *.dylib
	fi
fi

sleep 1

cd "/Applications"
rm -rf "bigfiletool.app"

exit 0
