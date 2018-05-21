/*
 * pyramid.cpp
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

	//apply pyrDown
	Mat dst1, dst2;
	pyrDown( src, dst1 );
	pyrDown( dst1, dst2);
	imwrite( "durant1.jpg", dst2 );

	namedWindow( "ORIGINAL", WINDOW_AUTOSIZE );
	imshow("ORIGINAL", src);
	namedWindow( "1st pyrDown", WINDOW_AUTOSIZE );
	imshow("1st pyrDown", dst1 );
	namedWindow( "2st pyrDown", WINDOW_AUTOSIZE );
	imshow("2st pyrDown", dst2 );

	pyrUp( dst2, dst1 );
	pyrUp( dst1, src );

	namedWindow( "NEW ORIGINAL", WINDOW_AUTOSIZE );
	imshow("NEW ORIGINAL", dst2);
	namedWindow( "1st pyrUp", WINDOW_AUTOSIZE );
	imshow("1st pyrUp", dst1 );
	namedWindow( "2st pyrUp", WINDOW_AUTOSIZE );
	imshow("2st pyrUp", src );

	waitKey(0);
	return 0;


}

