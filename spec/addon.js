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

	it('free test', function(done) {
		var targetFilename = "spec/fixtures/screen.png";
		var templateFilename = "spec/fixtures/g.png";
		cv.readImage(targetFilename, function(err, target){
			cv.readImage(templateFilename, function(err, template){
				var TM_CCORR_NORMED = 1;
				//var res = target.matchTemplateByMatrix(template, TM_CCORR_NORMED);
        //var matches = res.templateMatches(0.99,1.0, 10, true, 10, 10);
        //console.log(target, template, res);
        //console.log(matches);
				//var res = target.matchTemplate(templateFilename, TM_CCORR_NORMED);
        //console.log(res);
				done();
			});
		})
	});
});
