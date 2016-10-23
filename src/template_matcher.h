#ifndef TEMPLATE_MATCHER_H 
#define TEMPLATE_MATCHER_H 

#include <opencv/cv.h>
#include "matchingpointlist.h"

class TemplateMatcher {
  public:
    int matchPercentage = 70;
    int maximumMatches = 1;
    int downPyrs = 0;
    int searchExpansion = 15;
    int method = CV_TM_SQDIFF_NORMED;

    TemplateMatcher(
        int matchPercentage,
        int maximumMatches,
        int downPyrs,
        int searchExpansion,
        int method) :
      matchPercentage(matchPercentage),
      maximumMatches(maximumMatches),
      downPyrs(downPyrs),
      searchExpansion(searchExpansion),
      method(method) {
    }

    ~TemplateMatcher() {
    }

    MatchingPointList match(
        const cv::Mat &source,
        const std::vector<cv::Mat> &templates);

  private:

    cv::Mat makeResult(
        const cv::Mat &source,
        const cv::Mat &target);

    cv::Mat downPyrImage(const cv::Mat &image);

    std::vector<Point2D> multipleMinMaxLoc(const cv::Mat &image);

    Point2D updatedSearchPoint(
        const cv::Size &targetSize,
        std::vector<Point2D> &locations,
        int currMax,
        int upPyrs);

    void updateMatchingPoints(
        const cv::Mat &source,
        const cv::Mat &target,
        const cv::Mat &result,
        std::vector<Point2D> locations,
        MatchingPointList &matchingPointList,
        int templateIndex);

    bool isTargetFound(
        const MatchingPointList & matchingPointList,
        const Point2D &searchPoint);

    cv::Rect makeSearchRoi(
      const cv::Size &sourceSize,
      const Point2D &searchPoint,
      const cv::Mat &target);

    cv::Mat searchImage(
      const cv::Mat &source,
      const cv::Mat &target,
      const cv::Rect &searchRoi,
      cv::Size &resultSize);

    MatchingPoint findBestMatchLocation(
      const cv::Mat &result,
      const cv::Mat &target,
      const cv::Rect &searchRoi,
      int templateIndex);
};
#endif // MATCHINGPOINTLIST_H
