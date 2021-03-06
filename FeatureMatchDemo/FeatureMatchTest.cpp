
#include <opencv2/xfeatures2d.hpp>
#include<opencv2/features2d/features2d.hpp>
#include <opencv2/xfeatures2d/nonfree.hpp>

#include "FeatureMatchTest.h"

using namespace std;
using namespace cv;
using namespace cv::xfeatures2d;

FeatureMatchTest::FeatureMatchTest(std::shared_ptr<Pattern> left, std::shared_ptr<Pattern> right,
		std::shared_ptr<cv::DescriptorMatcher> matcher) :
				leftPattern(left), rightPattern(right), matcher(matcher)
{
	//step1:Create detector
	//define variable and class
	int minHessian = 400;	//定义SURF中的hessian阈值特征点检测算子
	Ptr<SurfFeatureDetector> detector = SurfFeatureDetector::create(minHessian);

	//step2:Detecte keypoint
	detector->detect(leftPattern->image, leftPattern->keypoints);
	detector->detect(rightPattern->image, rightPattern->keypoints);

	//step3:Compute descriptor
	detector->compute(leftPattern->image, leftPattern->keypoints, leftPattern->descriptors);
	detector->compute(rightPattern->image, rightPattern->keypoints, rightPattern->descriptors);
	cout << "leftPattern->keypoints: " <<  leftPattern->keypoints.size() << endl;
	cout << "rightPattern->keypoints: " <<  rightPattern->keypoints.size() << endl;

	cout << "leftPattern->descriptors: " <<  leftPattern->descriptors.size() << endl;
	cout << "rightPattern->descriptors: " <<  rightPattern->descriptors.size() << endl;

}

void FeatureMatchTest::match(vector<DMatch>& matches)
{

	matcher->match(leftPattern->descriptors, rightPattern->descriptors, matches);
}

void FeatureMatchTest::knnMatch(vector<DMatch>& matches)
{

	const float minRatio = 1.f / 1.5f;
	const int k = 2;

	vector<vector<DMatch>> knnMatches;
	matcher->knnMatch(leftPattern->descriptors, rightPattern->descriptors, knnMatches, k);

	for (size_t i = 0; i < knnMatches.size(); i++) {
		const DMatch& bestMatch = knnMatches[i][0];
		const DMatch& betterMatch = knnMatches[i][1];

		float  distanceRatio = bestMatch.distance / betterMatch.distance;
		if (distanceRatio < minRatio)
			matches.push_back(bestMatch);
	}
}

void FeatureMatchTest::refineMatcheswithHomography(vector<DMatch>& matches,
		double reprojectionThreshold, Mat& homography)
{
	const int minNumbermatchesAllowed = 8;
	if (matches.size() < minNumbermatchesAllowed)
		return;

	//Prepare data for findHomography
	vector<Point2f> srcPoints(matches.size());
	vector<Point2f> dstPoints(matches.size());

	for (size_t i = 0; i < matches.size(); i++) {
		srcPoints[i] = rightPattern->keypoints[matches[i].trainIdx].pt;
		dstPoints[i] = leftPattern->keypoints[matches[i].queryIdx].pt;
	}

	//find homography matrix and get inliers mask
	vector<uchar> inliersMask(srcPoints.size());
	homography = findHomography(srcPoints, dstPoints, CV_FM_RANSAC, reprojectionThreshold,
			inliersMask);

	vector<DMatch> inliers;
	for (size_t i = 0; i < inliersMask.size(); i++){
		if (inliersMask[i])
			inliers.push_back(matches[i]);
	}
	matches.swap(inliers);
}

void FeatureMatchTest::refineMatchesWithFundmentalMatrix(vector<DMatch>& matches, Mat& F)
{
	//Align all points
	vector<KeyPoint> alignedKps1, alignedKps2;
	for (size_t i = 0; i < matches.size(); i++) {
		alignedKps1.push_back(leftPattern->keypoints[matches[i].queryIdx]);
		alignedKps2.push_back(rightPattern->keypoints[matches[i].trainIdx]);
	}

	//Keypoints to points
	vector<Point2f> ps1, ps2;
	for (unsigned i = 0; i < alignedKps1.size(); i++)
		ps1.push_back(alignedKps1[i].pt);

	for (unsigned i = 0; i < alignedKps2.size(); i++)
		ps2.push_back(alignedKps2[i].pt);

	//Compute fundmental matrix
	vector<uchar> status;
	F = findFundamentalMat(ps1, ps2, status, FM_RANSAC);

	cout << "Fundamental Matrix:" << endl;
	cout << F << endl;
	if( F.empty() ){
		cout << "Fundamental empty" << endl;
		return ;
	}
	//�Ż�ƥ����
	vector<KeyPoint> leftInlier;
	vector<KeyPoint> rightInlier;
	vector<DMatch> inlierMatch;

	int index = 0;
	for (unsigned i = 0; i < matches.size(); i++) {
		if (status[i] != 0){
			leftInlier.push_back(alignedKps1[i]);
			rightInlier.push_back(alignedKps2[i]);
			matches[i].trainIdx = index;
			matches[i].queryIdx = index;
			inlierMatch.push_back(matches[i]);
			index++;
		}
	}
	leftPattern->keypoints = leftInlier;
	rightPattern->keypoints = rightInlier;
	matches = inlierMatch;
}

void FeatureMatchTest::showMatches(const vector<DMatch>& matches,Mat& matchesImg,
		const string& windowName) {

	drawMatches(leftPattern->image, leftPattern->keypoints, rightPattern->image,
			rightPattern->keypoints, matches, matchesImg);
	namedWindow(windowName);
	imshow(windowName, matchesImg);
	waitKey();
	destroyWindow(windowName);
}

void FeatureMatchTest::draw_contour( const std::vector<cv::DMatch>& matches, Mat &show_img )
{
	std::vector<Point2f> obj;
	std::vector<Point2f> scene;

	for (int i = 0; i < matches.size(); i++)
	{
		//-- Get the keypoints from the good matches
		obj.push_back(leftPattern->keypoints[matches[i].queryIdx].pt);
		scene.push_back(rightPattern->keypoints[matches[i].trainIdx].pt);
	}

	Mat H = findHomography(obj, scene, RANSAC);

	//-- Get the corners from the image_1 ( the object to be "detected" )
	std::vector<Point2f> obj_corners(4);
	obj_corners[0] = cvPoint(0, 0);
	obj_corners[1] = cvPoint(leftPattern->image.cols, 0);
	obj_corners[2] = cvPoint(leftPattern->image.cols, leftPattern->image.rows);
	obj_corners[3] = cvPoint(0, leftPattern->image.rows);

	std::vector<Point2f> scene_corners(4);

	perspectiveTransform(obj_corners, scene_corners, H );

	//-- Draw lines between the corners (the mapped object in the scene - image_2 )
	line(show_img, scene_corners[0] + Point2f(leftPattern->image.cols, 0),
			scene_corners[1] + Point2f(leftPattern->image.cols, 0), Scalar(0, 255, 0), 4);
	line(show_img, scene_corners[1] + Point2f(leftPattern->image.cols, 0),
			scene_corners[2] + Point2f(leftPattern->image.cols, 0), Scalar(0, 255, 0), 4);
	line(show_img, scene_corners[2] + Point2f(leftPattern->image.cols, 0),
			scene_corners[3] + Point2f(leftPattern->image.cols, 0), Scalar(0, 255, 0), 4);
	line(show_img, scene_corners[3] + Point2f(leftPattern->image.cols, 0),
			scene_corners[0] + Point2f(leftPattern->image.cols, 0), Scalar(0, 255, 0), 4);

}
