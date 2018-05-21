/*
 * morphologic.cpp
 *
 *  Created on: May 21, 2018
 *      Author: cui
 */

#include <opencv2/opencv.hpp>

using namespace cv;
using namespace std;

int main( int argc, char *argv[] )
{
	Mat src;
	src = imread( argv[1] );

	//blur
	Mat dst1, dst2, dst3;
	inRange( src, Scalar(0,0,100), Scalar(40,30,255), dst1 );

	Mat element = getStructuringElement( MORPH_ELLIPSE, Size(15,15) );
	dilate( dst1, dst2, element );
	erode( dst2, dst3, element );

	namedWindow( "ORIGINAL", WINDOW_AUTOSIZE );
	imshow("ORIGINAL", src);
	namedWindow( "SEGMENTED", WINDOW_AUTOSIZE );
	imshow("SEGMENTED", dst1 );
	namedWindow( "DLATION", WINDOW_AUTOSIZE );
	imshow("DLATION", dst2 );
	namedWindow( "EROSION", WINDOW_AUTOSIZE );
	imshow("EROSION", dst3 );

	waitKey(0);
	return 0;


}

