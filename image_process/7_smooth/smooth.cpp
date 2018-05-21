/*
 * smooth.cpp
 *
 *  Created on: May 21, 2018
 *      Author: cui
 */

#include <opencv2/opencv.hpp>

using namespace cv;

int main( int argc, char *argv[] )
{
	Mat src;
	src = imread( argv[1] );

	Mat dst1, dst2;
	GaussianBlur( src, dst1, Size(9,9), 0, 0 );
	medianBlur( src, dst2, 9);

	namedWindow( "ORIGINAL", WINDOW_AUTOSIZE );
	imshow("ORIGINAL", src);
	namedWindow( "GAUSSIAN BLUR", WINDOW_AUTOSIZE );
	imshow("GAUSSIAN BLUR", dst1 );
	namedWindow( "MEDIAN BLUR", WINDOW_AUTOSIZE );
	imshow("MEDIAN BLUR", dst2 );

	waitKey(0);
	return 0;


}

