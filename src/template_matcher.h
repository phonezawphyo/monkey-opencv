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

    static cv::Mat downPyrImage(
        const cv::Mat &image,
        int downPyrs);

    static std::vector<Point2D> multipleMinMaxLoc(
        const cv::Mat &image,
        int maximumMatches,
        int method);

    static Point2D updatedSearchPoint(
        const cv::Size &targetSize,
        std::vector<Point2D> &locations,
        int currMax,
        int upPyrs);

    static void updateMatchingPoints(
        const cv::Mat &source,
        const cv::Mat &target,
        cv::Size &resultSize,
        std::vector<Point2D> locations,
        MatchingPointList &matchingPointList,
        int matchPercentage,
        int maximumMatches,
        int downPyrs,
        int searchExpension,
        int sourceIndex,
        int method); 

    static bool isTargetFound(
        const MatchingPointList & matchingPointList,
        const Point2D &searchPoint,
        int searchExpansion,
        int maximumMatches);

    static cv::Rect makeSearchRoi(
      const cv::Size &sourceSize,
      const Point2D &searchPoint,
      const cv::Mat &target,
      int searchExpansion);

    //static void searchImage(
    //  const cv::Mat &source,
    //  const cv::Mat &target,
    //  const cv::Rect &searchRoi,
    //  cv::Mat &result,
    //  cv::Size &resultSize,
    //  int method);
    static cv::Mat searchImage(
      const cv::Mat &source,
      const cv::Mat &target,
      const cv::Rect &searchRoi,
      cv::Size &resultSize,
      int method);

    static MatchingPoint findBestMatchLocation(
      const cv::Mat &result,
      const cv::Mat &target,
      const cv::Rect &searchRoi,
      int sourceIndex,
      int matchPercentage,
      int method);
};
#endif // MATCHINGPOINTLIST_H
