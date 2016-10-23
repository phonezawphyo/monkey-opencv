const MonkeyOpencv = require('../index.js').MonkeyOpencv;
var assert = require('assert');

describe('Smoke MonkeyOpencv', function () {
  it('should readImage', function (done) {
    MonkeyOpencv.readImage('spec/fixtures/screen.g', function(err, im) {
      assert.ifError(err);
      assert.ok(im);
      done();
    });
  });

  it('should findSubImage', function (done) {
    assert.ok(MonkeyOpencv.findSubImage);
    assert.ok(typeof MonkeyOpencv.findSubImage === 'function');
    done();
  });
});

//require('./spec/monkey_opencv/find_sub_image.js');
