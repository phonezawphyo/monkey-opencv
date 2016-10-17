#include <node.h>
#include <nan.h>
#include "template_matcher.h"

using namespace v8;

void testMe(const Nan::FunctionCallbackInfo<Value>& args) {
	Isolate* isolate = args.GetIsolate();
	printf("Hello c!\n");
  args.GetReturnValue().Set(Nan::New("world").ToLocalChecked());
}

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

  Nan::Utf8String source(o->Get(strSource)->ToString());
  std::cout << "Source: " << *source << "\n";
  cv::Mat sourceImg = cv::imread(*source);

  Nan::Utf8String tmpl(o->Get(strTemplate)->ToString());
  std::cout << "Template: " << *tmpl<< "\n";
  cv::Mat templateImg = cv::imread(*tmpl);

  MatchingPointList matches = TemplateMatcher::fastMatchTemplate(sourceImg, templateImg, 90, 1, 1, 15);

  std::cout << "match: x:" << matches.at(0).position.x() << " y:" << matches.at(0).position.y() ;

  if (!o->Has(strTemplate)) {
    isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(isolate, "Missing argument 'template'")));
  } 

  // Invoke oncomplete callback
  Local<String> strOnComplete = Nan::New("oncomplete").ToLocalChecked();

  if (o->Has(strOnComplete)) {
    const unsigned argc = 2;
    v8::Local<v8::Value> argv[argc] = {
      Nan::New("hello world").ToLocalChecked(),
      Nan::New("hello kitty").ToLocalChecked() };
    Local<Function> oncomplete = o->Get(strOnComplete).As<Function>();
    Nan::MakeCallback(Nan::GetCurrentContext()->Global(), oncomplete, argc, argv);
  }

  //args.GetReturnValue().Set(Nan::New("world").ToLocalChecked());
}

//cv::Mat toCVMat(const QImage &image)
//{
//  cv::Mat mat(image.height(), image.width(), CV_8UC4, const_cast<uchar *>(image.bits()), image.bytesPerLine());
//  cv::Mat back(mat.rows, mat.cols, CV_8UC3);
//  int from_to[] = {0,0,  1,1,  2,2};
//
//  cv::mixChannels(&mat, 1, &back, 1, from_to, 3);
//
//  return back;
//}

void init(Local<Object> exports) {
  exports->Set(Nan::New("testMe").ToLocalChecked(),
      Nan::New<v8::FunctionTemplate>(testMe)->GetFunction());
  exports->Set(Nan::New("findSubImg").ToLocalChecked(),
      Nan::New<v8::FunctionTemplate>(findSubImg)->GetFunction());
}

NODE_MODULE(testme, init)
