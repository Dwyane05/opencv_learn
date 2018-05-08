#include <iostream>
#include <sstream>
#include <string>

using namespace std;
#include "opencv2/core.hpp"
#include "opencv2/highgui.hpp"
using namespace cv;

//const int CV_GUI_NORMAL = 0x10;

int main( int argc, char *argv[] )
{
	//read image
	Mat lena = imread( "lena.jpg" );
	Mat photo = imread( "photo.jpg" );
	
	if( !lena.data || !photo.data ){
		fprintf( stderr, "Please check image isn't exist\n" );
		return -1;
	}
	
	//create window
	namedWindow( "Lena", CV_GUI_NORMAL );
	namedWindow( "Photo", WINDOW_AUTOSIZE );
	//move
	moveWindow( "Lena", 10, 10 );
	moveWindow( "Phtot", 520, 10 );
	//show
	imshow( "Lena", lena );
	imshow( "Photo", photo );

	resizeWindow( "Lena", 512, 512 );

	waitKey(0);

	//destrory window
	destroyWindow( "Lena" );
	destroyWindow( "Photo" );
	
	//create 10 windows
	for( int i = 0; i < 10; i++ ){
		ostringstream ss;
		ss << "Photo " << i;
		namedWindow( ss.str() );
		moveWindow( ss.str(), 20*i, 20*i );
		imshow( ss.str(), photo );
	}
	waitKey(0);
	destroyAllWindows();
	return 0;
	return 0;
}
