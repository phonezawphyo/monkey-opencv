#include <node.h>
#include <nan.h>
#include <opencv/cv.h>

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

  //int matchPercentage = 0;
  //int maximumMatches = 0;
  //int downPyrs = 0;
  //int searchExpansion = 0;

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
  Local<Object> source = o->Get(strSource).As<Object>();

  Local<String> strData = Nan::New("data").ToLocalChecked();
  Local<Value> buffer = source->Get(strData);
  std::cout << "Buffer: " << (char*)node::Buffer::Data(buffer);
  std::cout << "Buffer: " << *Nan::Utf8String(buffer->ToString());
  //std::cout << "Buffer: " << (char*)buffer.data();

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


void fastMatchTemplate(const cv::Mat &source,
		const cv::Mat &target,
		int matchPercentage,
		int maximumMatches,
		int downPyrs,
		int searchExpansion)
{
	//MatchingPointList matchingPointList;

	try
	{
		// create copies of the images to modify
		cv::Mat copyOfSource = source.clone();
		cv::Mat copyOfTarget = target.clone();

		cv::Size sourceSize = source.size();
		cv::Size targetSize = target.size();

		// down pyramid the images
		for(int ii = 0; ii < downPyrs; ii++)
		{
			// start with the source image
			sourceSize.width  = (sourceSize.width  + 1) / 2;
			sourceSize.height = (sourceSize.height + 1) / 2;

			cv::Mat smallSource(sourceSize, source.type());
			cv::pyrDown(copyOfSource, smallSource);

			// prepare for next loop, if any
			copyOfSource = smallSource.clone();

			// next, do the target
			targetSize.width  = (targetSize.width  + 1) / 2;
			targetSize.height = (targetSize.height + 1) / 2;

			cv::Mat smallTarget(targetSize, target.type());
			pyrDown(copyOfTarget, smallTarget);

			// prepare for next loop, if any
			copyOfTarget = smallTarget.clone();
		}

		// perform the match on the shrunken images
		cv::Size smallTargetSize = copyOfTarget.size();
		cv::Size smallSourceSize = copyOfSource.size();

		cv::Size resultSize;
		resultSize.width = smallSourceSize.width - smallTargetSize.width + 1;
		resultSize.height = smallSourceSize.height - smallTargetSize.height + 1;

		cv::Mat result(resultSize, CV_32FC1);
		//cv::matchTemplate(copyOfSource, copyOfTarget, result, toOpenCVMethod(method));
		cv::matchTemplate(copyOfSource, copyOfTarget, result, CV_TM_SQDIFF);

		// find the top match locations
    /*
		QVector<QPoint> locations = multipleMinMaxLoc(result, maximumMatches, method);
    */

		// search the large images at the returned locations
		sourceSize = source.size();
		targetSize = target.size();

		//int twoPowerNumDownPyrs = std::pow(2.0f, downPyrs);

		// create a copy of the source in order to adjust its ROI for searching
		for(int currMax = 0; currMax < maximumMatches; ++currMax)
		{
			// transform the point to its corresponding point in the larger image
			/*
			QPoint &currMaxLocation = locations[currMax];
			currMaxLocation *= twoPowerNumDownPyrs;
			currMaxLocation.setX(currMaxLocation.x() + targetSize.width / 2);
			currMaxLocation.setY(currMaxLocation.y() + targetSize.height / 2);

			const QPoint &searchPoint = locations.at(currMax);

			// if we are searching for multiple targets and we have found a target or
			//  multiple targets, we don't want to search in the same location(s) again
			if(maximumMatches > 1 && !matchingPointList.isEmpty())
			{
				bool thisTargetFound = false;

				for(int currPoint = 0; currPoint < matchingPointList.size(); currPoint++)
				{
					const QPoint &foundPoint = matchingPointList.at(currPoint).position;
					if(std::abs(searchPoint.x() - foundPoint.x()) <= searchExpansion * 2 &&
							std::abs(searchPoint.y() - foundPoint.y()) <= searchExpansion * 2)
					{
						thisTargetFound = true;
						break;
					}
				}

				// if the current target has been found, continue onto the next point
				if(thisTargetFound)
					continue;
			}

			// set the source image's ROI to slightly larger than the target image,
			//  centred at the current point
			cv::Rect searchRoi;
			searchRoi.x = searchPoint.x() - (target.size().width) / 2 - searchExpansion;
			searchRoi.y = searchPoint.y() - (target.size().height) / 2 - searchExpansion;
			searchRoi.width = target.size().width + searchExpansion * 2;
			searchRoi.height = target.size().height + searchExpansion * 2;

			// make sure ROI doesn't extend outside of image
			if(searchRoi.x < 0)
				searchRoi.x = 0;

			if(searchRoi.y < 0)
				searchRoi.y = 0;

			if((searchRoi.x + searchRoi.width) > (sourceSize.width - 1))
			{
				int numPixelsOver = (searchRoi.x + searchRoi.width) - (sourceSize.width - 1);

				searchRoi.width -= numPixelsOver;
			}

			if((searchRoi.y + searchRoi.height) > (sourceSize.height - 1))
			{
				int numPixelsOver = (searchRoi.y + searchRoi.height) - (sourceSize.height - 1);

				searchRoi.height -= numPixelsOver;
			}

			cv::Mat searchImage(source, searchRoi);

			// perform the search on the large images
			resultSize.width = searchRoi.width - target.size().width + 1;
			resultSize.height = searchRoi.height - target.size().height + 1;

			result = cv::Mat(resultSize, CV_32FC1);
			cv::matchTemplate(searchImage, target, result, toOpenCVMethod(method));

			// find the best match location
			double minValue;
			double maxValue;
			cv::Point minLoc;
			cv::Point maxLoc;

			cv::minMaxLoc(result, &minValue, &maxValue, &minLoc, &maxLoc);

			double &value = (method == SquaredDifferenceMethod) ? minValue : maxValue;
			cv::Point &loc = (method == SquaredDifferenceMethod) ? minLoc : maxLoc;

			value *= 100.0;

			// transform point back to original image
			loc.x += searchRoi.x + target.size().width / 2;
			loc.y += searchRoi.y + target.size().height / 2;

			if(method == SquaredDifferenceMethod)
				value = 100.0f - value;

			if(value >= matchPercentage)
			{
				// add the point to the list
				matchingPointList.append(MatchingPoint(QPoint(loc.x, loc.y), value, sourceIndex));

				// if we are only looking for a single target, we have found it, so we
				//  can return
				if(maximumMatches <= 1)
					break;
			}
			else
				break; // skip the rest
			*/
		}
	}
	catch(const cv::Exception &e)
	{
		//mError = OpenCVException;
		//mErrorString = tr("OpenCV exception: %1").arg(e.what());

		//return MatchingPointList();
	}


	//return matchingPointList;
}

/*
QVector<QPoint> OpenCVAlgorithms::multipleMinMaxLoc(const cv::Mat &image, int maximumMatches, AlgorithmMethod method)
{
	QVector<QPoint> locations(maximumMatches);
	QVector<float> matches(maximumMatches, (method == SquaredDifferenceMethod) ? std::numeric_limits<float>::max() : -std::numeric_limits<float>::max());
	cv::Size size = image.size();

	// extract the raw data for analysis
	for(int y = 0; y < size.height; ++y)
	{
		for(int x = 0; x < size.width; ++x)
		{
			float data = image.at<float>(y, x);

			// insert the data value into the array if it is greater than any of the
			//  other array values, and bump the other values below it, down
			for(int j = 0; j < maximumMatches; ++j)
			{
				// require at least 50% confidence on the sub-sampled image
				// in order to make this as fast as possible
				if((method == SquaredDifferenceMethod && data < 0.5f && data < matches.at(j)) ||
						(method != SquaredDifferenceMethod && data > 0.5f && data > matches.at(j)))
				{
					// move the maxima down
					for(int k = maximumMatches - 1; k > j; --k)
					{
						matches[k] = matches.at(k-1);
						locations[k] = locations.at(k-1);
					}

					// insert the value
					matches[j] = data;
					locations[j].setX(x);
					locations[j].setY(y);
					break;
				}
			}
		}
	}

	return locations;
}
*/

void init(Local<Object> exports) {
  exports->Set(Nan::New("testMe").ToLocalChecked(),
      Nan::New<v8::FunctionTemplate>(testMe)->GetFunction());
  exports->Set(Nan::New("findSubImg").ToLocalChecked(),
      Nan::New<v8::FunctionTemplate>(findSubImg)->GetFunction());
}

NODE_MODULE(testme, init)
