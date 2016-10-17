#ifndef FIND_SUB_IMAGE_H
#define FIND_SUB_IMAGE_H

#include <nan.h>
#include "matchingpointlist.h"

using namespace v8;

/* options
 * =======
 * - source: Source image url
 * - template: Template image url
 * - matchPercent: (70) int 0 ~ 100
 * - maximumMatches: (1) int >= 1 ,
 * - downPyramids: (1) int >= 1,
 * - searchExpansion: (15) int >= 1,
 */
Local<Array> findSubImg(Persistent<Object> o);

int extractIntArgument(
    const char * key,
    Object o,
    int defaultValue);

Local<Array> prepareResult(MatchingPointList matches);

#endif
