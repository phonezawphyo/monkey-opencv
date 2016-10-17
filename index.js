const search = require('bindings')('fast_template_matcher.node');

search.findSubImage({
  source: 'spec/fixtures/screen.png',
  template: 'spec/fixtures/g.png',
  matchPercent: 70,
  maximumMatches: 1,
  downPyramids: 1,
  searchExpansion: 15,
}, function(matches){
  console.log('Results async: ',matches);
})


var resultsSync = search.findSubImage({
  source: 'spec/fixtures/screen.png',
  template: 'spec/fixtures/g.png',
  matchPercent: 70,
  maximumMatches: 1,
  downPyramids: 1,
  searchExpansion: 15,
})
console.log('Results sync: ', resultsSync);

console.log('Ended');
