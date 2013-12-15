var exec = require('child_process').exec;

exports.latestCommit = function(repro, branch, callback) {
    exec('git ls-remote ' + repro, function(error, stdout, stderr) {
        var repoList = stdout.split('\n');
        for (var i = 0; i < repoList.length; i++) {
            var result = repoList[i].match(/(.*)\trefs\/heads\/(.*)/);
            if (result && result.length > 2 && result[2] == branch) {
                callback(result[1]);
                return;
            }
        }
        callback(null);
    });
};

exports.clone = function(repro, path, callback) {
    exec('git clone ' + repro + ' ' + path, function(error, stdout, stderr) {
        callback(stdout);
    });
};

exports.fetch = function(repro, path, callback) {
    exec('cd ' + path + '; git fetch', function(error, stdout, stderr) {
        callback(stdout);
    });
};

exports.checkout = function(branch, path, callback) {
    exec('cd ' + path + ';git checkout -f ' + branch, function(error, stdout, stderr) {
        callback(stdout);
    });
};
