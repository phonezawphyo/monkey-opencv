#ifndef TEMPLATE_MATCHER_H 
#define TEMPLATE_MATCHER_H 

#include <opencv/cv.h>
#include "matchingpointlist.h"

class TemplateMatcher {
  public:

    static MatchingPointList fastMatchTemplate(
        const char* source,
        const char* target,
        int matchPercentage,
        int maximumMatches,
        int downPyrs,
        int searchExpansion,
        int method);

    static MatchingPointList fastMatchTemplate(
        const std::vector<cv::Mat> &sources,
        const cv::Mat &target,
        int matchPercentage,
        int maximumMatches,
        int downPyrs,
        int searchExpansion,
        int method);

  private:

    static std::vector<Point2D> multipleMinMaxLoc(
        const cv::Mat &image,
        int maximumMatches,
        int method);

    static cv::Rect makeSearchRoi(
      const cv::Size &sourceSize,
      const Point2D &searchPoint,
      const cv::Mat &target,
      int searchExpansion);

    static MatchingPoint findBestMatchLocation(
      const cv::Mat &result,
      const cv::Mat &target,
      const cv::Rect &searchRoi,
      int sourceIndex,
      int matchPercentage,
      int method);
};
#endif // MATCHINGPOINTLIST_H
