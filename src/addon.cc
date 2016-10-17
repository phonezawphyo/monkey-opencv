#include <nan.h>
//#include "sync.h"   // NOLINT(build/include)
#include "async.h"  // NOLINT(build/include)

using namespace v8;

// Expose synchronous and asynchronous access to our
NAN_MODULE_INIT(InitAll) {
  //Nan::Set(target, New<String>("calculateSync").ToLocalChecked(),
  //  GetFunction(New<FunctionTemplate>(CalculateSync)).ToLocalChecked());

  Nan::Set(target, Nan::New<String>("findSubImage").ToLocalChecked(),
    Nan::GetFunction(Nan::New<FunctionTemplate>(CalculateAsync)).ToLocalChecked());
}

NODE_MODULE(addon, InitAll)
