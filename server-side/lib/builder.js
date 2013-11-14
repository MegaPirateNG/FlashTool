var Queue  = require('forkqueue'),
    xml2js = require('xml2js'),
    git = require(__dirname + '/git'),
    Step = require('step'),
    parser = new xml2js.Parser({explicitArray: false, mergeAttrs: true, explicitRoot: false}),
    fs = require('fs'),
    crypto = require('crypto'),
    queue = new Queue(2,__dirname + '/build-worker'),
    os = require('os'),
    configData = {},
    hexFilePath = '';

var findConfigElementById = function(config, id) {
    for (var i = 0; i < config.length; i++) {
        if (config[i].id == id) {
            return config[i];
        }
    }
    throw new Error('Config id invalid!')
};

exports.init = function(configFile, publicPath) {
    hexFilePath = publicPath + '/hex/';
    fs.readFile(configFile, function(err, data) {
        parser.parseString(data, function (err, result) {
            configData = result;
        });
    });
};

exports.handleBuildJob = function(req, callback) {
    var buildConfig = {
        board : {},
        rcinput : {},
        rcmapping : {},
        platform : {},
        version : {},
        gpstype : {},
        gpsbaud : {},
        };

    //Validate request
    if (!req.body.board || !req.body.rcinput || !req.body.rcmapping || !req.body.platform || !req.body.version || !req.body.gpstype || !req.body.gpsbaud) {
        callback(false, 'missing parameters');
        return false;
    }

    try {
        buildConfig.board = findConfigElementById(configData.boards.board, req.body.board);
        buildConfig.rcinput = findConfigElementById(configData.rcinputs.rcinput, req.body.rcinput);
        buildConfig.rcmapping = findConfigElementById(configData.rcinputs.rcmapping, req.body.rcmapping);
        buildConfig.platform = findConfigElementById(configData.platforms.platform, req.body.platform);
        buildConfig.version = findConfigElementById(configData.versions.version, req.body.version);
        buildConfig.gpstype = findConfigElementById(configData.gps.gpstype, req.body.gpstype);
        buildConfig.gpsbaud = findConfigElementById(configData.gps.gpsbaud, req.body.gpsbaud);
    } catch (e) {
        callback(false, 'invalid parameters');
        return false;
    }

    Step(
        function checkCommit() {
            git.latestCommit(buildConfig.version['src-repository'], buildConfig.version['src-version'], this);
        },
        function gotCommit(commit) {
            var configHash = crypto.createHash('md5').update(JSON.stringify(buildConfig)).digest("hex"),
                path = buildConfig.version['src-path'],
                hexFile = configHash + '_' + commit + '.hex';

            //Check if hexfile already exists
            fs.exists(hexFilePath + '/' + hexFile + '.gz', function(exists) {
		            if (!exists) {
		                logger.info('Need to build hex file for config: ' + JSON.stringify(buildConfig));
		                queue.enqueue({
		                    'config' : buildConfig,
		                    'commit' : commit,
		                    'hexFile' : hexFilePath + '/' + hexFile,
		                    'path' : path
		                });
		            }
            });

            callback(true, hexFile);
        }
    );
};
