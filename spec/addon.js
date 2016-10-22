var cv = require('../index.js');
var assert = require('assert');

describe('Smoke Addon', function () {
  it('should import nicely', function (done) {
    cv = require('../index')
    assert.ok(cv, "imported fine")
    assert.ok(cv.version, "version is there:" + cv.version)
    assert.ok(cv.Matrix, "matrix is there")
    done();
  });

  it('should import multiple times', function(done){
    var cv1 = require('../index')
      , cv2 = require('../index');

    cv1.readImage('./fixtures/screen.png', function(err, im){
      assert.ifError(err);
      cv2.readImage('./fixtures/screen.png', function(err, im){
        assert.ifError(err);
        done();
      });
    });
  });

});
