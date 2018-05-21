/*
 * sharpen.cpp
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

	//use Sobel && Laplacian
	Mat dst1, dst2;
	Sobel( src, dst1, -1, 1, 1 );
	Laplacian( src, dst2, -1);
	imwrite( "durant1.jpg", dst2 );

	namedWindow( "ORIGINAL", WINDOW_AUTOSIZE );
	imshow("ORIGINAL", src);
	namedWindow( "SOBEL", WINDOW_AUTOSIZE );
	imshow("SOBEL", dst1 );
	namedWindow( "LAPLACIAN", WINDOW_AUTOSIZE );
	imshow("LAPLACIAN", dst2 );

	waitKey(0);
	return 0;


}

