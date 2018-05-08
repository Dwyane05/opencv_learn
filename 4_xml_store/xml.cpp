#include <iostream>
#include <sstream>
#include <string>

using namespace std;
#include "opencv2/opencv.hpp"
#include "opencv2/core.hpp"
using namespace cv;

int main( int argc, char *argv[] )
{
	//create write
	FileStorage fs( "test.yml",FileStorage::WRITE );
	//save as int
	int fps = 5;
	fs << "fps" << fps;

	//create Mat sample
	Mat m1 = Mat::eye( 2, 3, CV_32F );
	Mat m2 = Mat::ones( 3, 2, CV_32F );
	Mat result = (m1+1).mul(m1+3);
	//print result
	fs << "Result" << result;

	//release file
	fs.release();

	FileStorage fs2( "test.yml", FileStorage::READ );

	Mat r;
	fs2["Result"] >> r;
	cout << r << endl;
	fs2.release();
	return 0;
}

