

#include <iostream>
#include <opencv.hpp>
#include <string>
#include <opencv2\features2d\features2d.hpp>
#include <opencv2\nonfree\features2d.hpp>

#include "FeatureMatchTest.h"

using namespace std;
using namespace cv;

void featureMatchDemo();

int main()
{
	
}

void featureMatchDemo()
{
	const string imgName1 = "x://image//templeSR0001.png";
	const string imgName2 = "x://image//templeSR0002.png";

	Mat img1 = imread(imgName1);
	Mat img2 = imread(imgName2);

	if (!img1.data || !img2.data)
		return -1;

	shared_ptr<Pattern>  leftPattern = make_shared<Pattern>(img1);
	shared_ptr<Pattern> rightPattern = make_shared<Pattern>(img2);
	vector<DMatch> matches;

	shared_ptr<DescriptorMatcher> matcher = make_shared<BFMatcher>(NORM_L2);
	FeatureMatchTest ft(leftPattern, rightPattern, matcher);
	//ft.match(matches);
	ft.knnMatch(matches);
	//Mat F;
	//ft.refineMatchesWithFundmentalMatrix(matches,F);

	//Mat h;
	//ft.refineMatcheswithHomography(matches, 3.0, h);
	Mat outImg;
	drawMatches(leftPattern->image, leftPattern->keypoints, rightPattern->image, rightPattern->keypoints, matches, outImg);
	namedWindow("Matches");
	imshow("Matches", outImg);
	waitKey();

}