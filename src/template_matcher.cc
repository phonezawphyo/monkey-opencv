#define NOMINMAX
#include <opencv/highgui.h>
#include <nan.h>
#include "template_matcher.h"

using namespace v8;

MatchingPointList TemplateMatcher::fastMatchTemplate(
        const char* source,
        const char* target,
        int matchPercentage,
        int maximumMatches,
        int downPyrs,
        int searchExpansion,
        int method)
{
  cv::Mat sourceImg, templateImg;

  sourceImg = cv::imread(source);
  templateImg = cv::imread(target);
  
  std::vector<cv::Mat> sources;
  sources.reserve(1);
  sources.push_back(sourceImg);

  return fastMatchTemplate(
      sources,
      templateImg,
      matchPercentage,
      maximumMatches,
      downPyrs,
      searchExpansion,
      method);
}


MatchingPointList TemplateMatcher::fastMatchTemplate(
    const std::vector<cv::Mat> &sources,
    const cv::Mat &target,
		int matchPercentage,
		int maximumMatches,
		int downPyrs,
		int searchExpansion,
    int method)
{
  MatchingPointList matchingPointList;
  int sourceIndex = 0;

  for (const cv::Mat &source: sources)
  {

    // create copies of the images to modify
    cv::Mat copyOfSource = source.clone();
    cv::Mat copyOfTarget = target.clone();

    cv::Size sourceSize = source.size();
    cv::Size targetSize = target.size();

    // down pyramid the images
    for(int ii = 0; ii < downPyrs; ii++)
    {
      // start with the source image
      sourceSize.width  = (sourceSize.width  + 1) / 2;
      sourceSize.height = (sourceSize.height + 1) / 2;

      cv::Mat smallSource(sourceSize, source.type());
      cv::pyrDown(copyOfSource, smallSource);

      // prepare for next loop, if any
      copyOfSource = smallSource.clone();

      // next, do the target
      targetSize.width  = (targetSize.width  + 1) / 2;
      targetSize.height = (targetSize.height + 1) / 2;

      cv::Mat smallTarget(targetSize, target.type());
      pyrDown(copyOfTarget, smallTarget);

      // prepare for next loop, if any
      copyOfTarget = smallTarget.clone();
    }

    // perform the match on the shrunken images
    cv::Size smallTargetSize = copyOfTarget.size();
    cv::Size smallSourceSize = copyOfSource.size();

    cv::Size resultSize;
    resultSize.width = smallSourceSize.width - smallTargetSize.width + 1;
    resultSize.height = smallSourceSize.height - smallTargetSize.height + 1;

    cv::Mat result(resultSize, CV_32FC1);

    cv::matchTemplate(copyOfSource,
        copyOfTarget,
        result,
        method);

    // find the top match locations
    std::vector<Point2D> locations = multipleMinMaxLoc(result,
        maximumMatches,
        method);

    // search the large images at the returned locations
    sourceSize = source.size();
    targetSize = target.size();

    int twoPowerNumDownPyrs = std::pow(2.0f, downPyrs);

    // create a copy of the source in order to adjust its ROI for searching
    for(int currMax = 0; currMax < maximumMatches; ++currMax)
    {
      // transform the point to its corresponding point in the larger image
      Point2D &currMaxLocation = locations[currMax];
      currMaxLocation *= twoPowerNumDownPyrs;
      currMaxLocation.setPoint(
          currMaxLocation.x() + targetSize.width / 2,
          currMaxLocation.y() + targetSize.height / 2);


      const Point2D &searchPoint = locations.at(currMax);

      // if we are searching for multiple targets and we have found a target or
      //  multiple targets, we don't want to search in the same location(s) again
      if(maximumMatches > 1 && !matchingPointList.empty())
      {
        bool thisTargetFound = false;

        for(int currPoint = 0;(unsigned long) currPoint < matchingPointList.size(); currPoint++)
        {
          const Point2D &foundPoint = matchingPointList.at(currPoint).position;
          if(std::abs(searchPoint.x() - foundPoint.x()) <= searchExpansion * 2 &&
              std::abs(searchPoint.y() - foundPoint.y()) <= searchExpansion * 2)
          {
            thisTargetFound = true;
            break;
          }
        }

        // if the current target has been found, continue onto the next point
        if(thisTargetFound)
          continue;
      }

      // set the source image's ROI to slightly larger than the target image,
      //  centred at the current point

      cv::Rect searchRoi = makeSearchRoi(sourceSize, searchPoint, target, searchExpansion);

      // Invoke cv::matchTemplate
      result = searchImage(source, target, searchRoi, resultSize, method);

      // Find best match location
      MatchingPoint mpt = findBestMatchLocation(
          result,
          target,
          searchRoi,
          sourceIndex,
          matchPercentage,
          method);

      if (mpt.empty) {
        break;
      } else {
        matchingPointList.push_back(mpt);
        
        if(maximumMatches <= 1) {
          break;
        }
      }
    }
    ++sourceIndex;
  }

  return matchingPointList;
}

cv::Mat TemplateMatcher::searchImage(
    const cv::Mat &source,
    const cv::Mat &target,
    const cv::Rect &searchRoi,
    cv::Size &resultSize,
    int method) {

  cv::Mat searchImage(source, searchRoi);

  // perform the search on the large images
  resultSize.width = searchRoi.width - target.size().width + 1;
  resultSize.height = searchRoi.height - target.size().height + 1;

  cv::Mat result = cv::Mat(resultSize, CV_32FC1);
  cv::matchTemplate(searchImage, target, result, method);

  return result;
}

// set the source image's ROI to slightly larger than 
// the target image, centred at the current point
cv::Rect TemplateMatcher::makeSearchRoi(
    const cv::Size &sourceSize,
    const Point2D &searchPoint,
    const cv::Mat &target,
    int searchExpansion) {

  cv::Rect searchRoi;
  searchRoi.x = searchPoint.x() - (target.size().width) / 2 - searchExpansion;
  searchRoi.y = searchPoint.y() - (target.size().height) / 2 - searchExpansion;
  searchRoi.width = target.size().width + searchExpansion * 2;
  searchRoi.height = target.size().height + searchExpansion * 2;

  // make sure ROI doesn't extend outside of image
  if(searchRoi.x < 0)
    searchRoi.x = 0;

  if(searchRoi.y < 0)
    searchRoi.y = 0;

  if((searchRoi.x + searchRoi.width) > (sourceSize.width - 1))
  {
    int numPixelsOver = (searchRoi.x + searchRoi.width) - (sourceSize.width - 1);

    searchRoi.width -= numPixelsOver;
  }

  if((searchRoi.y + searchRoi.height) > (sourceSize.height - 1))
  {
    int numPixelsOver = (searchRoi.y + searchRoi.height) - (sourceSize.height - 1);

    searchRoi.height -= numPixelsOver;
  }

  return searchRoi;
}

MatchingPoint TemplateMatcher::findBestMatchLocation(
    const cv::Mat &result,
    const cv::Mat &target,
    const cv::Rect &searchRoi,
    int sourceIndex,
    int matchPercentage,
    int method) {

  double minValue;
  double maxValue;
  cv::Point minLoc;
  cv::Point maxLoc;

  cv::minMaxLoc(result, &minValue, &maxValue, &minLoc, &maxLoc);

  double &value = (method == CV_TM_SQDIFF_NORMED) ? minValue : maxValue;
  cv::Point &loc = (method == CV_TM_SQDIFF_NORMED) ? minLoc : maxLoc;

  value *= 100.0;

  // transform point back to original image
  loc.x += searchRoi.x + target.size().width / 2;
  loc.y += searchRoi.y + target.size().height / 2;

  if(method == CV_TM_SQDIFF_NORMED)
    value = 100.0f - value;

  if(value >= matchPercentage)
  {
    // add the point to the list
    return MatchingPoint(Point2D(loc.x, loc.y), value, sourceIndex);
  }
  else
  {
    return MatchingPoint();
  }
}


std::vector<Point2D> TemplateMatcher::multipleMinMaxLoc(
    const cv::Mat &image,
    int maximumMatches,
    int method)
{
  std::vector<Point2D> locations(maximumMatches);

  std::vector<float> matches(maximumMatches, (method == CV_TM_SQDIFF_NORMED) ? std::numeric_limits<float>::max() : -std::numeric_limits<float>::max());
	cv::Size size = image.size();

	// extract the raw data for analysis
	for(int y = 0; y < size.height; ++y)
	{
		for(int x = 0; x < size.width; ++x)
		{
			float data = image.at<float>(y, x);

			// insert the data value into the array if it is greater than any of the
			//  other array values, and bump the other values below it, down
			for(int j = 0; j < maximumMatches; ++j)
			{
				// require at least 50% confidence on the sub-sampled image
				// in order to make this as fast as possible
				if((method == CV_TM_SQDIFF_NORMED && data < 0.5f && data < matches.at(j)) ||
						(method != CV_TM_SQDIFF_NORMED && data > 0.5f && data > matches.at(j)))
				{
					// move the maxima down
					for(int k = maximumMatches - 1; k > j; --k)
					{
						matches[k] = matches.at(k-1);
						locations[k] = locations.at(k-1);
					}

					// insert the value
					matches[j] = data;
					locations[j].setPoint(x, y);
					break;
				}
			}
		}
	}

	return locations;
}
