#include "corealgmatlab.h"

//using namespace cv;

corealgmatlab::corealgmatlab()
{
}

corealgmatlab::~corealgmatlab()
{
}

cv::Mat corealgmatlab::conv2(const cv::Mat &img, const cv::Mat& ikernel, ConvolutionType type)
{
	cv::Mat dest;
	cv::Mat kernel;
	flip(ikernel, kernel, -1); 
	cv::Mat source = img;
	if (CONVOLUTION_FULL == type)
	{
		source = cv::Mat();
		const int additionalRows = kernel.rows - 1, additionalCols = kernel.cols - 1;
		
		copyMakeBorder(img, source, (additionalRows + 1) / 2, additionalRows / 2, (additionalCols + 1) / 2, additionalCols / 2, cv::BORDER_CONSTANT, cv::Scalar(0));
	}
	cv::Point anchor(kernel.cols - kernel.cols / 2 - 1, kernel.rows - kernel.rows / 2 - 1);
	int borderMode = cv::BORDER_CONSTANT;
	filter2D(source, dest, img.depth(), kernel, anchor, 0, borderMode);

	if (CONVOLUTION_VALID == type)
	{
		dest = dest.colRange((kernel.cols - 1) / 2, dest.cols - kernel.cols / 2).rowRange((kernel.rows - 1) / 2, dest.rows - kernel.rows / 2);
	}
	return dest;
}
