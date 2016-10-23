const MonkeyAlgo = require('../index.js').MonkeyAlgo;
var assert = require('assert');

describe('Smoke MonkeyAlgo', function () {
  it('should readImage', function (done) {
    MonkeyAlgo.readImage('spec/fixtures/screen.g', function(err, im) {
      assert.ifError(err);
      assert.ok(im);
      done();
    });
  });

  it('should findSubImage', function (done) {
    assert.ok(MonkeyAlgo.findSubImage);
    assert.ok(typeof MonkeyAlgo.findSubImage === 'function');
    done();
  });
});
