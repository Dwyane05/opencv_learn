

#include <iostream>
#include <opencv2/opencv.hpp>
#include <string>
#include <opencv2/features2d/features2d.hpp>
#include <opencv2/xfeatures2d/nonfree.hpp>

#include "FeatureMatchTest.h"

using namespace std;
using namespace cv;

void featureMatchDemo( String &src, String &test );

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
	if( parser.has("help") ){
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
	featureMatchDemo( img_file1, img_file2 );
	cout << "main end" << endl;
	return 0;
	
}

void featureMatchDemo( String &src, String &test )
{
	const string imgName1 = src;
	const string imgName2 = test;

	Mat img1 = imread(imgName1);
	Mat img2 = imread(imgName2);

	if (!img1.data || !img2.data)
		return ;

	shared_ptr<Pattern>  leftPattern = make_shared<Pattern>(img1);
	shared_ptr<Pattern> rightPattern = make_shared<Pattern>(img2);
	vector<DMatch> matches;

	shared_ptr<DescriptorMatcher> matcher = make_shared<BFMatcher>(NORM_L2);
	FeatureMatchTest ft(leftPattern, rightPattern, matcher);
	//ft.match(matches);
	ft.knnMatch(matches);
	cout << "knnMatch numbers: " << matches.size() << endl;
	if( matches.size() == 0 ){
		cout << "knnMatch no pointers" << endl;
		return;
	}

	Mat F;
	ft.refineMatchesWithFundmentalMatrix(matches,F);
	cout << "Fundamental numbers: " << matches.size() << endl;
	if( matches.size() == 0 ){
		cout << "Fundamental no pointers" << endl;
		return;
	}

	Mat h;
	ft.refineMatcheswithHomography(matches, 3.0, h);
	cout << "Homography numbers: " << matches.size() << endl;
	if( matches.size() == 0 ){
		cout << "Homography no pointers" << endl;
		return;
	}

	Mat matches_img;
	string win_name = "Func_Matches";

	ft.showMatches( matches, matches_img, win_name);

	ft.draw_contour( matches, matches_img );
	imshow( "contour result", matches_img );
	waitKey(0);

#if 0
	Mat outImg;
	drawMatches(leftPattern->image, leftPattern->keypoints, rightPattern->image, rightPattern->keypoints, matches, outImg);
	namedWindow("Matches");
	imshow("Matches", outImg);
	waitKey(0);
#endif
}
