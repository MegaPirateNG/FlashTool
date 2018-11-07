var express = require('express'),
    xml2js = require('xml2js'),
    parser = new xml2js.Parser({explicitArray: false, mergeAttrs: true, explicitRoot: false});

var xmlBodyParser = function(req, res, next) {
    if (req._body) return next();
    req.body = req.body || {};

    // ignore GET
    if ('GET' == req.method || 'HEAD' == req.method) return next();

    // check Content-Type
    if ('text/xml' != req.header('content-type')) return next();

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
        logger.info('Client requests hex ' + req.headers['user-agent']);
        builder.handleBuildJob(req, function(status, hexfile) {
            if (status) {
                var body = '<xml><firmware>' + hexfile + '</firmware></xml>';
                res.setHeader('Content-Type', 'text/xml');
                res.setHeader('Content-Length', body.length);
                res.end(body);
            } else {
                var body = '<xml><error>' + hexfile + '</error></xml>';
                res.setHeader('Content-Type', 'text/xml');
                res.setHeader('Content-Length', body.length);
                res.send(500, body);
            }
        });
    });

    app.use(express.static(publicPath));

    app.listen(8888);
    logger.info('Server started');
};