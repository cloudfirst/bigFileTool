#!/bin/bash

export PATH=/bin:/usr/bin:/sbin:/usr/sbin

if [ "$UID" -ne 0 ]; then
	echo "Must be run as root; try: sudo $0"
	exit 1
fi

cd /

echo 'Stopping any running OxFold BigFileToll ...'
pkill bigfiletest

echo 'Stopping any running oxfold-webtool ...'
pkill oxfold-webtool

echo "Removing OxFold BigFileTool files..."
rm -rf '/Applications/bigfiletool.app'
echo "remove bigfiletool.app"

cd '/Library/Application Support/OxFold/BigFileTool'
if [ "`pwd`" = '/Library/Application Support/OxFold/BigFileTool' ]; then
	rm -rf *.sh *.dylib oxfold-webtool
fi

echo 'Uninstall complete.'
echo
echo 'You can delete this folder and its contents if you do not intend to re-use'
echo '  /Library/Application Support/OxFold/BigFileTool'
echo

exit 0
