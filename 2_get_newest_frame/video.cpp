/*
 * 读取视频和摄像头
 */
 
#include <iostream>
#include <string>
#include <sstream>
#include <unistd.h>
using namespace std;
 
//OpenCV 头文件
#include "opencv2/core.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"
using namespace cv;

//OpenCV 命令行解析器函数
const char * keys = 
{
	"{help h usage ? || print this message}"
	"{@video || Video file, if not defined try to use webcamera}"
};

int main( int argc, char *argv[] )
{
	CommandLineParser parser( argc, argv, keys );
	parser.about( "Chapter 2. v1.0.0" );
	
	//如果需要，显示帮助文档
	if( parser.has("help")){
		parser.printMessage();
		return 0;
	}
	String videoFile = parser.get<String>(0);

	//分析params 的变量，检查params是否正确
	if( !parser.check() ){
		parser.printErrors();
		return 0;
	}
	
	VideoCapture cap;	//打开默认相机
	if( videoFile != "" )
		cap.open( videoFile );
	else
		cap.open(0);
	//检查是否被打开
	if( !cap.isOpened() ){
		fprintf( stderr, "Can't open video \n" );
		return -1;
	}

	//sleep(1);
	//cap.set(CAP_PROP_FRAME_WIDTH,320);
	//cap.set(CAP_PROP_FRAME_HEIGHT,240);
//	namedWindow( "Video", 1 );
	cout << "width = " << cap.get(CAP_PROP_FRAME_WIDTH) << endl;
	cout << "height = " << cap.get(CAP_PROP_FRAME_HEIGHT) << endl;
	cout <<	"frames = " << cap.get(CV_CAP_PROP_FPS) << endl;
	cout << "format = " << cap.get(CAP_PROP_FORMAT) << endl;
	cout << "brightness = " << cap.get(CAP_PROP_BRIGHTNESS ) << endl; 
	cout << "contrast = " << cap.get(CAP_PROP_CONTRAST) << endl;
	cout << "saturation = " << cap.get(CAP_PROP_SATURATION ) << endl;
	cout << "hue = " << cap.get(CAP_PROP_HUE) << endl;
	cout << "gain = " << cap.get(CAP_PROP_GAIN) << endl;
	cout << "exposure = " << cap.get(CAP_PROP_EXPOSURE) << endl;
	

	for( int i = 1; i < 100; i++ ){
		Mat frame;
//		cap.set( CAP_PROP_BUFFERSIZE, 0);
		cap.read(frame);
		if( !frame.data ){
			i--;
			continue;
		}
		if( i % 30 == 0 ){
			string name = "video";
			string num = to_string(i);
			name.append( num );
			name.append(".png");
			cvtColor(frame, frame, COLOR_BGR2GRAY);
			imwrite( name, frame );
		}
//		waitKey(1000/30);
//		if( waitKey(1000/10) == 27 )
//			break;
	}
	
	//释放cap
	cap.release();
//	waitKey(0);	
	return 0;
}
