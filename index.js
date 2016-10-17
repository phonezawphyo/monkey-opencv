const testme = require('bindings')('testme.node');
const pngparse = require('pngparse');
pngparse.parseFile('screen.png', function (error, image) {
  pngparse.parseFile('g.png', function (error, template) {
    testme.findSubImage({
      source: 'screen.png',
      template: 'g.png',
      matchPercent: 70,
      maximumMatches: 1,
      downPyramids: 1,
      searchExpansion: 15,
    }, function(matches){
      console.log('search complete: ',matches);
    })
    console.log('Ended');
  });
})
