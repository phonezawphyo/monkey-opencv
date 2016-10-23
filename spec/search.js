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
        confidence: 99,
        templateIndex: 0
      }]);

      done();
    })

  });

  it('should find single subimage (promise)', function () {
    return search.findSubImage({
      source: 'spec/fixtures/screen.png',
      templates: [
        'spec/fixtures/g.png',
        'spec/fixtures/g.png',
      ],
      matchPercent: 70,
      maximumMatches: 1,
      downPyramids: 1,
      searchExpansion: 15,
      method: constants.TM_SQDIFF_NORMED,
    })
    .then(function(matches){
      assert.deepEqual(matches,[{
        position: { x: 78, y: 610 },
        confidence: 99,
        templateIndex: 0
      }, {
        position: { x: 78, y: 610 },
        confidence: 99,
        templateIndex: 1
      }]);
    });
  });
});
