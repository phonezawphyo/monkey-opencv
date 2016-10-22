#include "opencv.h"

class Constants: public Nan::ObjectWrap {
public:
  static void Init(Local<Object> target);
};
