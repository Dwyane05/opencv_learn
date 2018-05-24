#include "FeatureMatchTest.h"
#include <opencv2\nonfree\features2d.hpp>

using namespace std;
using namespace cv;

FeatureMatchTest::FeatureMatchTest(std::shared_ptr<Pattern> left, std::shared_ptr<Pattern> right, std::shared_ptr<cv::DescriptorMatcher> matcher) :
leftPattern(left), rightPattern(right), matcher(matcher) {

	//step1:Create detector
	int minHessian = 400;
	SurfFeatureDetector detector(minHessian);

	//step2:Detecte keypoint
	detector.detect(leftPattern->image, leftPattern->keypoints);
	detector.detect(rightPattern->image, rightPattern->keypoints);

	//step3:Compute descriptor
	detector.compute(leftPattern->image, leftPattern->keypoints, leftPattern->descriptors);
	detector.compute(rightPattern->image, rightPattern->keypoints, rightPattern->descriptors);
}

void FeatureMatchTest::match(vector<DMatch>& matches) {

	matcher->match(leftPattern->descriptors, rightPattern->descriptors, matches);
}

void FeatureMatchTest::knnMatch(vector<DMatch>& matches) {

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

void FeatureMatchTest::refineMatcheswithHomography(vector<DMatch>& matches, double reprojectionThreshold, Mat& homography){
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
	homography = findHomography(srcPoints, dstPoints, CV_FM_RANSAC, reprojectionThreshold, inliersMask);

	vector<DMatch> inliers;
	for (size_t i = 0; i < inliersMask.size(); i++){
		if (inliersMask[i])
			inliers.push_back(matches[i]);
	}
	matches.swap(inliers);
}

void FeatureMatchTest::refineMatchesWithFundmentalMatrix(vector<DMatch>& matches, Mat& F) {
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

	cout << "Fundmental Matric:" << endl;
	cout << F << endl;

	//优化匹配结果
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

/*void FeatureMatchTest::showMatches(const vector<DMatch>& matches,Mat& matchesImg, const string& windowName) {

	drawMatches(leftPattern->image, leftPattern->keypoints, rightPattern->image, rightPattern->keypoints, matches, matchesImg);
	namedWindow(windowName);
	imshow(windowName, matchesImg);
	waitKey();
	destroyWindow(windowName);
}*/