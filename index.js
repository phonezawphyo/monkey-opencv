var binary = require('node-pre-gyp');
var path = require('path');
var Stream = require('stream').Stream;
var Buffers = require('buffers');
var util = require('util');
var binding_path = binary.find(path.resolve(path.join(__dirname,'./package.json')));
var async = require('async');
var bindings = require(binding_path);
var MonkeyOpencv = require('./monkey_opencv');

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

bindings.MonkeyOpencv = MonkeyOpencv;

module.exports = bindings;
