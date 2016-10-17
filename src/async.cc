//#include <node.h>
#include <nan.h>
#include "template_matcher.h"
#include "async.h"

using namespace v8;

class FindImageWorker : public Nan::AsyncWorker {
  public:
    FindImageWorker(Nan::Callback *callback,
			std::string source,
      std::string target,
      int matchPercent,
      int maximumMatches,
      int downPyramids,
      int searchExpansion) : AsyncWorker(callback),
        source(source), target(target), matchPercent(matchPercent), maximumMatches(maximumMatches), downPyramids(downPyramids), searchExpansion(searchExpansion) {

    }

    ~FindImageWorker() {}

    void Execute() {
      matches = TemplateMatcher::fastMatchTemplate(
          source.c_str(),
          target.c_str(), 
          matchPercent,
          maximumMatches,
          downPyramids,
          searchExpansion);
    }

    Local<Array> toResult() {
      Local<Array> array = Nan::New<Array>();

      for(std::vector<int>::size_type i = 0; i != matches.size(); i++) {
        MatchingPoint point = matches.at(i);
        Local<Object> positionObj = Nan::New<Object>();
        Local<Object> pointObj = Nan::New<Object>();

        Nan::Set(positionObj, Nan::New("x").ToLocalChecked(),Nan::New(point.position.x()));
        Nan::Set(positionObj, Nan::New("y").ToLocalChecked(),Nan::New(point.position.y()));
        Nan::Set(pointObj, Nan::New("position").ToLocalChecked(),positionObj);
        Nan::Set(pointObj, Nan::New("confidence").ToLocalChecked(),Nan::New(point.confidence));
        Nan::Set(pointObj, Nan::New("imageIndex").ToLocalChecked(),Nan::New(point.imageIndex));

        Nan::Set(array, i, pointObj);
      }

      return array;
    };

    void HandleOKCallback() {
      Nan::EscapableHandleScope scope;

      Local<Array> array = toResult();

      Local<Value> argv[] = {
        array
      };

      callback->Call(1, argv);
    }

	private:
    std::string source;
    std::string target;
		int matchPercent;
		int maximumMatches;
		int downPyramids;
		int searchExpansion;
    MatchingPointList matches;
};

int extractIntArgument(const char * key, Local<Object> o, int defaultValue) {

  Local<String> strKey = Nan::New(key).ToLocalChecked();

  if (o->Has(strKey)) {
    Local<Value> v = o->Get(strKey);

    if (!v->IsNumber()) {
      std::string s1 = "Wrong argument: ";
      std::string s2 = key;
      Nan::ThrowTypeError((s1 + s2).c_str());
      return -1;
    }

    return v->NumberValue();
  }

  return defaultValue;
}


/* options
 * =======
 * - source: Source image url
 * - template: Template image url
 * - matchPercent: (70) int 0 ~ 100
 * - maximumMatches: (1) int >= 1 ,
 * - downPyramids: (1) int >= 1,
 * - searchExpansion: (15) int >= 1,
 */
NAN_METHOD(CalculateAsync) {
  Isolate* isolate = Isolate::GetCurrent();
  Nan::EscapableHandleScope scope;

  int argc = info.Length();
  if (argc != 1 && argc != 2) {
    Nan::ThrowTypeError("Wrong number of arguments (2)");
    return;
  }

	Local<Object> o = info[0].As<Object>();

  if (!o->IsObject()) {
    Nan::ThrowTypeError("Wrong arguments");
    return;
  }

  Local<String> strSource = Nan::New("source").ToLocalChecked();
  Local<String> strTemplate = Nan::New("template").ToLocalChecked();

  Nan::Utf8String source(o->Get(strSource)->ToString());
  Nan::Utf8String tmpl(o->Get(strTemplate)->ToString());

  if (!o->Has(strSource)) {
    isolate->ThrowException(Exception::TypeError(Nan::New("Missing argument 'source'").ToLocalChecked()));
		return;
  }

  if (!o->Has(strTemplate)) {
    isolate->ThrowException(Exception::TypeError(Nan::New("Missing argument 'template'").ToLocalChecked()));
		return;
  } 

  int matchPercent = extractIntArgument("matchPercent", o, 70);
  int maximumMatches = extractIntArgument("maximumMatches", o, 1);
  int downPyramids = extractIntArgument("downPyramids", o, 1);
  int searchExpansion = extractIntArgument("searchExpansion", o, 15);

  if (matchPercent == -1 || maximumMatches == -1 || downPyramids == -1 || searchExpansion == -1) {
    return;
  }


  if (argc == 1) {
    // Sync

    Nan::Callback *callback = new Nan::Callback();
    FindImageWorker *worker = new FindImageWorker(callback,
        std::string(*source),
        std::string(*tmpl),
        matchPercent,
        maximumMatches,
        downPyramids,
        searchExpansion);
    worker->Execute();
    Local<Array> array = worker->toResult();

    info.GetReturnValue().Set(array);

  } else if (argc == 2) {
    // Async

    Nan::Callback *callback = new Nan::Callback(info[1].As<Function>());
    FindImageWorker *worker = new FindImageWorker(callback,
        std::string(*source),
        std::string(*tmpl),
        matchPercent,
        maximumMatches,
        downPyramids,
        searchExpansion);
    Nan::AsyncQueueWorker(worker);

  }
}
