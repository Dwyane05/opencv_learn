
#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/features2d/features2d.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/xfeatures2d.hpp>
#include <iostream>
using namespace std;
using namespace cv;
using namespace cv::xfeatures2d;

vector<Mat> train_set;
vector<Mat> testDesc;
int main()
{
	Mat test1 = imread("caiyang1.jpg");
	Mat test2 = imread("caiyang2.jpg");
	Mat test3 = imread("caiyang3.jpg");
	Mat test4 = imread("caiyang4.jpg");
	Mat test5 = imread("caiyang5.jpg");
	Mat test6 = imread("caiyang6.jpg");
	Mat test7 = imread("caiyang7.jpg");
	Mat test8 = imread("caiyang8.jpg");
	Mat test9 = imread("caiyang9.jpg");
	Mat test10 = imread("caiyang10.jpg");
	Mat test11 = imread("caiyang11.jpg");
	Mat test12 = imread("caiyang12.jpg");
	Mat test13 = imread("caiyang13.jpg");
	Mat test14 = imread("caiyang14.jpg");
	Mat test[14] = { test1,test2,test3,test4,test5,test6,test7,test8,test9,test10,test11,test12,test13,test14 };
	for (unsigned int i = 0; i < 14; i++)
	{
		train_set.push_back(test[i]);
	}
	Ptr<SURF> surf;
	surf = SURF::create(300, 4, 3);
	vector<vector<KeyPoint> > keyPoint2;
	surf->detect(train_set, keyPoint2);
	surf->compute(train_set, keyPoint2, testDesc);

	FileStorage fswrite;
	fswrite.open("yangben.xml", FileStorage::WRITE);
	fswrite << "src1" << testDesc[0];
	fswrite << "src2" << testDesc[1];
	fswrite << "src3" << testDesc[2];
	fswrite << "src4" << testDesc[3];
	fswrite << "src5" << testDesc[4];
	fswrite << "src6" << testDesc[5];
	fswrite << "src7" << testDesc[6];
	fswrite << "src8" << testDesc[7];
	fswrite << "src9" << testDesc[8];
	fswrite << "src10" << testDesc[9];
	fswrite << "src11" << testDesc[10];
	fswrite << "src12" << testDesc[11];
	fswrite << "src13" << testDesc[12];
	fswrite << "src14" << testDesc[13];


	fswrite.release();
	cout << "write finish" << endl;

	getchar();
	return 0;
	
}
