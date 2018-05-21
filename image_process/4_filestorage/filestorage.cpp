
#include <opencv2/opencv.hpp>
using namespace cv;
using namespace std;

Mat img1;

void tb1_Callback( int value, void * )
{
	Mat tmp = img1 + value;
	imshow( "main_win", tmp );
}

int main()
{
	img1 = imread( "lena.jpg", IMREAD_GRAYSCALE );
	if( img1.empty() ){
		cout << "Couldn't load image" << endl;
		return -1;
	}
	
	int tb1_init = 100;
	FileStorage file1( "config.xml", FileStorage::WRITE );
	file1 << "tb1_value" << tb1_init;
	file1.release();
	cout << "tb1_value write to file 100" << endl;

	int tb1_value = 1;
	FileStorage fs1( "config.xml", FileStorage::READ );
	tb1_value = fs1["tb1_value"];
	fs1.release();
	cout << "file value: " << tb1_value << endl;

	namedWindow("main_win");
	createTrackbar( "brightness", "main_win", &tb1_value,
		255, tb1_Callback );
	tb1_Callback( tb1_value, NULL );
	
	waitKey( 0 );

	FileStorage fs2( "config.xml", FileStorage::WRITE );
	fs2 << "tb1_value" << tb1_value;
	fs2.release();

	return 0; 
}
