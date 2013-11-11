var builder = require('./lib/builder'),
    server = require('./lib/server'),
    configFile = __dirname + '/public/update.xml',
    publicPath = __dirname + '/public';

builder.init(configFile, publicPath);
server.init(publicPath, builder);