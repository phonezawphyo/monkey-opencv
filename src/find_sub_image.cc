#include "find_sub_image.h"
#include "matrix.h"

using namespace v8;

void FindSubImage::Execute() {
  TemplateMatcher matcher(matchPercent, maximumMatches, downPyramids, searchExpansion, method);

  try {
    matches = matcher.match(source, templates);
  } catch(const cv::Exception &e) {
    SetErrorMessage(e.what());
  }
}

void FindSubImage::HandleOKCallback() {
  Nan::EscapableHandleScope scope;

  Local<Array> array = toResult();

  Local<Value> argv[] = {
    array
  };

  callback->Call(1, argv);
}

void FindSubImage::HandleErrorCallback() {
  Nan::ThrowTypeError(ErrorMessage());
}

Local<Array> FindSubImage::toResult() {
  Local<Array> array = Nan::New<Array>();

  for(std::vector<int>::size_type i = 0; i != matches.size(); i++) {
    MatchingPoint point = matches.at(i);
    Local<Object> positionObj = Nan::New<Object>();
    Local<Object> pointObj = Nan::New<Object>();

    Nan::Set(positionObj, Nan::New("x").ToLocalChecked(),Nan::New(point.position.x()));
    Nan::Set(positionObj, Nan::New("y").ToLocalChecked(),Nan::New(point.position.y()));
    Nan::Set(pointObj, Nan::New("position").ToLocalChecked(),positionObj);
    Nan::Set(pointObj, Nan::New("confidence").ToLocalChecked(),Nan::New(point.confidence));
    Nan::Set(pointObj, Nan::New("templateIndex").ToLocalChecked(),Nan::New(point.imageIndex));

    Nan::Set(array, i, pointObj);
  }

  return array;
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


/* options
 * =======
 * - source: Source image url/Matrix
 * - templates: [Template image urls/Matrices]
 * - matchPercent: (70) int 0 ~ 100
 * - maximumMatches: (1) int >= 1 ,
 * - downPyramids: (1) int >= 1,
 * - searchExpansion: (15) int >= 1,
 * - method: 0~6
 * - callback: fn
 */
NAN_METHOD(FindSubImage::findSubImage) {
  Nan::EscapableHandleScope scope;

  int argc = info.Length();
  if (argc != 8) {
    Nan::ThrowTypeError("Wrong number of arguments (8)");
    return;
  }

  // Extract arguments
  cv::Mat source = Nan::ObjectWrap::Unwrap<Matrix>(info[0]->ToObject())->mat;
  Local<Array> templateArray = Local<Array>::Cast(info[1]->ToObject());
  int matchPercent = info[2]->NumberValue();
  int maximumMatches = info[3]->NumberValue();
  int downPyramids = info[4]->NumberValue();;
  int searchExpansion = info[5]->NumberValue();
  int method = info[6]->NumberValue();
  Nan::Callback *callback = new Nan::Callback(info[7].As<Function>());

  // Unwrap source
  std::vector<cv::Mat> templates;
  templates.reserve(templateArray->Length());

	for(uint32_t idx = 0; idx < templateArray->Length(); ++idx) {
    Matrix* tmpl = Nan::ObjectWrap::Unwrap<Matrix>(templateArray->Get(idx)->ToObject());
    templates.push_back(tmpl->mat);
  }

  // Exec async
  FindSubImage *worker = new FindSubImage(callback,
      source,
      templates,
      matchPercent,
      maximumMatches,
      downPyramids,
      searchExpansion,
      method);
  Nan::AsyncQueueWorker(worker);
}


void FindSubImage::Init(v8::Local<v8::Object> target) {
  v8::Local<Function> fn = Nan::GetFunction(Nan::New<FunctionTemplate>(findSubImage)).ToLocalChecked();
  Nan::Set(target, Nan::New<String>("findSubImage").ToLocalChecked(),fn);
}

