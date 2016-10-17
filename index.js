const testme = require('bindings')('testme.node');
const pngparse = require('pngparse');
pngparse.parseFile('screen.png', function (error, image) {
  pngparse.parseFile('g.png', function (error, template) {
    testme.findSubImg({
      source: 'screen.png',
      template: 'g.png',
      matchPercent: 80,
      maximumMatches: 1,
      downPyramids: 1,
      searchExpansion: 60,
      oncomplete: function(matches){
        console.log('search complete: ',matches);
      },
    });
  });
})
