#include <nan.h>
#include "find_sub_image.h"
#include "opencv.h"
#include "contours.h"
#include "constants.h"
#include "matrix.h"

using namespace v8;

// Expose synchronous and asynchronous access to our
NAN_MODULE_INIT(InitAll) {
  FindSubImage::Init(target);
  OpenCV::Init(target);
  Matrix::Init(target);
  Contour::Init(target);
  Constants::Init(target);
}

NODE_MODULE(fast_template_matcher, InitAll)
