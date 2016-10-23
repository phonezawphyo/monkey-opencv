var binary = require('node-pre-gyp');
var path = require('path');
var Stream = require('stream').Stream;
var Buffers = require('buffers');
var util = require('util');
var binding_path = binary.find(path.resolve(path.join(__dirname,'./package.json')));
var async = require('async');
var bindings = require(binding_path);

var Constants = bindings.Constants;

var Matrix = bindings.Matrix,
	ImageStream,
	ImageDataStream;

Matrix.prototype.inspect = function(){
  var size = (this.size()||[]).join('x');
  return "[ Matrix " + size + " ]";
}

ImageStream = bindings.ImageStream = function(){
  this.writable = true;
}
util.inherits(ImageStream, Stream);


ImageStream.prototype.write = function(buf){
  var self = this;
  bindings.readImage(buf, function(err, matrix){
    if (err) return self.emit('error', err);
    self.emit('data', matrix);
  });
}


ImageDataStream = bindings.ImageDataStream = function(){
  this.data = Buffers([]);
  this.writable = true;
}
util.inherits(ImageDataStream, Stream);


ImageDataStream.prototype.write = function(buf){
  this.data.push(buf);
  return true;
}


ImageDataStream.prototype.end = function(b){
  var self = this;
  if (b) ImageStream.prototype.write.call(this, b);

  var buf = this.data.toBuffer();
  bindings.readImage(buf, function(err, im){
    if (err) return self.emit('error', err);
    self.emit('load', im);
  });
}

var nativeFindSubImage = bindings.findSubImage;

bindings.MonkeyAlgo = {

  findSubImage: function(options, fn) {

    var readIfNeeded = function(o, key, next) {
      if (typeof o === 'string') {
        if (o.length > 0) {
          bindings.readImage(o, function(err, sourceMat){
            next(null, sourceMat);
          });
        } else {
          throw new TypeError(key);
        }
      } else if (typeof o == 'object') {
        next(null, o);
      } else {
        throw new TypeError(key);
      }
    };

    var execute = function(options, fn) {
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
              readIfNeeded(o, 'templates', next);
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

        if (typeof options.source === 'undefined') {
          throw 'Invalid argument: source';
        }
        /* 
         * - source: Source image url/Matrix
         * - templates: [Template image urls/Matrices]
         * - matchPercent: (70) int 0 ~ 100
         * - maximumMatches: (1) int >= 1 ,
         * - downPyramids: (1) int >= 1,
         * - searchExpansion: (15) int >= 1,
         * - method: 0~6
         */
        if (typeof options.method === 'undefined') {
          options.method = Constants.TM_SQDIFF_NORMED;
        }
        nativeFindSubImage(
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

    if (typeof fn === 'function') {
      execute(options, fn);
    } else {
      return new Promise(function(resolve, reject) {
        //try {
          execute(options, function(matches) {
            resolve(matches);
          });
        //} catch(e) {
        //    console.log('h3',e);
        //  reject(e);
        //}
      });
    }
  },
};

module.exports = bindings;
