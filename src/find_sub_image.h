#ifndef __FindSubImage_H__ 
#define __FindSubImage_H__

#include <nan.h>
#include "template_matcher.h"

class FindSubImage: public Nan::AsyncWorker {
  public:
    FindSubImage(Nan::Callback *callback,
    	std::string source,
      std::string target,
      int matchPercent,
      int maximumMatches,
      int downPyramids,
      int searchExpansion,
      int method) : AsyncWorker(callback),
        source(source), target(target), matchPercent(matchPercent), maximumMatches(maximumMatches), downPyramids(downPyramids), searchExpansion(searchExpansion), method(method) {
    }
    
    ~FindSubImage() {}

    void Execute();
    void HandleOKCallback();
    
    v8::Local<v8::Array> toResult();

    static void Init(v8::Local<v8::Object> target);
    static NAN_METHOD(findSubImage);

	private:

    std::string source;
    std::string target;
		int matchPercent;
		int maximumMatches;
		int downPyramids;
		int searchExpansion;
		int method;
    MatchingPointList matches;
};

#endif
