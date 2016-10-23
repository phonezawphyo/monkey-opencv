#ifndef __FindSubImage_H__ 
#define __FindSubImage_H__

#include <nan.h>
#include "template_matcher.h"

class FindSubImage: public Nan::AsyncWorker {
  public:
    FindSubImage(Nan::Callback *callback,
      const cv::Mat &source,
      const std::vector<cv::Mat> &templates,
      int matchPercent,
      int maximumMatches,
      int downPyramids,
      int searchExpansion,
      int method) : AsyncWorker(callback),
        source(source), templates(templates), matchPercent(matchPercent), maximumMatches(maximumMatches), downPyramids(downPyramids), searchExpansion(searchExpansion), method(method) {
    }
    
    ~FindSubImage() {
      templates.clear();
    }

    void Execute();
    void HandleOKCallback();
    void HandleErrorCallback();
    

    static void Init(v8::Local<v8::Object> target);
    static NAN_METHOD(findSubImage);

	private:

    v8::Local<v8::Array> toResult();

    cv::Mat source;
    std::vector<cv::Mat> templates;
		int matchPercent;
		int maximumMatches;
		int downPyramids;
		int searchExpansion;
		int method;
    MatchingPointList matches;
};

#endif
