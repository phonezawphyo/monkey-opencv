#ifndef ADDON_H
#define ADDON_H

#include <nan.h>
#include "matchingpointlist.h"

using namespace v8;

int extractIntArgument(
    const char * key,
    Local<Object> o,
    int defaultValue);

Local<Array> prepareResult(MatchingPointList matches);

#endif
