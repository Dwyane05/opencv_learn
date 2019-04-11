#include<opencv2/opencv.hpp>
#include<vector>
#include<iostream>

using namespace std;
using namespace cv;


//--------------------
//函数功能：获取图像的局部均值与局部标准差的图
//函数名称：getVarianceMean
//函数参数：Mat &scr：输入图像，为单通道；
//函数参数：Mat &meansDst：计算得到的均值的图，均值的值与输入图像中的点一一对应，为单通道；
//函数参数：Mat &varianceDst：计算得到的标准差的图，标准差的值与输入图像中的点一一对应，为单通道；
//函数参数：int winSize：局部均值的窗口大小，应为单数；
//返回类型：bool
//--------------------

bool getVarianceMean(Mat &scr, Mat &meansDst, Mat &varianceDst, int winSize)
{
	if (!scr.data)  //判断图像是否被正确读取；
	{
		cerr << "获取方差与均值的函数读入图片有误";
		return false;
	}

	if (winSize % 2 == 0)
	{
		cerr << "计算局部均值与标准差的窗口大小应该为单数";
		return false;
	}

	Mat copyBorder_yChannels;                        //扩充图像边界；
	int copyBorderSize = (winSize - 1) / 2;
	copyMakeBorder(scr, copyBorder_yChannels, copyBorderSize, copyBorderSize, copyBorderSize, copyBorderSize, BORDER_REFLECT);

	for (int i = (winSize - 1) / 2; i < copyBorder_yChannels.rows - (winSize - 1) / 2; i++)
	{
		for (int j = (winSize - 1) / 2; j < copyBorder_yChannels.cols - (winSize - 1) / 2; j++)
		{
			
			Mat temp = copyBorder_yChannels(Rect(j - (winSize - 1) / 2, i - (winSize - 1) / 2, winSize, winSize));   //截取扩展后的图像中的一个方块；

			Scalar  mean;
			Scalar  dev;
			meanStdDev(temp, mean, dev);
			
			varianceDst.at<float>(i - (winSize - 1) / 2, j - (winSize - 1) / 2) = dev.val[0];     ///一一对应赋值；
			meansDst.at<float>(i - (winSize - 1) / 2, j - (winSize - 1) / 2) = mean.val[0];
		}
	}

	
	return true;
}

//--------------------------
//函数功能：获取图像的局部均值与局部标准差的图
//函数名称：adaptContrastEnhancement
//函数参数：Mat &scr：输入图像，为三通道RGB图像；
//函数参数：Mat &dst：增强后的输出图像，为三通道RGB图像；
//函数参数：int winSize：局部均值的窗口大小，应为单数；
//函数参数：int maxCg：增强幅度的上限；
//返回类型：bool
//--------------------

bool adaptContrastEnhancement(Mat &scr, Mat &dst, int winSize,int maxCg)
{
	if (!scr.data)  //判断图像是否被正确读取；
	{
		cerr << "自适应对比度增强函数读入图片有误";
		return false;
	}

	Mat ycc;                        //转换空间到YCrCb；
	cvtColor(scr, ycc, COLOR_RGB2YCrCb);

	vector<Mat> channels(3);        //分离通道；
	split(ycc, channels);

	
	Mat localMeansMatrix(scr.rows , scr.cols , CV_32FC1);
	Mat localVarianceMatrix(scr.rows , scr.cols , CV_32FC1);
	
	if (!getVarianceMean(channels[0], localMeansMatrix, localVarianceMatrix, winSize))   //对Y通道进行增强；
	{
		cerr << "计算图像均值与标准差过程中发生错误";
		return false;
	}

	Mat temp = channels[0].clone();

	Scalar  mean;
	Scalar  dev;
	meanStdDev(temp, mean, dev);

	float meansGlobal = mean.val[0];
	Mat enhanceMatrix(scr.rows, scr.cols, CV_8UC1);

	for (int i = 0; i < scr.rows; i++)            //遍历，对每个点进行自适应调节
	{
		for (int j = 0; j < scr.cols; j++)
		{
			if (localVarianceMatrix.at<float>(i, j) >= 0.01)
			{
				float cg = 0.2*meansGlobal / localVarianceMatrix.at<float>(i, j);
				float cgs = cg > maxCg ? maxCg : cg;
				cgs = cgs < 1 ? 1 : cgs;
				
				int e = localMeansMatrix.at<float>(i, j) + cgs* (temp.at<uchar>(i, j) - localMeansMatrix.at<float>(i, j));
				if (e > 255){ e = 255; }
				else if (e < 0){ e = 0; }
				enhanceMatrix.at<uchar>(i, j) = e;
			}
			else
			{
				enhanceMatrix.at<uchar>(i, j) = temp.at<uchar>(i, j);
			}
		}
			
	}
	
	channels[0] = enhanceMatrix;    //合并通道，转换颜色空间回到RGB
	merge(channels, ycc);

	cvtColor(ycc, dst, COLOR_YCrCb2RGB);
		
}

//----------------
//主函数
//----------------
void main()
{
	Mat scr = imread("temp1.jpg", IMREAD_COLOR);  //以彩色的方式读入图像
	 	
	if (!scr.data)               //判断图像是否被正确读取；
	{
		return;
	}

	const int WINSIZE = 15;      //WINSIZE表示求均值与方差的窗口大小，应该是单数
	const int MAXCG = 10;        //设定最大的增强比例

	Mat dst = scr.clone();

	adaptContrastEnhancement(scr, dst, WINSIZE,MAXCG);
	
	imwrite("01.png", dst);      //写出图像
	
	system("pause");
	return;
}

