
#include <opencv2/core/core.hpp>  
#include <opencv2/highgui/highgui.hpp>  
#include <stdio.h>  
#include<opencv2/imgproc/imgproc.hpp>


IplImage* src = 0;
IplImage* dst = 0;
void on_mouse(int event, int x, int y, int flags, void* ustc)//event鼠标事件代号，x,y鼠标坐标，flags拖拽和键盘操作的代号
{
	static CvPoint pre_pt( -1,-1 );//初始坐标
	static CvPoint cur_pt( -1,-1 );//实时坐标
	CvFont font;
	cvInitFont(&font, CV_FONT_HERSHEY_SIMPLEX, 0.5, 0.5, 0, 1, CV_AA);//初始化字体
	char temp[16];
	if (event == CV_EVENT_LBUTTONDOWN)//左键按下，读取初始坐标，并在图像该点出划圆
	{

		cvCopy(dst, src);//将原始图片复制到dst中;
		sprintf(temp, "(%d,%d)", x, y);
		pre_pt = cvPoint(x, y);
		cvPutText(src, temp, pre_pt, &font, cvScalar(255, 0, 0, 0));//显示坐标
		cvCircle(src, pre_pt, 3, cvScalar(255, 0, 0, 0), CV_FILLED, CV_AA, 0);//划圆
		cvShowImage("src", src);
		cvCopy(src, dst);
	}
	else if (event == CV_EVENT_MOUSEMOVE && !(flags & CV_EVENT_FLAG_LBUTTON))//若鼠标在滑动，左键没有按下
	{
		cvCopy(dst, src);
		sprintf(temp, "(%d,%d)", x, y);
		cur_pt = cvPoint(x, y);
		cvPutText(src, temp, cur_pt, &font, cvScalar(255, 0, 0, 0));//实时显示鼠标移动的坐标
		cvShowImage("src", src);
	}//event鼠标事件代号，x,y鼠标坐标，flags拖拽和键盘操作的代号
	else if (event == CV_EVENT_MOUSEMOVE && (flags & CV_EVENT_FLAG_LBUTTON))
	{
		cvCopy(dst, src);
		sprintf(temp, "(%d,%d)", x, y);
		cur_pt = cvPoint(x, y);
		cvPutText(src, temp, cur_pt, &font, cvScalar(255, 0, 0, 0));
		cvRectangle(src, pre_pt, cur_pt, cvScalar(0, 255, 0, 0), 1, 8, 0);//根据初始点和结束点将矩形画到图片上
		cvShowImage("src", src);
	}
	else if (event == CV_EVENT_LBUTTONUP)//左键松开，将在图片dst上画矩形
	{
		sprintf(temp, "(%d,%d)", x, y);
		cur_pt = cvPoint(x, y);
		cvPutText(src, temp, cur_pt, &font, cvScalar(255, 0, 0, 0));
		cvCircle(src, cur_pt, 3, cvScalar(255, 0, 0, 0), CV_FILLED, CV_AA, 0);//绘制圆形
		cvRectangle(src, pre_pt, cur_pt, cvScalar(0, 255, 0, 0), 1, 8, 0);//绘制矩形
		cvShowImage("src", src);
		cvCopy(src, dst);
	}
}
int main()
{
	src = cvLoadImage("test.jpg");
	if( !src ){
		fprintf( stderr, "Can't load image\n" );
		return -1;
	}
	dst = cvCloneImage(src);
	cvNamedWindow("src", 1);//定义一个“src”窗口
	cvSetMouseCallback("src", on_mouse, 0);//调用回调函数
	cvShowImage("src", src);
	cvWaitKey(0);
	cvDestroyAllWindows();
	//释放窗口
	cvReleaseImage(&src);
	cvReleaseImage(&dst);
	return 0;
}
