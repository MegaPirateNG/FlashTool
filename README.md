MegaPirateNG - FlashTool and Build-Server
=========================================

There is currently no official server available, since the project is from 2013 and the demand is not that high.
But you can compile flashtool yourself and build and run the server part within docker quite easily.

FlashTool
---------

You need Qt 5.1 SDK or higher to compile.
Does compile on Windows, MacOSX and Linux, but currently only tested on Windows and MacOSX (and avrdude.exe is currently hardcoded but a Windows and MacOSX Version is provided)
```FLASHTOOL_PATH_URI``` needs to be changed to correct build server url.

Build-Server
------------

#### Directly on your system. (ubuntu linux recommended)

The Build-Server is written in JavaScript and uses NodeJS to run. You need a recent NodeJS.
Use ```npm install``` to install all NodeJS dependencies. 
The server also needs some tools installed:
* arduino ide
* gcc
* md5sum
* make
* git
* gzip

You need to alter the ```update.xml``` in the ```public``` sub-directory. (set ```<settings hexurl="http://127.0.0.1:8888/hex"/>```)
Also there are some local paths you probably need to alter there too. (within ```<versions>```)

Use ```node app.js``` to start the server. The server will listen on port 8888, the server should not be run as root.

#### You also can build and use a docker container.

Alter update.xml if you want it externally available (other than 127.0.0.1)

    # build docker container
    docker build -t flashtool-server .

    # Run docker container
    docker run --rm -p 8888:8888 -it --name flashtool flashtool-server

