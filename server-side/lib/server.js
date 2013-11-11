var express = require('express'),
    utils = require('express/node_modules/connect/lib/utils'),
    xml2js = require('xml2js'),
    parser = new xml2js.Parser({explicitArray: false, mergeAttrs: true, explicitRoot: false});

var xmlBodyParser = function(req, res, next) {
    if (req._body) return next();
    req.body = req.body || {};

    // ignore GET
    if ('GET' == req.method || 'HEAD' == req.method) return next();

    // check Content-Type
    if ('text/xml' != utils.mime(req)) return next();

    // flag as parsed
    req._body = true;

    // parse
    var buf = '';
    req.setEncoding('utf8');
    req.on('data', function(chunk){ buf += chunk });
    req.on('end', function() {
        parser.parseString(buf, function(err, json) {
            if (err) {
                err.status = 400;
                next(err);
            } else {
                req.body = json;
                next();
            }
        });
    });
};

exports.init = function(publicPath, builder) {
    app = express();
    app.use(xmlBodyParser);

    app.post('/hex', function(req, res) {
        console.log('Get hex');
        builder.handleBuildJob(req, function(hexfile) {
            var body = '<xml><firmware>' + hexfile + '</firmware></xml>';
            res.setHeader('Content-Type', 'text/xml');
            res.setHeader('Content-Length', body.length);
            res.end(body);
        });
    });

    app.use(express.static(publicPath));

    app.listen(8888);
    console.log('Server started');
};