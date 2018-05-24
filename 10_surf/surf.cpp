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
void refineMatcheswithHomography(vector<DMatch>& matches, double reprojectionThreshold, Mat& homography);
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

	Mat src_image1 = imread( img_file1, CV_LOAD_IMAGE_COLOR );
	Mat src_image2 = imread( img_file2, CV_LOAD_IMAGE_COLOR );
	if( src_image1.empty() || src_image2.empty() ){
		cout << "Error loading image " << endl;
		return -1;
	}

	imshow( "image1", src_image1 );
	imshow( "image2", src_image2 );

	//define variable and class
	int minHessian = 400;	//定义SURF中的hessian阈值特征点检测算子
	Ptr<SurfFeatureDetector> detector = SurfFeatureDetector::create(minHessian);

	vector<KeyPoint>	keypoints_1, keypoints_2;

	//检测特征点并放入容器
//	detector.detect( src_image1, keypoints_1 );		//2.4 version use
//	detector.detect( src_image2, keypoints_2 );
//	detector->detect(src_image1, keypoints_1);
//	detector->detect(src_image2, keypoints_2);
	Mat dstImage1, dstImage2;
	detector->detectAndCompute(src_image1, Mat(), keypoints_1,dstImage1);
	detector->detectAndCompute(src_image2, Mat(), keypoints_2,dstImage2);

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

	Ptr<DescriptorMatcher>	matcher = DescriptorMatcher::create( "FlannBased" );
	vector<DMatch>	mach;
	matcher->match( dstImage1, dstImage2, mach );

	//so far, match features end; but maybe has fault descriptors;
	//no problem, we just see all match descriptors
	Mat img_all_matches;
	drawMatches( src_image1, keypoints_1, src_image2, keypoints_2, mach, img_all_matches );
	imshow( "all matches", img_all_matches );
	cout << "all match descriptors: " << mach.size() << endl;

/*
 * KNNMatch匹配过程中很可能发生错误的匹配，错误的匹配主要有两种：匹配的特征点事错误的，图像上的特征点无法匹配。
 * 常用的删除错误的匹配有
 * 1.交叉过滤
 * 如果第一幅图像的一个特征点和第二幅图像的一个特征点相匹配，则进行一个相反的检查，即将第二幅图像上的特征点与
 * 第一幅图像上相应特征点进行匹配，如果匹配成功，则认为这对匹配是正确的。
 * OpenCV中的BFMatcher已经包含了这种过滤   BFMatcher matcher(NORM_L2,true)，在构造BFMatcher是
 * 将第二个参数设置为true。
 *
 * 2.比率测试
 * KNNMatch，可设置K = 2 ，即对每个匹配返回两个最近邻描述符，仅当第一个匹配与第二个匹配之间的距离足够小时，
 * 才认为这是一个匹配。
 */
	//next, use 比率测试
	double max_dist = 0;
	double min_dist = 100;
	for( int i = 0; i < dstImage1.rows; i++ ){
		double dist = mach[i].distance;
		if( dist < min_dist ) min_dist = dist;
		if( dist > max_dist ) max_dist = dist;
	}
	cout << "minimum distance: " << min_dist << endl;
	cout << "maximum distance: " << max_dist << endl;

	vector<DMatch>	goodmatches;
	for( int i = 0; i < dstImage1.rows; i++ ){
		if( mach[i].distance < 2*min_dist )
			goodmatches.push_back( mach[i] );
	}

	Mat img_match;
	drawMatches( src_image1, keypoints_1, src_image2, keypoints_2, goodmatches, img_match );
//	drawMatches( src_image1, keypoints_1, src_image2, keypoints_2, mach, img_match );
	for( int i = 0; i < goodmatches.size(); i++ ){
		cout << "Matching conditions：" << goodmatches[i].queryIdx << "--" << goodmatches[i].trainIdx << endl;
	}
	cout << "rate filter--match descriptors: " << goodmatches.size() << endl;
	imshow("good match result",img_match );

	//RASIC方法计算基础矩阵，并细化匹配结果
	//Align all points
	vector<KeyPoint> alignedKps1, alignedKps2;
	for (int i = 0; i < goodmatches.size(); i++) {
		alignedKps1.push_back(keypoints_1[goodmatches[i].queryIdx]);
		alignedKps2.push_back(keypoints_2[goodmatches[i].trainIdx]);
	}

	//Keypoints to points
	vector<Point2f> ps1, ps2;
	for (unsigned i = 0; i < alignedKps1.size(); i++)
		ps1.push_back(alignedKps1[i].pt);

	for (unsigned i = 0; i < alignedKps2.size(); i++)
		ps2.push_back(alignedKps2[i].pt);

	//Compute fundmental matrix
	vector<uchar> status;
	Mat F = findFundamentalMat(ps1, ps2, status, FM_RANSAC);

	cout << "Fundmental Matric:" << endl;
	cout << F << endl;

	//使用RANSAC(随机抽样一致性)方法计算基础矩阵后可以得到一个status向量，用来删除错误的匹配
	vector<KeyPoint> leftInlier;
	vector<KeyPoint> rightInlier;
	vector<DMatch> inlierMatch;

	int index = 0;
	for (unsigned i = 0; i < goodmatches.size(); i++) {
		if (status[i] != 0){
			leftInlier.push_back(alignedKps1[i]);
			rightInlier.push_back(alignedKps2[i]);
			goodmatches[i].trainIdx = index;
			goodmatches[i].queryIdx = index;
			inlierMatch.push_back(goodmatches[i]);
			index++;
		}
	}
	keypoints_1 = leftInlier;
	keypoints_2 = rightInlier;
	goodmatches = inlierMatch;
	//now show F result
	Mat img_f_matches;
	drawMatches( src_image1, keypoints_1, src_image2, keypoints_2, goodmatches, img_f_matches );
	cout << "F filter--match descriptors: " << goodmatches.size() << endl;
	imshow( "F matches", img_f_matches );

	waitKey(0);
	//计算单应矩阵H，并细化匹配结果
	const int minNumbermatchesAllowed = 8;
	if (goodmatches.size() < minNumbermatchesAllowed){
		cout << "H filter: dots too little" << endl;
		return -1;
	}
	//Prepare data for findHomography
	vector<Point2f> srcPoints(goodmatches.size());
	vector<Point2f> dstPoints(goodmatches.size());

	for (size_t i = 0; i < goodmatches.size(); i++) {
		srcPoints[i] = keypoints_1[goodmatches[i].trainIdx].pt;
		dstPoints[i] = keypoints_2[goodmatches[i].queryIdx].pt;
	}
	//find homography matrix and get inliers mask
	double reprojectionThreshold = 3.0;
	vector<uchar> inliersMask(srcPoints.size());
	Mat homography = findHomography(srcPoints, dstPoints, CV_FM_RANSAC, reprojectionThreshold,
			inliersMask);

	vector<DMatch> inliers;
	for (int i = 0; i < inliersMask.size(); i++){
		if (inliersMask[i])
			inliers.push_back(goodmatches[i]);
	}
	goodmatches.swap(inliers);

	//now show F result
	Mat img_h_matches;
	drawMatches( src_image1, keypoints_1, src_image2, keypoints_2, goodmatches, img_h_matches );
	cout << "H filter--match descriptors: " << goodmatches.size() << endl;
	imshow( "H matches", img_h_matches );
	waitKey(0);
	return 0;

}
