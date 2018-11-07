var git = require(__dirname + '/git'),
    Step = require('step'),
    fs = require('fs-extra'),
    path = require('path'),
    exec = require('child_process').exec;

process.on('message', function(payload) {
    Step(
        function checkExistingHEX() {
            fs.exists(payload.hexFile+'.gz', this);
        },
        function checkSrcPath(exists) {
            if (exists) {
                process.send({msg: 'Firmware already built on prev task, fin'});
                process.send('next');
                return;
            }
            fs.exists(payload.path + '/.git', this);
        },
        function checkDir(exists) {
            if (!exists) {
                process.send({msg: 'Creating build root: ' + payload.path});
                fs.mkdir(payload.path, this);
            } else {
                return 'exists';
            }
        },
        function checkCommit(error, status) {
            if (status === 'exists') {
                process.send({msg: 'git fetch '+payload.config.version['src-repository']});
                git.fetch(payload.config.version['src-repository'], payload.path, this);
            } else if (error === null) {
                process.send({msg: 'git clone '+payload.config.version['src-repository']});
                git.clone(payload.config.version['src-repository'], payload.path, this);
            } else {
                process.send({msg: 'Error with creating the build directory: ' + error});
                process.send('next');
                return;
            }
        },
        function cloned(status) {
            git.checkout(payload.config.version['src-version'], payload.path, this);
        },
        function prepareMakeFile(status) {
            var makeConfig = '#Config\n' +
            'BOARD = mega2560\n' +
            'HAL_BOARD ?= HAL_BOARD_MPNG\n' +
            'PORT = /dev/ttyACM0\n' +
            'PX4_ROOT=../PX4Firmware\n'+
						'NUTTX_SRC=../PX4NuttX/nuttx\n';
            for (var name in payload.config) {
                if (payload.config[name]['src-flags']) {
                    makeConfig += '#' + name + '\n';
                    makeConfig += 'EXTRAFLAGS += -D' + payload.config[name]['src-flags'] +'\n';
                }
            }
            if (payload.config.version['make'] === 'mpng') {
	            makeConfig += 'EXTRAFLAGS += -DTHISFIRMWARE="\\"' + payload.config.version['src-dir'] + ' ' + payload.config.version['number'] + ' (' + payload.commit.substr(0, 7) + ')\\""\n';
            	makeConfig += 'BUILDROOT = ' + payload.path + '/_build' + '\n';
            }
            
            fs.writeFile(payload.path + '/config.mk', makeConfig, this);
        },
        function build(status) {
            process.send({msg: 'Build: '+payload.path + '/' + payload.config.version['src-dir']});
            exec('cd ' + payload.path + '/' + payload.config.version['src-dir'] + '; make '+payload.config.version['make']+' > compile.log 2>&1', this);
        },
        function copyHex(error, stdout, stderror) {
            process.send({msg: 'Copy HEX'});
            var srcHex = '';
            if (payload.config.version['make'] === 'f4by') {
            	srcHex = payload.path + '/../PX4Firmware/Images/f4by_APM.px4';
            } else {
            	srcHex = payload.path + '/_build/' + payload.config.version['src-dir'] + '.hex';
            }
            var dstHex = payload.hexFile;
            process.send({msg: 'Copy HEX from:'+srcHex +'  TO:'+dstHex});
            fs.copy(srcHex, dstHex, this);
        },
        function removeHexFile(status) {
            var srcHex = payload.path + '/_build/' + payload.config.version['src-dir'] + '.hex';
            fs.remove(srcHex, this);
        },
        function removeBuildDir(status) {
            fs.remove(payload.path + '/_build/', this);
        },
        function createMd5Hash(status) {
            var hexPath = path.dirname(payload.hexFile),
                hexName = path.basename(payload.hexFile);
            exec('cd ' + hexPath + ';md5sum -b ' + hexName + ' > ' + hexName + '.md5', this);
        },
        function createGz(status) {
            process.send({msg: 'Compress HEX: '+payload.hexFile});
            var hexPath = path.dirname(payload.hexFile),
                hexName = path.basename(payload.hexFile);
            exec('cd ' + hexPath + ';gzip ' + hexName, this);
        },
        function fin(status) {
            process.send('next');
        }
    );
});

process.send('next');
