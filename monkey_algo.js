var binary = require('node-pre-gyp');
var path = require('path');
var binding_path = binary.find(path.resolve(path.join(__dirname,'./package.json')));
var async = require('async');
var bindings = require(binding_path);

var Matrix = bindings.Matrix;
var Constants = bindings.Constants;

var initOptions = function(options) {
  if (typeof options.source === 'undefined') {
    throw new TypeError('source (String, Matrix or Buffer expected)');
  }

  if (typeof options.matchPercent === 'undefined') {
    options.matchPercent = 70;
  }

  if (typeof options.maximumMatches === 'undefined') {
    options.maximumMatches = 1;
  }

  if (typeof options.downPyramids === 'undefined') {
    options.downPyramids = 0;
  }

  if (typeof options.searchExpansion === 'undefined') {
    options.searchExpansion = 10;
  }

  if (typeof options.method === 'undefined') {
    options.method = Constants.TM_SQDIFF_NORMED;
  }

  [
    'matchPercent',
    'maximumMatches',
    'downPyramids',
    'searchExpansion',
    'method',
  ]
  .forEach(function(key) {
    if (typeof options[key] !== 'number') {
      throw new TypeError(`${key} (Number expected)`);
    }
  });

  if (options.matchPercent < 0 || options.matchPercent > 100) {
    throw new TypeError('matchPercent (0 <= n <= 100)');
  }

  if (options.maximumMatches <= 0) {
    throw new TypeError('maximumMatches (n > 0)');
  }

  if (options.downPyramids < 0) {
    throw new TypeError('downPyramids (n >= 0)');
  }

  if (options.searchExpansion < 0) {
    throw new TypeError('searchExpansion (n >= 0)');
  }

  if (options.method < 0 || options.method > 5) {
    throw new TypeError('method (0 <= n <= 5)');
  }
};

var readIfNeeded = function(o, key, next) {
  if (typeof o === 'string') {
    if (o.length > 0) {
      bindings.readImage(o, function(err, sourceMat){
        next(null, sourceMat);
      });
    } else {
      throw new TypeError(`${key} (String, Matrix or Buffer expected)`);
    }
  } else if (o instanceof Buffer) {
    bindings.readImage(o, function(err, sourceMat){
      next(null, sourceMat);
    });
  } else if (o instanceof Matrix) {
    next(null, o);
  } else {
    throw new TypeError(`${key} (String, Matrix or Buffer expected)`);
  }
};

var executeFindSubImage = function(options, fn) {
  async.parallel({
    source: function(next) {
      readIfNeeded(options.source, 'source', function(err, source) {
        options.source = source;
        next(null, null);
      });
    },
    templates: function(next) {
      if (options.templates instanceof Array) {
        async.map(options.templates, function(o, next) {
          readIfNeeded(o, 'templates[]', next);
        }, function(err, templates) {
          options.templates = templates;
          options.template = templates[0];
          next(null, null);
        });
      } else {
        throw new TypeError('templates: Array expected.');
      }
    }
  }, function(err, results) {

    initOptions(options);

    bindings.findSubImage(
      options.source,
      options.templates,
      options.matchPercent,
      options.maximumMatches,
      options.downPyramids,
      options.searchExpansion,
      options.method,
      fn);
  });
};

module.exports = {
  readImage: bindings.readImage,

  findSubImage: function(options, fn) {
    if (typeof fn === 'function') {
      executeFindSubImage(options, fn);
    } else {
      return new Promise(function(resolve, reject) {
        executeFindSubImage(options, function(matches) {
          resolve(matches);
        });
      });
    }
  },
};
