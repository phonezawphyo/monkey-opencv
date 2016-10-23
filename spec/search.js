const constants = require('../index.js').Constants;
const search = require('../index.js').MonkeyAlgo;
var assert = require('assert');

describe('Smoke FindSubImage', function () {
  it('should find single subimage (callback)', function (done) {
    search.findSubImage({
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
    return search.findSubImage({
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
});
