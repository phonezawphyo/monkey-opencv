const testme = require('bindings')('testme.node');
const pngparse = require('pngparse');
pngparse.parseFile('screen.png', function (error, image) {
  pngparse.parseFile('g.png', function (error, template) {
    //console.log('image',image);
    //console.log('template',template);
    //console.log(testme.testMe());
    //console.log(testme.findSubImg({
    //  source: 'screen.png',
    //  template: 'g.png',
    //}));
    console.log(testme.findSubImg({
      source: 'screen.png',
      template: 'g.png',
      oncomplete: function(o,p){
        console.log('search complete',o,p);
      },
    }));
  });
})
