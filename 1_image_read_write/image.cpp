#include <iostream>
#include <string>
#include <sstream>
using namespace std;

//OpenCV 头文件
#include "opencv2/core.hpp"
#include "opencv2/highgui.hpp"
using namespace cv;

int main( int argc, char *argv[] )
{
	Mat color = imread( "lena.jpg" );
	Mat gray =  imread( "lena.jpg", 0 );

	if( !color.data ){
		cerr << cv::getBuildInformation() << endl;
		fprintf( stderr, "image read error\n" );
		return -1;
	}	

	imwrite( "lenagray.jpg", gray );
#if 0
	for( int i = 0; i < color.rows; i++ ){
		for ( int j = 0; j < color.cols; j++ ){
			Vec3b	pixel = color.at<Vec3b>(i,j);
			cout << "Pixel value(B G R)" << "(" << i << "," << j << ")" << ": (" << (int)pixel[0] << ","
			<< (int)pixel[1] << "," << (int)pixel[2] << ")" << endl;
		}
	}
#endif

	int myRow = color.cols - 1;
	int myCol = color.rows - 1;
	Vec3b pixel = color.at<Vec3b>(myRow, myCol);
	cout << "Pixel value(B G R): (" << (int)pixel[0] << ","
	<< (int)pixel[1] << "," << (int)pixel[2] << ")" << endl;


//	imshow( "Lena BGR", color );
//	imshow( "Lena Gray", gray );

//	waitKey(0);
	return 0;
	
}
