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
        int searchExpansion);

    static MatchingPointList fastMatchTemplate(
        const cv::Mat &source,
        const cv::Mat &target,
        int matchPercentage,
        int maximumMatches,
        int downPyrs,
        int searchExpansion);

  private:

    static std::vector<Point2D> multipleMinMaxLoc(
        const cv::Mat &image,
        int maximumMatches,
        int method);

};
#endif // MATCHINGPOINTLIST_H
