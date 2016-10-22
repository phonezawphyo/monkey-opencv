const bindings = require('../index.js');
var assert = require('assert');

describe('Smoke Matrix', function () {
  it('should construct matrix in different formats.', function (done) {
  	assert.ok(bindings.Matrix);
  	assert.ok(new bindings.Matrix);
  	assert.ok(new bindings.Matrix(1, 2));
  	assert.ok(new bindings.Matrix(1, 2, bindings.Constants.CV_8U));
  	assert.ok(new bindings.Matrix(1, 2, bindings.Constants.CV_8U, [1]));
		done();
	});

  it('should sync find single subimage', function (done) {
    var matches = bindings.findSubImage({
      source: 'spec/fixtures/screen.png',
      template: 'spec/fixtures/g.png',
      matchPercent: 70,
      maximumMatches: 1,
      downPyramids: 1,
      searchExpansion: 15,
    });

    assert.deepEqual(matches,[{
      position: { x: 78, y: 610 },
      confidence: 99,
      imageIndex: 0
    }]);

    done();
  });
});
