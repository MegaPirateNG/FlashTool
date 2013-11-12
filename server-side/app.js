var builder = require('./lib/builder'),
    server = require('./lib/server'),
    winston = require('winston'),
    logger = new (winston.Logger)({
        transports: [
            new winston.transports.Console(),
            new winston.transports.File({ filename: __dirname + '/build-server.log' })
        ],
        exceptionHandlers: [
            new winston.transports.File({ filename: __dirname + '/build-server-errors.log' })
        ]
    }),
    configFile = __dirname + '/public/update.xml',
    publicPath = __dirname + '/public';

GLOBAL.logger = logger;

builder.init(configFile, publicPath);
server.init(publicPath, builder);