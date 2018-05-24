/*
 * surf.cpp
 *
 *  Created on: May 22, 2018
 *      Author: cui
 */

#include<opencv2/features2d/features2d.hpp>
#include<opencv2/opencv.hpp>
#include<opencv2/highgui/highgui.hpp>
#include<opencv2/xfeatures2d/nonfree.hpp>
#include<opencv2/core/core.hpp>

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
	Mat test_image2 = imread( img_file2, IMREAD_GRAYSCALE );
	if( train_image1.empty() || test_image2.empty() ){
		cout << "Error loading image " << endl;
		return -1;
	}

	imshow( "image1", train_image1 );
	imshow( "image2", test_image2 );

	//检测surf关键点、提取训练图像描述符并放入容器
	int minHessian = 80;	//定义SURF中的hessian阈值特征点检测算子
	Ptr<SurfFeatureDetector> detector = SurfFeatureDetector::create(minHessian);
	vector<KeyPoint>	train_keypoint, test_keypoint;

	detector->detect(train_image1, train_keypoint );
	detector->detect(test_image2, test_keypoint );

	//-- Step 2: Calculate descriptors (feature vectors)   提取特征描述符，放入Mat变量
	// Based on http://docs.opencv.org/3.2.0/d9/d97/tutorial_table_of_content_features2d.html
	Ptr<SurfDescriptorExtractor> extractor = SurfDescriptorExtractor::create();
	Mat train_descriptor, test_descriptor;
	extractor->compute(train_image1, train_keypoint, train_descriptor);
	extractor->compute(test_image2, test_keypoint, test_descriptor);

	//-- Step 3: Matching descriptor vectors using FLANN matcher
	//使用flann匹配器匹配描述符向量
	FlannBasedMatcher matcher;
	vector< DMatch > matches;
	matcher.match(train_descriptor, test_descriptor, matches);

#if 0
	//检测特征点并放入容器
	Mat img_keypoints1;
	Mat img_keypoints2;
	drawKeypoints(src_image1, keypoints_1, img_keypoints1,
			Scalar::all(-1), DrawMatchesFlags::DEFAULT );
	drawKeypoints(src_image2, keypoints_2, img_keypoints2,
			Scalar::all(-1), DrawMatchesFlags::DEFAULT );

	imshow( "surf1",  img_keypoints1 );
	imshow( "surf2",  img_keypoints2 );
#endif

//	Ptr<DescriptorMatcher>	matcher = DescriptorMatcher::create( "FlannBased" );
//	vector<DMatch>	mach;
//	matcher->match( dstImage1, dstImage2, mach );

	double max_dist = 0;
	double min_dist = 100;
	for( int i = 0; i < train_descriptor.rows; i++ ){
		double dist = matches[i].distance;
		if( dist < min_dist ) min_dist = dist;
		if( dist > max_dist ) max_dist = dist;
	}
	cout << "minimum distance: " << min_dist << endl;
	cout << "maximum distance: " << max_dist << endl;

	vector<DMatch>	goodmatches;
	for( int i = 0; i < train_descriptor.rows; i++ ){
		if( matches[i].distance < 2*min_dist )
			goodmatches.push_back( matches[i] );
	}

	Mat img_match;
//	drawMatches( src_image1, keypoints_1, src_image2, keypoints_2, goodmatches, img_match );
	drawMatches( train_image1, train_keypoint, test_image2, test_keypoint,
			goodmatches, img_match, Scalar::all(-1), Scalar::all(-1),
			std::vector<char>(), DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS);
	//-- Localize the object
	std::vector<Point2f> obj;
	std::vector<Point2f> scene;

	for (int i = 0; i < goodmatches.size(); i++)
	{
		//-- Get the keypoints from the good matches
		obj.push_back(train_keypoint[goodmatches[i].queryIdx].pt);
		scene.push_back(test_keypoint[goodmatches[i].trainIdx].pt);
	}

	Mat H = findHomography(obj, scene, RANSAC);

	//-- Get the corners from the image_1 ( the object to be "detected" )
	std::vector<Point2f> obj_corners(4);
	obj_corners[0] = cvPoint(0, 0); obj_corners[1] = cvPoint(train_image1.cols, 0);
	obj_corners[2] = cvPoint(train_image1.cols, train_image1.rows); obj_corners[3] = cvPoint(0, train_image1.rows);
	std::vector<Point2f> scene_corners(4);

	perspectiveTransform(obj_corners, scene_corners, H);

	//-- Draw lines between the corners (the mapped object in the scene - image_2 )
	line(img_match, scene_corners[0] + Point2f(train_image1.cols, 0), scene_corners[1] + Point2f(train_image1.cols, 0), Scalar(0, 255, 0), 4);
	line(img_match, scene_corners[1] + Point2f(train_image1.cols, 0), scene_corners[2] + Point2f(train_image1.cols, 0), Scalar(0, 255, 0), 4);
	line(img_match, scene_corners[2] + Point2f(train_image1.cols, 0), scene_corners[3] + Point2f(train_image1.cols, 0), Scalar(0, 255, 0), 4);
	line(img_match, scene_corners[3] + Point2f(train_image1.cols, 0), scene_corners[0] + Point2f(train_image1.cols, 0), Scalar(0, 255, 0), 4);

	//-- Show detected matches
	namedWindow("image", WINDOW_NORMAL);
	imshow("image", img_match);
	// scale image to fit on screen
	resizeWindow("image", 800, 600);

	waitKey(0);
	return 0;

}

