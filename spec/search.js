const search = require('bindings')('fast_template_matcher.node');
var assert = require('assert');

describe('Basic search', function () {
  it('should async find single subimage', function (done) {
    search.findSubImage({
      source: 'spec/fixtures/screen.png',
      template: 'spec/fixtures/g.png',
      matchPercent: 70,
      maximumMatches: 1,
      downPyramids: 1,
      searchExpansion: 15,
    }, function(matches){
      assert.deepEqual(matches,[{
        position: { x: 78, y: 610 },
        confidence: 99,
        imageIndex: 0
      }]);

      done();
    })

  });

  it('should sync find single subimage', function (done) {
    var matches = search.findSubImage({
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
