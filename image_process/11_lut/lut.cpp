/*
 * lut.cpp
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
	//create lut table
	uchar *M = (uchar*)malloc(256*sizeof(uchar));
	for( int i = 0; i < 256; i++ ){
		M[i] = i*0.5;
	}
	Mat lut( 1, 256, CV_8UC1, M );

	//apply lut
	Mat dst;
	LUT( src, lut, dst );

	namedWindow( "ORIGINAL", WINDOW_AUTOSIZE );
	imshow("ORIGINAL", src);
	namedWindow( "LUT", WINDOW_AUTOSIZE );
	imshow("LUT", dst );


	waitKey(0);
	return 0;


}

