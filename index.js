var binary = require('node-pre-gyp');
var path = require('path');
var binding_path = binary.find(path.resolve(path.join(__dirname,'./package.json')));
var bindings = require(binding_path);

var Matrix = bindings.Matrix;

Matrix.prototype.inspect = function(){
  var size = (this.size()||[]).join('x');
  return "[ Matrix " + size + " ]";
}

module.exports = bindings;


//module.exports = {
//  findSubImage: function(options, next) {
//    if (typeof next === 'function') {
//      binding.findSubImage(options, next);
//    } else {
//      return binding.findSubImage(options);
//    }
//  },
//};
