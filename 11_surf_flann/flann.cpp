/*
 * flann.cpp
 *
 *  Created on: May 23, 2018
 *      Author: cui
 */

#include <opencv2/features2d/features2d.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/xfeatures2d/nonfree.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc.hpp>

#include<iostream>
using namespace std;
using namespace cv;
using namespace cv::xfeatures2d;
//-lopencv_core -lopencv_video -lopencv_videoio  -lopencv_highgui -lopencv_imgcodecs -lopencv_imgproc
//-lopencv_features2d -lopencv_xfeatures2d -lopencv_flann -lopencv_ml -lopencv_shape -lopencv_calib3d -L${LIBS_PATH}/ingenic/opencv320/lib

// OpenCV command line parser functions
// Keys accecpted by command line parser
const char* keys =
{
   "{help h usage ? | | print this message}"
   "{@image1 || Image1 to process}"
   "{@image2 || Image2 to process}"
};

int main( int argc, char *argv[] )
{
	CommandLineParser parser(argc, argv, keys);
	parser.about("Surf. PhotoTool v1.0.0");
	//If requires help show
	if(parser.has("help")){
	  parser.printMessage();
	  return 0;
	}

	String img_file1= parser.get<String>(0);
	String img_file2= parser.get<String>(1);
	cout << "img_file: " << img_file1 << endl;
	cout << "img_file2: " << img_file2 << endl;

	// Check if params are correctly parsed in his variables
	if(!parser.check()){
	  parser.printErrors();
	  return 0;
	}

	Mat train_image1 = imread( img_file1, IMREAD_GRAYSCALE );
	Mat testImage_gray = imread( img_file2, IMREAD_GRAYSCALE );
	if( train_image1.empty() || testImage_gray.empty() ){
		cout << "Error loading image " << endl;
		return -1;
	}

	imshow( "train image", train_image1 );
	imshow( "test image", testImage_gray );

	double t0 = (double)getTickCount();
	//检测surf关键点、提取训练图像描述符并放入容器
	int minHessian = 80;	//定义SURF中的hessian阈值特征点检测算子
	Ptr<SurfFeatureDetector> detector = SurfFeatureDetector::create(minHessian);
	vector<KeyPoint>	train_keypoint;

	detector->detect(train_image1, train_keypoint );


	//-- Step 2: Calculate descriptors (feature vectors)   提取特征描述符，放入Mat变量
	// Based on http://docs.opencv.org/3.2.0/d9/d97/tutorial_table_of_content_features2d.html
	Ptr<SurfDescriptorExtractor> extractor = SurfDescriptorExtractor::create();
	Mat train_descriptor;
	extractor->compute(train_image1, train_keypoint, train_descriptor);

	//-- Step 3: Matching descriptor vectors using FLANN matcher
	//使用flann匹配器匹配描述符向量
	FlannBasedMatcher matcher;
	vector< Mat > train_desc_collection(1, train_descriptor);
	matcher.add( train_desc_collection );
	matcher.train();
	double elapsed = ((double)getTickCount()-t0)/getTickFrequency();
	cout << "train elapsed: " << elapsed << endl;

#if 1
	//match
	//<3>检测S关键点、提取测试图像描述符
	vector<KeyPoint>	test_keypoint;
	Mat test_decriptor;
	detector->detect(testImage_gray, test_keypoint );
	extractor->compute(testImage_gray, test_keypoint, test_decriptor);

	//<4>匹配训练和测试描述符
	vector<vector<DMatch> >	matches;
	matcher.knnMatch( test_decriptor, matches, 2 );
	cout << "knnmatches numbers: " << matches.size()-1 << endl;

	//<5>根据劳式算法，得到优秀的匹配点
	vector<DMatch>	good_matches;
	for( int i = 0; i < matches.size(); i++ ){
		if( matches[i][0].distance < 0.6 * matches[i][1].distance )
			good_matches.push_back( matches[i][0] );
	}
	cout << "good matches numbers: " << good_matches.size()-1 << endl;
	//<6>绘制匹配点并显示窗口
	Mat dst_img;
	drawMatches( testImage_gray, test_keypoint, train_image1, train_keypoint,
			good_matches, dst_img );
	imshow( "match window", dst_img );

	waitKey(0);
#else
	//创建视频对象，定义帧率
	unsigned int framecount = 0;
	VideoCapture cap(0);
	if( !cap.isOpened() ){
		cout << "Can't open video \n" << endl;
		return -1;
	}

	while( 1 ){
		//<1>参数设置
		int64 time0 = getTickCount();
		Mat testImage, testImage_gray;
		cap >> testImage;
		if( testImage.empty() )
			continue;
		//<2>转换为灰度图
		cvtColor( testImage, testImage_gray, COLOR_BGR2GRAY );

		//<3>检测S关键点、提取测试图像描述符
		vector<KeyPoint>	test_keypoint;
		Mat test_decriptor;
		detector->detect(testImage_gray, test_keypoint );
		extractor->compute(testImage_gray, test_keypoint, test_decriptor);

		//<4>匹配训练和测试描述符
		vector<vector<DMatch> >	matches;
		matcher.knnMatch( test_decriptor, matches, 2 );

		//<5>根据劳式算法，得到优秀的匹配点
		vector<DMatch>	good_matches;
		for( int i = 0; i < matches.size(); i++ ){
			if( matches[i][0].distance < 0.6 * matches[i][1].distance )
				good_matches.push_back( matches[i][0] );
		}

		//<6>绘制匹配点并显示窗口
		Mat dst_img;
		drawMatches( testImage_gray, test_keypoint, train_image1, train_keypoint,
				good_matches, dst_img );
		imshow( "match window", dst_img );

		//<7>输出帧率信息
		cout << "current frame: " << getTickFrequency()/(getTickCount() - time0) << endl;
	}
#endif

	return 0;

}



