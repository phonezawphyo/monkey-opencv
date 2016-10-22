const cv = require('../index');
var assert = require('assert');

describe('Smoke Matrix', function () {
  it('should construct matrix in different formats.', function (done) {
  	assert.ok(cv.Matrix);
  	assert.ok(new cv.Matrix);
  	assert.ok(new cv.Matrix(1, 2));
  	assert.ok(new cv.Matrix(1, 2, cv.Constants.CV_8U));
  	assert.ok(new cv.Matrix(1, 2, cv.Constants.CV_8U, [1]));
		done();
	});

  it('should have accessors working', function (done) {
  	var mat = new cv.Matrix(1, 2);
  	mat.set(0,0,3);
  	mat.set(0,1,5000);
  	assert.deepEqual(mat.row(0), [3,5000]);

  	mat = new cv.Matrix(1,2);
  	assert.equal(mat.set(0,0,3), undefined);
  	assert.equal(mat.get(0,0), 3);

  	mat = new cv.Matrix(6,7);
  	assert.equal(mat.width(), 7);

  	mat = new cv.Matrix(6,7);
  	assert.equal(mat.height(), 6);

  	mat = new cv.Matrix(6,7);
  	assert.deepEqual(mat.size(), [6, 7]);

  	mat = new cv.Matrix(6,7);
  	assert.equal(mat.width(), 7);
  	mat.resize(8,9);
  	assert.equal(mat.width(), 8);

  	mat = new cv.Matrix.Eye(4,4)
  	assert.deepEqual(mat.row(1), [0,1,0,0])
  	assert.deepEqual(mat.row(2), [0,0,1,0])

  	mat = new cv.Matrix.Eye(4,4);
  	assert.deepEqual(mat.col(1), [0,1,0,0])
  	assert.deepEqual(mat.col(2), [0,0,1,0])

  	assert.equal(new cv.Matrix().empty(), true);

  	mat = new cv.Matrix(1, 2, cv.Constants.CV_8UC3, [1, 2, 3]);
  	assert.deepEqual(mat.pixelRow(0), [1, 2, 3, 1, 2, 3]);

		done();
	});

	it('should have functions working', function(done) {
	  // convertTo
	  var mat = new cv.Matrix(75, 75, cv.Constants.CV_32F, [2.0]);
	  var matNew = new cv.Matrix(75, 75, cv.Constants.CV_8U);
	  mat.convertTo(matNew, cv.Constants.CV_8U, 2, 1);
	  assert.equal(matNew.pixel(0, 0), 5);
	
	  // reshape
	  mat = new cv.Matrix(75, 75, cv.Constants.CV_8UC1);
	  matNew = mat.reshape(1, 1);
	  assert.equal(matNew.height(), 1);
	  assert.equal(matNew.width(), 5625);
	
	  // GetRotationMatrix2D
	  mat = cv.Matrix.getRotationMatrix2D(0, 0, 90, 1.0);
	  assert.deepEqual(mat.size(), [2,3], 'GetRotationMatrix2D');
	
		done();
	});

	it("should have .norm working", function(done){
		cv.readImage("spec/fixtures/coin1.jpg", function(err, im) {
			cv.readImage("spec/fixtures/coin2.jpg", function(err, im2){
				assert.ok(im);
				assert.ok(im2);

				var errorL2 = im.norm(im2, cv.Constants.NORM_L2);
				assert.ok(errorL2 > 7200.0);
				errorL2 = im.norm(im, cv.Constants.NORM_L2);
				assert.equal(errorL2, 0);
				done();
			});
		});
	});

	it("should have .bitwiseXor working", function(done){
	  var mat1 = new cv.Matrix(1,1);
	  mat1.set(0,0, 1);
	
	  var mat2 = new cv.Matrix(1,1);
	  mat2.set(0,0, 1);
	
	  var xored = new cv.Matrix(1,1);
	  xored.bitwiseXor(mat1, mat2);
	
	  assert.equal(xored.get(0,0), 0);
	
		done();
	});

	it("should have .absDiff and .countNonZero working", function(done){
		cv.readImage("spec/fixtures/coin1.jpg", function(err, im) {
			cv.readImage("spec/fixtures/coin1.jpg", function(err, im2){
				assert.ok(im);
				assert.ok(im2);

				var diff = new cv.Matrix(im.width(), im.height());
				diff.absDiff(im, im2);

				diff.convertGrayscale();
				assert.equal(diff.countNonZero(), 0);
				done();
			});
		});
	});

	it('should have subtract working', function(done) {
		var a = new cv.Matrix.Zeros(1,1);
		a.set(0, 0, 3);
		var b = new cv.Matrix.Zeros(1,1);
		b.set(0, 0, 1);
		a.subtract(b);
		assert.deepEqual(a.get(0, 0), 2);
		done();
	});

	it('should have mean working', function(done) {
		var a = new cv.Matrix.Zeros(2, 2, cv.Constants.CV_8UC3);

		// Set [0, 0] element to 1 for all three channels
		a.set(0, 0, 1, 0);
		a.set(0, 0, 1, 1);
		a.set(0, 0, 1, 2);

		var means = a.mean();
		assert.deepEqual(means, [0.25, 0.25, 0.25, 0]);
		done();
	});

	it('should have .matchTemplateByMatrix working', function(done) {
		var targetFilename = "spec/fixtures/car1.jpg";
		var templateFilename = "spec/fixtures/car1_template.jpg";
		cv.readImage(targetFilename, function(err, target){
			cv.readImage(templateFilename, function(err, template){
				var TM_CCORR_NORMED = 3;
				var res = target.matchTemplateByMatrix(template, TM_CCORR_NORMED);
				var minMax = res.minMaxLoc();
				var topLeft = minMax.maxLoc;
				assert.ok(topLeft, "RGB Found Match");
				assert.equal(topLeft.x, 42, "match location x === 42");
				assert.equal(topLeft.y, 263, "match location y === 263");
				target.canny(5,300);
				template.canny(5,300);
				res = target.matchTemplateByMatrix(template, TM_CCORR_NORMED);
				minMax = res.minMaxLoc();
				topLeft = minMax.maxLoc;
				assert.ok(topLeft, "Canny edge Found Match");
				assert.equal(topLeft.x, 42, "match location x === 42");
				assert.equal(topLeft.y, 263, "match location y === 263");
				done();
			});
		})
	});
});
