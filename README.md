# MonkeyOpencv

Provides methods built upon opencv.

* MonkeyOpencv (Main module)
* Matrix (Wrapper for opencv cv::Mat)
* Constants (Wrapper for opencv constrants)
* ImageStream (Wrapper to create cv::Mat from Stream)
* ImageDataStream (DataStream wrapper)

## MonkeyOpencv
### readImage(imagePath, callback)

> Wrapper for `cv::imread`.

* imagePath - Path of image file.
* callback - returns 2 args (error, Matrix)


### findSubImage(options, callback)

> Finds positions of template matches in the source image. Can supply multiple template images.

* options
  * source (filepath, Buffer or Matrix)
  * templates (Array of filepath, Buffer or Matrix)
  * matchPercent (default 70) - Confidence lelvel to pick matches(max 100)
  * maximumMatches (default 1) - Number of matches to pick for each template
  * downPyramids (default 0) - Scale down the images before matching (using cv::downPyr)
  * searchExpansion (default 10) - Way to adjust search ROI for optimiaztions.
  * method (default TM_SQDIFF_NORMED) - Method to perform `cv::matchTemplate`.
    * TM_SQDIFF
    * TM_SQDIFF_NORMED
    * TM_CCORR
    * TM_CCORR_NORMED
    * TM_CCOEFF
    * TM_CCOEFF_NORMED
* callback - returns 1 arg (matches);
  * Optional. If not provided, a `Promise` will be returned;
  * matches - Array of match result objects with the following properties.
    * position: {x, y}
    * rect: {x, y, width, height}
    * confidence: 0~100
    * templateIndex: Index of template.
    
```
var MonkeyOpencv = require('monkey-opencv').MonkeyOpencv;
var Constants = require ('monkey-opencv').Constants;
var fs = require ('fs');

MonkeyOpencv.findSubImage({
  source: 'source.png',
  templates: ['template.png'],
  matchPercent: 70,
  maximumMatches: 1,
  downPyramids: 1,
  searchExpansion: 15,
  method: Constants.TM_CCORR_NORMED,
}, function(matches){
  console.log('Matches with callback: ',matches[0].position);
})

// Example with Matrix as a template
MonkeyOpencv.readImage('template.png', function(tmpl) {
  MonkeyOpencv.findSubImage({
    source: 'source.png',
    templates: [tmpl, 'invalid_template.png'],
  })
  .then(function(matches){
    console.log('Matches with Promise: ', matches[0].position);
  }, function(err){});
  });
});

// Example with Buffer as a template
var source = fs.readFileSync('template.png');
MonkeyOpencv.findSubImage({
  source: source,
  templates: ['template.png', 'invalid_template.png'],
})
.then(function(matches){
  console.log('Matches with Promise: ', matches[0].position);
}, function(err){});
});

```

# Installation

```
npm install --save monkey-opencv
```

## Requirements for building from source

### node-gyp, node-pre-gyp

### opencv

```
npm install node-gyp node-pre-gyp -g
```

### Requirements for Mac

XCode

### Requirements for windows

Install window build tools. Refer to https://github.com/nodejs/node-gyp.

```
npm install --global --production windows-build-tools
```


# References

1. https://github.com/Jmgr/actiona
2. https://github.com/monai/node-imagesearch
3. https://github.com/peterbraden/node-opencv
