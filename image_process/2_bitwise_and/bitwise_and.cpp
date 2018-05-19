#include <opencv2/opencv.hpp>

using namespace cv;
using namespace std;

int main()
{
	Mat img1 = imread( "lena.jpg", IMREAD_GRAYSCALE );
	if( img1.empty() ){
		cout << "Couldn't load image" << endl;
		return -1; 
	}
	imshow( "Original", img1 );

	Mat mask( img1.rows, img1.cols, CV_8UC1, Scalar(0,0,0) );
	circle( mask, Point(img1.rows/2, img1.cols/2), 150, 255, -1 );
	imshow( "mask", mask );

	Mat r;
	bitwise_and( img1, mask, r);
	imshow( "bitwise", r );
	
	const uchar white = 255;
	for( int i = 0; i < r.rows; i++ ){
		for( int j = 0; j < r.cols; j++ ){
			if( !r.at<uchar>(i,j) )
				r.at<uchar>(i,j) = white;
		}
	}
	imshow( "result", r );
	waitKey(0);
	return 0;
	
}
