#!/bin/bash

export PATH=/bin:/usr/bin:/sbin:/usr/sbin:/usr/local/bin

OSX_RELEASE=`sw_vers -productVersion | cut -d . -f 1,2`

cd "/Library/Application Support/OxFold/BigFileTool"

chown -R $USER:staff myrouter
chown -R $USER:staff client

ln -f -s "/Library/Application Support/OxFold/BigFileTool" /usr/local/bigfiletool

exit 0
