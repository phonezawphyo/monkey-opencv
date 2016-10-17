const testme = require('bindings')('fast_template_matcher.node');

testme.findSubImage({
  source: 'images/screen.png',
  template: 'images/g.png',
  matchPercent: 70,
  maximumMatches: 1,
  downPyramids: 1,
  searchExpansion: 15,
}, function(matches){
  console.log('Results async: ',matches);
})


var resultsSync = testme.findSubImage({
  source: 'images/screen.png',
  template: 'images/g.png',
  matchPercent: 70,
  maximumMatches: 1,
  downPyramids: 1,
  searchExpansion: 15,
})
console.log('Results sync: ', resultsSync);

console.log('Ended');
