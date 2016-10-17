#include "template_matcher.h"

#include <node.h>
#include <opencv/cv.h>
#include <opencv/highgui.h>
#include "addon.h"

void findSubImg(const Nan::FunctionCallbackInfo<Value>& args) {
  Isolate* isolate = Isolate::GetCurrent();
  HandleScope scope(isolate);

	printf("SubImage!\n");

  if (args.Length() != 1) {
    Nan::ThrowTypeError("Wrong number of arguments");
    return;
  }

  Local<Object> o = args[0].As<Object>();

  if (!o->IsObject()) {
    Nan::ThrowTypeError("Wrong arguments");
    return;
  }

  Local<String> strSource = Nan::New("source").ToLocalChecked();
  Local<String> strTemplate = Nan::New("template").ToLocalChecked();

  if (!o->Has(strSource)) {
    isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(isolate, "Missing argument 'source'")));
  }

  if (!o->Has(strTemplate)) {
    isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(isolate, "Missing argument 'template'")));
  } 

  Nan::Utf8String source(o->Get(strSource)->ToString());
  Nan::Utf8String tmpl(o->Get(strTemplate)->ToString());

  int matchPercent = extractIntArgument("matchPercent", o, 70);
  int maximumMatches = extractIntArgument("maximumMatches", o, 1);
  int downPyramids = extractIntArgument("downPyramids", o, 1);
  int searchExpansion = extractIntArgument("searchExpansion", o, 15);

  if (matchPercent == -1 || maximumMatches == -1 || downPyramids == -1 || searchExpansion == -1) {
    return;
  }

  MatchingPointList matches;

  try {
    matches = TemplateMatcher::fastMatchTemplate(
        *source,
        *tmpl, 
        matchPercent,
        maximumMatches,
        downPyramids,
        searchExpansion);
  } catch(int e) {
    isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(isolate, "OpenCV error")));
    return;
  }

  // Invoke oncomplete callback
  Local<String> strOnComplete = Nan::New("oncomplete").ToLocalChecked();

  if (o->Has(strOnComplete)) {
    Local<Array> array = prepareResult(matches);

    const unsigned argc = 1;
    v8::Local<v8::Value> argv[argc] = { array };
    Local<Function> oncomplete = o->Get(strOnComplete).As<Function>();
  
    Nan::MakeCallback(Nan::GetCurrentContext()->Global(), oncomplete, argc, argv);
  }

  args.GetReturnValue().Set(Nan::True());
}

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

Local<Array> prepareResult(MatchingPointList matches) {
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
}

void init(Local<Object> exports) {
  exports->Set(Nan::New("findSubImg").ToLocalChecked(),
      Nan::New<v8::FunctionTemplate>(findSubImg)->GetFunction());
}

NODE_MODULE(testme, init)
