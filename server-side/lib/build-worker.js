var git = require(__dirname + '/git'),
    Step = require('step'),
    fs = require('fs-extra'),
    path = require('path'),
    exec = require('child_process').exec;

process.on('message', function(payload) {
    Step(
        function checkCommit() {
            git.clone(payload.config.version['src-repository'], payload.path, this);
        },
        function cloned(status) {
            git.checkout(payload.config.version['src-version'], payload.path, this);
        },
        function prepareMakeFile(status) {
            var makeConfig = '#Config\n' +
            'BOARD = mega2560\n' +
            'HAL_BOARD ?= HAL_BOARD_MPNG\n' +
            'PORT = /dev/ttyACM0\n' +
            'BUILDROOT = ' + payload.path + '_build' + '\n' +
            'EXTRAFLAGS += -DTHISFIRMWARE="' + payload.config.version['src-dir'] + ' ' + payload.config.version['number'] + ' (' + payload.commit.substr(0, 7) + ')"\n';
            for (var name in payload.config) {
                if (payload.config[name]['src-flags']) {
                    makeConfig += '#' + name + '\n';
                    makeConfig += 'EXTRAFLAGS += -D' + payload.config[name]['src-flags'] +'\n';
                }
            }
            fs.writeFile(payload.path + '/config.mk', makeConfig, this);
        },
        function build(status) {
            exec('cd ' + payload.path + '/' + payload.config.version['src-dir'] + ';make', this);
        },
        function copyHex(error, stdout, stderror) {
            var srcHex = payload.path + '_build/' + payload.config.version['src-dir'] + '.hex',
                dstHex = payload.hexFile;
            fs.copy(srcHex, dstHex, this);
        },
        function removeHexFile(status) {
            var srcHex = payload.path + '_build/' + payload.config.version['src-dir'] + '.hex';
            fs.remove(srcHex, this);
        },
        function removeBuildDir(status) {
            fs.remove(payload.path + '_build/', this);
        },
        function removeSrcDir(status) {
            fs.remove(payload.path, this);
        },
        function createMd5Hash(status) {
            var hexPath = path.dirname(payload.hexFile),
                hexName = path.basename(payload.hexFile);
            exec('cd ' + hexPath + ';md5sum -b ' + hexName + ' > ' + hexName + '.md5', this);
        },
        function fin(status) {
            process.send('next');
        }
    );
});

process.send('next');
