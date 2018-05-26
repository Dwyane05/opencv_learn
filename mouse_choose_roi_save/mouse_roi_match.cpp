/*
 * mouse_roi.cpp
 *
 *  Created on: May 25, 2018
 *      Author: cui
 */

#include<opencv2/opencv.hpp>
#include<iostream>
#include"opencv2/xfeatures2d.hpp"
#include<opencv2/highgui/highgui.hpp>

using namespace cv;
using namespace std;
using namespace cv::xfeatures2d;
using namespace cv::ml;

#define WINDOW_NAME "【程序窗口】"

void on_MouseHandle(int event, int x, int y, int flags, void*param);
void DrawRectangle(cv::Mat& img, cv::Rect box);

Rect g_rectangle;
bool g_bDrawingBox = false;//是否进行绘制
RNG g_rng(12345);

int main(int argc, char **argv)
{
	//【1】准备参数
	Mat srcImage = imread("1.jpg");
	if( !srcImage.data ){
		cout << "Could load image: 1.jpg" << endl;
		return -1;
	}
	g_rectangle = Rect(-1, -1, 0, 0);

	//【2】设置鼠标操作回调函数
	namedWindow(WINDOW_NAME);
	setMouseCallback(WINDOW_NAME, on_MouseHandle, (void*)&srcImage);

	while (1)
	{
		if (waitKey(10) == 27) { break; }//esc键，程序退出
		imshow(WINDOW_NAME, srcImage);

	}

	//ROI
	Mat imageROI = srcImage(g_rectangle);
	Mat imageROIG;
		//预备
   cvtColor ( imageROI ,imageROIG, CV_RGB2GRAY);

	//SURF
	Mat a = imageROIG;    //读取灰度图像
	Mat b = imread("2.jpg", 0);
	if( !b.data ){
		cout << "Could load image: 2.jpg" << endl;
		return -1;
	}
	Ptr<SURF> surf;                   //创建方式和opencv2中的不一样
									  //    Ptr<SIFT> sift;
	surf = SURF::create(900, 5, 4);       //阈值

	BFMatcher matcher;                //匹配器
	Mat c, d;
	vector<KeyPoint> key1, key2;
	vector<DMatch> matches;

	//结果为一个Mat矩阵，它的行数与特征点向量中元素个数是一致的。每行都是一个N维描述子的向量
	surf->detectAndCompute(a, Mat(), key1, c);      //检测关键点和匹配描述子
	surf->detectAndCompute(b, Mat(), key2, d);

	matcher.match(c, d, matches);         // 匹配，得到匹配向量

	sort(matches.begin(), matches.end());  // 匹配点排序
	vector< DMatch > good_matches;            // 匹配两幅图像的描述子
	int ptsPairs = min(50, (int)(matches.size() * 0.15));
	cout << ptsPairs << endl;
	for (int i = 0; i < ptsPairs; i++)       // 将匹配较好的特征点存入good_matches中
	{
		good_matches.push_back(matches[i]);
	}
	Mat outimg;
	drawMatches(                               // 绘制匹配点
		a,                                    // 原图像1
		key1,                                 // 原图像1的特征点
		b,                                    // 原图像2
		key2,                                 // 原图像2的特征点
		good_matches,                         // 原图像1的特征点匹配原图像2的特征点[matches[i]]
		outimg,                               // 输出图像具体由flags决定
		Scalar::all(-1),                    // 匹配的颜色（特征点和连线),若matchColor==Scalar::all(-1)，颜色随机
		Scalar::all(-1),                    // 单个点的颜色，即未配对的特征点，若matchColor==Scalar::all(-1)，颜色随机
		vector<char>(),                       // Mask决定哪些点将被画出，若为空，则画出所有匹配点
		DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS);  //Fdefined by DrawMatchesFlags


	vector<Point2f> obj;
	vector<Point2f> scene;

	for (size_t i = 0; i < good_matches.size(); i++)
	{
		////good_matches[i].queryIdx保存着第一张图片匹配点的序号，keypoints_1[good_matches[i].queryIdx].pt.x 为该序号对应的点的x坐标
		obj.push_back(key1[good_matches[i].queryIdx].pt);
		scene.push_back(key2[good_matches[i].trainIdx].pt);
	}

	vector<Point2f> scene_corners(4);
	vector<Point2f> obj_corners(4);
	obj_corners[0] = Point(0, 0);
	obj_corners[1] = Point(a.cols, 0);
	obj_corners[2] = Point(a.cols, a.rows);
	obj_corners[3] = Point(0, a.rows);

	Mat H = findHomography(              // 在两个平面之间寻找单映射变换矩阵
		obj,                            // 在原平面上点的坐标
		scene,                          // 在目标平面上点的坐标
		4);                        // 用于计算单映射矩阵的方法

	perspectiveTransform(                // 向量组的透视变换
		obj_corners,                    // 输入两通道或三通道的浮点数组，每一个元素是一个2D/3D 的矢量转换
		scene_corners,                  // 输出和src同样的size和type
		H);                             // 3x3 或者4x4浮点转换矩阵


										// 绘制
	line(outimg, scene_corners[0] + Point2f((float)a.cols, 0), scene_corners[1] + Point2f((float)a.cols, 0), Scalar(0, 255, 0), 2, LINE_AA);
	line(outimg, scene_corners[1] + Point2f((float)a.cols, 0), scene_corners[2] + Point2f((float)a.cols, 0), Scalar(0, 255, 0), 2, LINE_AA);
	line(outimg, scene_corners[2] + Point2f((float)a.cols, 0), scene_corners[3] + Point2f((float)a.cols, 0), Scalar(0, 255, 0), 2, LINE_AA);
	line(outimg, scene_corners[3] + Point2f((float)a.cols, 0), scene_corners[0] + Point2f((float)a.cols, 0), Scalar(0, 255, 0), 2, LINE_AA);

	namedWindow("匹配图", 0);
	imshow("匹配图", outimg);
	waitKey(-1);
	return 0;
}

void on_MouseHandle(int event, int x, int y, int falgs, void* param)
{
	Mat& image = *(cv::Mat*)param;
	switch (event)
	{
		//鼠标移动消息
	case EVENT_MOUSEMOVE:
	{

		if (g_bDrawingBox) //标识符为真，则记录下长和宽到Rect型变量中
		{
			g_rectangle.width = x - g_rectangle.x;
			g_rectangle.height = y - g_rectangle.y;
		}
	}
	break;
	//左键按下信号
	case EVENT_LBUTTONDOWN:
	{
		cout << " EVENT_LBUTTONDOWN" << endl;
		g_bDrawingBox = true;
		g_rectangle = Rect(x, y, 0, 0);//记录起点
	}
	break;
	//左键抬起信号
	case EVENT_LBUTTONUP:
	{
		cout << " EVENT_LBUTTONUP" << endl;
		g_bDrawingBox = false;
		//对宽高小于0的处理
		if (g_rectangle.width < 0)
		{
			g_rectangle.x += g_rectangle.width;
			g_rectangle.width *= -1;
		}
		if (g_rectangle.height < 0)
		{
			g_rectangle.y += g_rectangle.height;
			g_rectangle.height *= -1;

		}

		//调用绘制函数
		DrawRectangle(image, g_rectangle);
	}
	break;
	}
}

void DrawRectangle(cv::Mat& img, cv::Rect box)
{
	rectangle(img, box.tl(), box.br(), Scalar(0, 0, 225));


}


