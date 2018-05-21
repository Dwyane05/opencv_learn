/*
 * inpainting.cpp
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
	if( src.empty() ){
		cout << "Couldn't load image" << endl;
		return -1;
	}
#if 0
	//read mask file
	Mat mask;
	mask = imread( argv[2] );
	cvtColor( mask, mask, COLOR_BGR2GRAY );
#else
	Mat mask;
	mask = Mat::zeros( src.size(), CV_8U );
//	cvtColor( src, mask, COLOR_BGR2GRAY );
//	threshold( mask, mask, 235, 235, THRESH_BINARY );
#endif
	//apply inpainting
	Mat dst1, dst2;
	inpaint(src, mask, dst1, 3, INPAINT_TELEA);
	inpaint(src, mask, dst2, 3, INPAINT_NS);


	namedWindow( "ORIGINAL", WINDOW_AUTOSIZE );
	imshow("ORIGINAL", src);
	namedWindow( "MASK", WINDOW_AUTOSIZE );
	imshow("MASK", mask);
	namedWindow( "INPAINT_TELEA", WINDOW_AUTOSIZE );
	imshow("INPAINT_TELEA", dst1 );
	namedWindow( "INPAINT_NS", WINDOW_AUTOSIZE );
	imshow("INPAINT_NS", dst2 );

	waitKey(0);
	return 0;


}

