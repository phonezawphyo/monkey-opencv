const constants = require('../../index.js').Constants;
const MonkeyOpencv = require('../../index.js').MonkeyOpencv;
var assert = require('assert');
var fs = require('fs');

describe('Smoke MonkeyOpencv.findSubImage', function () {
  it('should find single subimage (callback)', function (done) {
    MonkeyOpencv.findSubImage({
      source: 'spec/fixtures/screen.png',
      templates: ['spec/fixtures/g.png'],
      matchPercent: 70,
      maximumMatches: 1,
      downPyramids: 1,
      searchExpansion: 15,
    }, function(matches){
      assert.deepEqual(matches,[{
        position: { x: 78, y: 610 },
        rect: { x: 36, y: 568, width: 85, height: 84 },
        confidence: 99,
        templateIndex: 0
      }]);

      done();
    })
  });

  it('should find two subimages, not find one invalid image (promise)', function () {
    return MonkeyOpencv.findSubImage({
      source: 'spec/fixtures/screen.png',
      templates: [
        'spec/fixtures/g.png',
        'spec/fixtures/g2.png',
        'spec/fixtures/invalid.png',
      ],
      matchPercent: 90,
      maximumMatches: 1,
      downPyramids: 3,
      searchExpansion: 5,
      method: constants.TM_SQDIFF_NORMED,
    })
    .then(function(matches){
      assert.deepEqual(matches,[{
        position: { x: 78, y: 610 },
        rect: { x: 36, y: 568, width: 85, height: 84 },
        confidence: 99,
        templateIndex: 0
      }, {
        position: { x: 146, y: 314},
        rect: { x: 110, y: 278, width: 72, height: 73 },
        confidence: 99,
        templateIndex: 1
      }]);
    });
  });

  it('should find car template (from String filename)', function (done) {
    MonkeyOpencv.findSubImage({
      source: 'spec/fixtures/car1.jpg',
      templates: ['spec/fixtures/car1_template.jpg'],
      matchPercent: 99,
      maximumMatches: 1,
      downPyramids: 4,
      searchExpansion: 15,
    }, function(matches){
      assert.deepEqual(matches,[{
        position: { x: 195, y: 370 },
        rect: { x: 42, y: 263, width: 307, height: 214 },
        confidence: 99,
        templateIndex: 0
      }]);

      done();
    })
  });

  it('should find car template (from Buffer and Matrix)', function (done) {
    var img = fs.readFileSync('spec/fixtures/car1.jpg');

    MonkeyOpencv.readImage('spec/fixtures/car1_template.jpg', function(err, tmpl1) {
      MonkeyOpencv.findSubImage({
        source: img,
        templates: [tmpl1],
        matchPercent: 99,
        maximumMatches: 1,
        downPyramids: 4,
        searchExpansion: 15,
      }, function(matches){
        assert.deepEqual(matches,[{
          position: { x: 195, y: 370 },
          rect: { x: 42, y: 263, width: 307, height: 214 },
          confidence: 99,
          templateIndex: 0
        }]);

        done();
      });
    });
  });
});
