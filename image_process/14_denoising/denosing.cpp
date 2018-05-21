/*
 * denosing.cpp
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

	//添加一些噪音
	Mat noisy = src.clone();
	Mat noise( src.size(), src.type() );
	randn(noise, 0, 50 );
	noisy += noise;

	//apply denosing
	Mat dst;
	fastNlMeansDenoisingColored( noisy, dst, 30, 30, 7, 21);

	namedWindow( "ORIGINAL", WINDOW_AUTOSIZE );
	imshow("ORIGINAL", src);
	namedWindow( "ORIGINAL WITH NOISE", WINDOW_AUTOSIZE );
	imshow("ORIGINAL WITH NOISE", noisy );
	namedWindow( "DENOSING", WINDOW_AUTOSIZE );
	imshow("DENOSING", dst );

	waitKey(0);
	return 0;


}

