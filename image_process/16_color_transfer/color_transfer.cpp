/*
 * color_transfer.cpp
 *
 *  Created on: May 21, 2018
 *      Author: cui
 */

#include <opencv2/opencv.hpp>
#include <opencv2/imgproc.hpp>

using namespace cv;
using namespace std;

int main( int argc, char *argv[] )
{
	Mat src, tar;
	src = imread( argv[1] );
	if( src.empty() ){
		cout << "Couldn't load image" << endl;
		return -1;
	}

	tar = imread( argv[2] );
	if( tar.empty() ){
		cout << "Couldn't load image" << endl;
		return -1;
	}

	//变换到Lab和CV_32FC1
	Mat src_lab, tar_lab;
	cvtColor( src, src_lab, COLOR_BGR2Lab );
	cvtColor( tar, tar_lab, COLOR_BGR2Lab );
	src_lab.convertTo( src_lab, CV_32FC1);
	tar_lab.convertTo( tar_lab, CV_32FC1);

	//为每幅图像找到每个通道的标准值和std值
	Mat mean_src, mean_tar, stdd_src, stdd_tar;
	meanStdDev( src_lab, mean_src, stdd_src );
	meanStdDev( tar_lab, mean_tar, stdd_tar );

	//拆分成独立的通道
	vector<Mat> src_chan, tar_chan;
	split( src_lab, src_chan );
	split( tar_lab, tar_chan );

	//为每个通道计算颜色分布
	for( int i = 0; i < 3; i++ ){
		tar_chan[i] -= mean_tar.at<double>(i);
		tar_chan[i] *= (stdd_src.at<double>(i) /stdd_src.at<double>(i) );
		tar_chan[i] += mean_src.at<double>(i);
		cout << "mean_tar[" << i << "]: " <<  mean_tar.at<double>(i) << endl;
		cout << "stdd_src.at<double>(" << i << "): " <<  stdd_src.at<double>(i) << endl;
		cout << "(stdd_src.at<double>(i) /stdd_src.at<double>(i) )" <<
				(stdd_src.at<double>(i) /stdd_src.at<double>(i) ) << endl;
	}

	//合并通道，转换到CV_8UC1的每个通道，并转换到BGR
	Mat output;
	merge( tar_chan, output );
	output.convertTo( output, CV_8UC1 );
	cvtColor( output, output, COLOR_Lab2BGR );

	namedWindow( "Source Image", WINDOW_AUTOSIZE );
	imshow("Source Image", src);
	namedWindow( "Target Image", WINDOW_AUTOSIZE );
	imshow("Target Image", tar);
	namedWindow( "Result Image", WINDOW_AUTOSIZE );
	imshow("Result Image", output);
	imwrite("result.jpg", output);
	waitKey(0);
	return 0;
}



