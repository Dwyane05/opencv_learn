/*
 * color_sapace.cpp
 *
 *  Created on: May 21, 2018
 *      Author: cui
 */

#include <opencv2/opencv.hpp>
#include <opencv2/imgproc.hpp>

using namespace cv;
using namespace std;

vector<Mat> showSeparatedChannels( vector<Mat> channels );

int main( int argc, char *argv[] )
{
	Mat image;
	image = imread( argv[1] );
	if( image.empty() ){
		cout << "Couldn't load image" << endl;
		return -1;
	}
	namedWindow( "Picture", WINDOW_AUTOSIZE );
	imshow("Picture", image);

	//split channels
	vector<Mat> channels;
	split( image, channels );

//	namedWindow( "Blue channel(gray)", WINDOW_AUTOSIZE );
//	imshow( "Blue channel(gray)", channels[0] );
//	namedWindow( "Green channel(gray)", WINDOW_AUTOSIZE );
//	imshow( "Green channel(gray)", channels[1] );
//	namedWindow( "Red channel(gray)", WINDOW_AUTOSIZE );
//	imshow( "Red channel(gray)", channels[2] );

	//显示BGR中的通道
	vector<Mat> separatedChannels = showSeparatedChannels( channels );

//	namedWindow( "Blue channel", WINDOW_AUTOSIZE );
//	imshow( "Blue channel", separatedChannels[0] );
//	namedWindow( "Green channel", WINDOW_AUTOSIZE );
//	imshow( "Green channel", separatedChannels[1] );
//	namedWindow( "Red channel", WINDOW_AUTOSIZE );
//	imshow( "Red channel", separatedChannels[2] );

	//imggray
	Mat imggray;
	cvtColor( image, imggray, COLOR_BGR2GRAY );
//	namedWindow( "Gray pic", WINDOW_AUTOSIZE );
//	imshow( "Gray pic", imggray );

	//CIExyz示例代码
	Mat imgxyz;
	cvtColor( image, imgxyz, COLOR_BGR2XYZ );

	vector<Mat> xyz_channels;
	split( imgxyz, xyz_channels );
//	namedWindow( "X xyz_channel(gray)", WINDOW_AUTOSIZE );
//	imshow( "X xyz_channel(gray)", xyz_channels[0] );
//	namedWindow( "Y xyz_channel(gray)", WINDOW_AUTOSIZE );
//	imshow( "Y xyz_channel(gray)", xyz_channels[1] );
//	namedWindow( "Z xyz_channel(gray)", WINDOW_AUTOSIZE );
//	imshow( "Z xyz_channel(gray)", xyz_channels[2] );

	vector<Mat> xyz_separatedChannels = showSeparatedChannels( xyz_channels );
	for( int i = 0; i < 3; i++ ){
		cvtColor( xyz_separatedChannels[i], xyz_separatedChannels[i], COLOR_XYZ2BGR );
	}
//	namedWindow( "X channel", WINDOW_AUTOSIZE );
//	imshow( "X channel", xyz_separatedChannels[0] );
//	namedWindow( "Y channel", WINDOW_AUTOSIZE );
//	imshow( "Y channel", xyz_separatedChannels[1] );
//	namedWindow( "Z channel", WINDOW_AUTOSIZE );
//	imshow( "Z channel", xyz_separatedChannels[2] );

	//YCrCb颜色代码示例
	Mat imgyuv;
	cvtColor( image, imgyuv, COLOR_BGR2YCrCb );

	vector<Mat> yuv_channels;
	split( imgyuv, yuv_channels );

//	namedWindow( "Y yuv_channel(gray)", WINDOW_AUTOSIZE );
//	imshow( "Y yuv_channel(gray)", yuv_channels[0] );
//	namedWindow( "Cr yuv_channel(gray)", WINDOW_AUTOSIZE );
//	imshow( "Cr yuv_channel(gray)", yuv_channels[1] );
//	namedWindow( "Cb yuv_channel(gray)", WINDOW_AUTOSIZE );
//	imshow( "Cb yuv_channel(gray)", yuv_channels[2] );

	vector<Mat> yuv_separatedChannels = showSeparatedChannels( xyz_channels );
	for( int i = 0; i < 3; i++ ){
		cvtColor( yuv_separatedChannels[i], yuv_separatedChannels[i], COLOR_YCrCb2BGR );
	}
//	namedWindow( "Y channel", WINDOW_AUTOSIZE );
//	imshow( "Y channel", yuv_separatedChannels[0] );
//	namedWindow( "Cr channel", WINDOW_AUTOSIZE );
//	imshow( "Cr channel", yuv_separatedChannels[1] );
//	namedWindow( "Cb channel", WINDOW_AUTOSIZE );
//	imshow( "Cb channel", yuv_separatedChannels[2] );

	//选取指定像素
	Mat bw1;
	inRange(imgyuv, Scalar(0,133,77), Scalar(255,173,177), bw1 );
	namedWindow( "Selected pixels(YCrCb)", WINDOW_AUTOSIZE );
	imshow( "Selected pixels(YCrCb)", bw1 );

	//HSV颜色代码示例
	Mat imghsv;
	cvtColor( image, imghsv, COLOR_BGR2HSV );

	vector<Mat> hsv_channels;
	split( imghsv, hsv_channels );
//	namedWindow( "H hsv_channels(gray)", WINDOW_AUTOSIZE );
//	imshow( "H hsv_channels(gray)", hsv_channels[0] );
//	namedWindow( "S hsv_channels(gray)", WINDOW_AUTOSIZE );
//	imshow( "S hsv_channels(gray)", hsv_channels[1] );
//	namedWindow( "V hsv_channels(gray)", WINDOW_AUTOSIZE );
//	imshow( "V hsv_channels(gray)", hsv_channels[2] );
//
//	namedWindow( "HSV image(all channels)", WINDOW_AUTOSIZE );
//	imshow( "HSV image(all channels)", imghsv );

	//选取指定像素
	Mat bw;
	inRange(imghsv, Scalar(0,10,60), Scalar(20,150,250), bw );
	namedWindow( "Selected pixels", WINDOW_AUTOSIZE );
	imshow( "Selected pixels", bw );

	//HLS颜色代码示例
	Mat imghls;
	cvtColor( image, imghls, COLOR_BGR2HSV );

	vector<Mat> hls_channels;
	split( imghls, hls_channels );
//	namedWindow( "H hls_channels(gray)", WINDOW_AUTOSIZE );
//	imshow( "H hls_channels(gray)", hls_channels[0] );
//	namedWindow( "L hls_channels(gray)", WINDOW_AUTOSIZE );
//	imshow( "L hls_channels(gray)", hls_channels[1] );
//	namedWindow( "S hls_channels(gray)", WINDOW_AUTOSIZE );
//	imshow( "S hls_channels(gray)", hls_channels[2] );
//
//	namedWindow( "HLS image(all channels)", WINDOW_AUTOSIZE );
//	imshow( "HLS image(all channels)", imghls );

	//CIE L*a*b 颜色代码示例
	Mat imglab;
	cvtColor( image, imglab, COLOR_BGR2Lab );
	vector<Mat> lab_channels;
	split( imglab, lab_channels );

//	namedWindow( "L channels(gray)", WINDOW_AUTOSIZE );
//	imshow( "L channels(gray)", lab_channels[0] );
//	namedWindow( "A channels(gray)", WINDOW_AUTOSIZE );
//	imshow( "A channels(gray)", lab_channels[1] );
//	namedWindow( "B channels(gray)", WINDOW_AUTOSIZE );
//	imshow( "B channels(gray)", lab_channels[2] );
//
//	namedWindow( "Lab image(all channels)", WINDOW_AUTOSIZE );
//	imshow( "Lab image(all channels)", imglab );

	//CIE L*a*b 颜色代码示例
	Mat imgluv;
	cvtColor( image, imgluv, COLOR_BGR2Luv );
	vector<Mat> luv_channels;
	split( imgluv, luv_channels );

//	namedWindow( "L channels(gray)", WINDOW_AUTOSIZE );
//	imshow( "L channels(gray)", luv_channels[0] );
//	namedWindow( "U channels(gray)", WINDOW_AUTOSIZE );
//	imshow( "U channels(gray)", luv_channels[1] );
//	namedWindow( "V channels(gray)", WINDOW_AUTOSIZE );
//	imshow( "V channels(gray)", luv_channels[2] );
//
//	namedWindow( "Luv image(all channels)", WINDOW_AUTOSIZE );
//	imshow( "Luv image(all channels)", imgluv );

	waitKey(0);
	return 0;
}

vector<Mat> showSeparatedChannels( vector<Mat> channels )
{
	vector<Mat> separatedChannels;
	//创建每幅图像的每一个通道
	for( int i = 0; i < 3; i++ ){
		Mat zer = Mat::zeros( channels[0].rows, channels[0].cols,
				channels[0].type() );
		vector<Mat> aux;
		for( int j = 0; j < 3; j++ ){
			if( j == i )
				aux.push_back(channels[i]);
			else
				aux.push_back(zer);
		}

		Mat chann;
		merge( aux, chann );

		separatedChannels.push_back( chann);
	}

	return separatedChannels;
}

