MegaPirateNG - FlashTool and Build-Server
=========================================

FlashTool
---------

You need Qt 5.1 SDK to compile.
Should compile on Windows, MacOSX and Linux, but currently only tested on Windows (and avrdude.exe is currently hardcoded)
```FLASHTOOL_PATH_URI``` needs to be changed to correct build server url.

Build-Server
------------

The Build-Server is written in JavaScript and uses NodeJS to run. You need a recent NodeJS.
Use ```npm install``` to install all NodeJS dependencies. 
The server also needs some tools installed:
* arduino ide
* gcc
* md5sum
* make
* git
* gzip

You need to alter the ```update.xml``` in the ```public``` sub-directory. (set ```<settings hexurl="http://tobedefined:8888/hex"/>```)

Use ```node app.js``` to start the server. The server will listen on port 8888, the server should not be run as root.

