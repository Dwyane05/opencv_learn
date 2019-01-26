#include <cstdlib>
#include <cmath>
#include <limits>
#include <iostream>
#include <sstream>
#include <utility>
#include <vector>
#include <math.h>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/opencv.hpp>

using namespace cv;
using namespace std;

/*
产生高斯噪声的函数，引自维基百科
*/

double generateGaussianNoise()
{
    static bool hasSpare = false;
    static double rand1, rand2;

    if (hasSpare)
    {
        hasSpare = false;
        return sqrt(rand1) * sin(rand2);
    }

    hasSpare = true;

    rand1 = rand() / ((double)RAND_MAX);
    if (rand1 < 1e-100) rand1 = 1e-100;
    rand1 = -2 * log(rand1);
    rand2 = (rand() / ((double)RAND_MAX)) * CV_PI*2;
//    cout << "gaussi: " << sqrt(rand1) * cos(rand2) << endl;
    return sqrt(rand1) * cos(rand2);
}

//为图像添加高斯噪声
void addGaussianNoise(Mat &srcImag)
{
//    Mat dstImage = srcImag.clone();
    int channels = srcImag.channels();
    int rowsNumber = srcImag.rows;
    int colsNumber = srcImag.cols*channels;
    //判断图像的连续性
    if (srcImag.isContinuous())
    {
        colsNumber *= rowsNumber;
        rowsNumber = 1;
    }
    for (int i = 0; i < rowsNumber; i++)
    {
        for (int j = 0; j < colsNumber; j++)
        {
            //添加高斯噪声
            int val = srcImag.ptr<uchar>(i)[j] +
                generateGaussianNoise()*32;
            if (val < 0)
                val = 0;
            if (val>255)
                val = 255;
            srcImag.ptr<uchar>(i)[j] = (uchar)val;
        }
    }
//    return srcImag;
}


Mat eliminate_noise(vector<Mat> &img_vec)
{
    if( img_vec.empty() ){
        cerr << "image vector empty" << endl;
        Mat err;
        return err;
    }
    Mat average = Mat::zeros(img_vec.at(0).size(), CV_32FC3);
    int n = (int)img_vec.size();
    for(int i = 0; i < n; i++){
        accumulate(img_vec.at(i), average);//累加到average
    }
    average = average / n; //求出平均图像
    average.convertTo(average,CV_8U);

    return average;
}


int main(int argc, char* argv[])
{
//    string name ="image.jpg";
//    int ImageSize =30;
    int ImageSize = atoi(argv[2]);
    string name = argv[1];
    Mat pic = imread(name, IMREAD_COLOR);

//    根据数量生成图片
    vector<Mat> my_pics;
    string newname;
    for (size_t i = 0; i < (size_t)ImageSize; i++){
        Mat newImage = pic.clone();
        addGaussianNoise(newImage);
        ostringstream ss;
        ss << "noise" << i << ".jpg";
        newname = ss.str();
//        imwrite(newname, newImage);
        my_pics.push_back(newImage);
    }
    cout << "add Noise end" << endl;

    //获得消除噪声的输出
    Mat out = eliminate_noise(my_pics);
    ostringstream ss;
    ss << "output" << ImageSize << ".jpg";
    newname = ss.str();
    Mat dest_img = out.clone();
    imwrite(newname, dest_img);
    cout << "deal end" << endl;
    imshow( "src", pic);
    imshow( "dst", dest_img);
    waitKey(0);

    return 0;
}




