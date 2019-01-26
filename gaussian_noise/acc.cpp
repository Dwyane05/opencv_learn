#include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/video.hpp>
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/ml/ml.hpp"
#include <cstring>

using namespace std;
using namespace cv;

#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/ml/ml.hpp"

using namespace  std;
using namespace  cv;


int main(int argc, char* argv[])
{
    Mat img[8];
    Mat average;
    bool pause = false;

    VideoCapture video_file(0/*"bike.avi"*/);//"bike.avi"//在这里改相应的文件名,输入也可以为摄像头设备  输入形式为 '0'
    namedWindow("foreground", 0);
    namedWindow("average", 1);
    int i = 0;
    for (int j=0;;j++)
    {
        video_file >> img[0];
        average = Mat::zeros(img[0].size(), CV_32FC3);//存储累加后数据的对象赋值为零,放在循环中会在每次循环中都重新置零
        for (i = 1; i < 8;i++)
        {
            video_file >> img[i];
            waitKey(5);
            imshow("foreground", img[i]);
            accumulate(img[i], average);//累加到average

        }
        average /= i;
        average.convertTo(average, CV_8U);
        imshow("average", average);

        char key = cv::waitKey(50);
        if (key == 45)
        {
            break;
        }
        if (key == ' ')
            pause = !pause;

    }
    return 0;
}
