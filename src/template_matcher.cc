#define NOMINMAX
#include <opencv/highgui.h>
#include <nan.h>
#include "template_matcher.h"

using namespace v8;

MatchingPointList TemplateMatcher::match(
    const cv::Mat &source,
    const std::vector<cv::Mat> &templates) {

  MatchingPointList matchingPointList;

  int templateIndex = 0;

  cv::Mat copyOfSource = downPyrImage(source);

  for (const cv::Mat &target: templates) {
    
    cv::Mat copyOfTarget = downPyrImage(target);
    cv::Mat result = makeResult(source, target);

    // Make matchTemplate result
    cv::matchTemplate(copyOfSource,
        copyOfTarget,
        result,
        method);

    // Find the top match locations
    std::vector<Point2D> locations = multipleMinMaxLoc(result);
    
    // Update matching points in matchingPointList
    updateMatchingPoints(source, target,
        result,
        locations,
        matchingPointList,
        templateIndex);

    ++templateIndex;
  }

  return matchingPointList;
}
    
cv::Mat TemplateMatcher::makeResult(
    const cv::Mat &source,
    const cv::Mat &target) {

    cv::Size sourceSize = source.size();
    cv::Size targetSize = target.size();

    cv::Size resultSize;
    resultSize.width = sourceSize.width - targetSize.width + 1;
    resultSize.height = sourceSize.height - targetSize.height + 1;

    return cv::Mat(resultSize, CV_32FC1);
}

cv::Mat TemplateMatcher::downPyrImage(
    const cv::Mat &image) {

  // create copies of the images to modify
  cv::Mat imageCopy = image.clone();
  cv::Size imageSize = image.size();

  // down pyramid the images
  for(int ii = 0; ii < downPyrs; ii++)
  {
    // start with the image image
    imageSize.width  = (imageSize.width  + 1) / 2;
    imageSize.height = (imageSize.height + 1) / 2;

    cv::Mat smallImage(imageSize, image.type());
    cv::pyrDown(imageCopy, smallImage);

    // prepare for next loop, if any
    imageCopy = smallImage.clone();
  }

  return imageCopy;
}
    
void TemplateMatcher::updateMatchingPoints(
    const cv::Mat &source,
    const cv::Mat &target,
    const cv::Mat &result,
    std::vector<Point2D> locations,
    MatchingPointList &matchingPointList,
    int templateIndex) {

  // search the large images at the returned locations
  cv::Size sourceSize = source.size();
  cv::Size targetSize = target.size();
  cv::Size resultSize = result.size();

  int upPyrs = std::pow(2.0f, downPyrs);

  // create a copy of the source in order to adjust its ROI for searching
  for(int currMax = 0; currMax < maximumMatches; ++currMax)
  {
    // transform the point to its corresponding point in the larger image
    const Point2D &searchPoint = updatedSearchPoint(targetSize, locations, currMax, upPyrs);

    // Do next target if this target is found
    if (isTargetFound(matchingPointList, searchPoint)) {
      continue;
    }

    // set the source image's ROI to slightly larger than the target image,
    //  centred at the current point
    cv::Rect searchRoi = makeSearchRoi(sourceSize, searchPoint, target);

    // Invoke cv::matchTemplate
    cv::Mat result = searchImage(source, target, searchRoi, resultSize);

    // Find best match location
    MatchingPoint matchPoint = findBestMatchLocation(
        result,
        target,
        searchRoi,
        templateIndex);

    if (matchPoint.empty) {
      break;
    } else {
      matchingPointList.push_back(matchPoint);

      if(maximumMatches <= 1) {
        break;
      }
    }
  }
}

Point2D TemplateMatcher::updatedSearchPoint(
    const cv::Size &targetSize,
    std::vector<Point2D> &locations,
    int currMax,
    int upPyrs) {

  Point2D &currMaxLocation = locations[currMax];
  currMaxLocation *= upPyrs;
  currMaxLocation.setPoint(
      currMaxLocation.x() + targetSize.width / 2,
      currMaxLocation.y() + targetSize.height / 2);

  return currMaxLocation;
}

/* If we are searching for multiple targets and we have found a target or
 *  multiple targets, we don't want to search in the same location(s) again
 */
bool TemplateMatcher::isTargetFound(
    const MatchingPointList & matchingPointList,
    const Point2D &searchPoint) {

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

    return thisTargetFound;
  } else {
    return false;
  }
}

cv::Mat TemplateMatcher::searchImage(
    const cv::Mat &source,
    const cv::Mat &target,
    const cv::Rect &searchRoi,
    cv::Size &resultSize) {

  cv::Mat img(source, searchRoi);

  // perform the search on the large images
  resultSize.width = searchRoi.width - target.size().width + 1;
  resultSize.height = searchRoi.height - target.size().height + 1;

  cv::Mat result = cv::Mat(resultSize, CV_32FC1);
  cv::matchTemplate(img, target, result, method);

  return result;
}

// set the source image's ROI to slightly larger than 
// the target image, centred at the current point
cv::Rect TemplateMatcher::makeSearchRoi(
    const cv::Size &sourceSize,
    const Point2D &searchPoint,
    const cv::Mat &target) {

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
    int templateIndex) {

  double minValue;
  double maxValue;
  cv::Point minLoc;
  cv::Point maxLoc;

  cv::minMaxLoc(result, &minValue, &maxValue, &minLoc, &maxLoc);

  double &value = (method == CV_TM_SQDIFF_NORMED) ? minValue : maxValue;
  cv::Point &loc = (method == CV_TM_SQDIFF_NORMED) ? minLoc : maxLoc;

  value *= 100.0;

  const cv::Size &size = target.size();

  // transform point back to original image
  loc.x += searchRoi.x + size.width / 2;
  loc.y += searchRoi.y + size.height / 2;

  cv::Rect rect(loc.x - size.width / 2,loc.y - size.height / 2, size.width, size.height);

  if(method == CV_TM_SQDIFF_NORMED)
    value = 100.0f - value;

  if(value >= matchPercentage)
  {
    // add the point to the list
    return MatchingPoint(Point2D(loc.x, loc.y), rect, value, templateIndex);
  }
  else
  {
    return MatchingPoint();
  }
}


std::vector<Point2D> TemplateMatcher::multipleMinMaxLoc(const cv::Mat &image)
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
