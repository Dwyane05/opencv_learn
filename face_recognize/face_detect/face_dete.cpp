#define _CRT_SECURE_NO_DEPRECATE
#include <iostream>
#include <stdio.h>
#include "opencv2/core.hpp"
#include "opencv2/core/utility.hpp"
#include "opencv2/core/ocl.hpp"
#include "opencv2/imgcodecs.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/features2d.hpp"
#include "opencv2/calib3d.hpp"
#include "opencv2/imgproc.hpp"
#include"opencv2/flann.hpp"
#include"opencv2/xfeatures2d.hpp"
#include"opencv2/ml.hpp"
#include"opencv2/face.hpp"
#include"opencv2/face/facerec.hpp"
#include"opencv2/objdetect.hpp"

using namespace cv;
using namespace std;
using namespace cv::xfeatures2d;
using namespace cv::ml;
using namespace face;

int main()
{
    CascadeClassifier cas("haarcascade_frontalface_default.xml");    //加载人脸分类器
    Ptr<FaceRecognizer> fc = createFisherFaceRecognizer();
    Mat a1 = imread("01.jpg",CV_LOAD_IMAGE_GRAYSCALE);    //加载用于训练的人脸照片
    Mat a2 = imread("02.jpg", CV_LOAD_IMAGE_GRAYSCALE);
    Mat a3 = imread("03.jpg", CV_LOAD_IMAGE_GRAYSCALE);
    Mat a4 = imread("04.jpg", CV_LOAD_IMAGE_GRAYSCALE);
    Mat a5 = imread("05.jpg", CV_LOAD_IMAGE_GRAYSCALE);
    Mat b1 = imread("11.jpg", CV_LOAD_IMAGE_GRAYSCALE);
    Mat b2 = imread("12.jpg", CV_LOAD_IMAGE_GRAYSCALE);
    Mat b3 = imread("13.jpg", CV_LOAD_IMAGE_GRAYSCALE);
    Mat b4 = imread("14.jpg", CV_LOAD_IMAGE_GRAYSCALE);
    Mat b5 = imread("15.jpg", CV_LOAD_IMAGE_GRAYSCALE);
    Mat c1 = imread("21.jpg", CV_LOAD_IMAGE_GRAYSCALE);
    Mat c2 = imread("22.jpg", CV_LOAD_IMAGE_GRAYSCALE);
    Mat c3 = imread("23.jpg", CV_LOAD_IMAGE_GRAYSCALE);
    Mat c4 = imread("24.jpg", CV_LOAD_IMAGE_GRAYSCALE);
    Mat c5 = imread("25.jpg", CV_LOAD_IMAGE_GRAYSCALE);


    vector<Mat> images;    
    vector<int>labels;

    images.push_back(a1);        //照片和标签存入向量里，我了三个人的
    labels.push_back(0);
    images.push_back(a2);
    labels.push_back(0);
    images.push_back(a3);
    labels.push_back(0);
    images.push_back(a4);
    labels.push_back(0);
    images.push_back(a5);
    labels.push_back(0);

    images.push_back(b1);
    labels.push_back(1);
    images.push_back(b2);
    labels.push_back(1);
    images.push_back(b3);
    labels.push_back(1);
    images.push_back(b4);
    labels.push_back(1);
    images.push_back(b5);
    labels.push_back(1);

    images.push_back(c1);
    labels.push_back(2);
    images.push_back(c2);
    labels.push_back(2);
    images.push_back(c3);
    labels.push_back(2);
    images.push_back(c4);
    labels.push_back(2);
    images.push_back(c5);
    labels.push_back(2);

    fc->train(images, labels);    //训练
    VideoCapture cap;
    cap.open(1);
    Mat image;
    vector<Rect> recs;
    Mat test(400, 500, CV_8UC1);
    Mat gray;
    int x = 0, y = 0;

    for (;;)
    {
        cap >> image;
        if (image.empty())
            break;
        cas.detectMultiScale(image, recs,1.2,6,0,Size(50,50));    //先检测人脸
        for (int i = 0; i < recs.size();i++)
        {
            rectangle(image, recs[i], Scalar(0, 0, 255));
            x = recs[i].x + recs[i].width / 2;
            y = recs[i].y + recs[i].height / 2;

            Mat roi = image(recs[i]);       //因为我训练的样本是400*500大小，所以需要把摄像头中的人脸区域改为400*500大小
            cvtColor(roi, gray, CV_BGR2GRAY);
            resize(gray, test, Size(400, 500));
            int result = fc->predict(test);
            switch (result)
            {
            case 0:
                putText(image, "a", Point(recs[i].x, recs[i].y), FONT_HERSHEY_SIMPLEX, 1.5, Scalar(0, 0, 255), 2); break;
            case 1:
                putText(image, "b", Point(recs[i].x, recs[i].y), FONT_HERSHEY_SIMPLEX, 1.5, Scalar(0, 0, 255), 2); break;
            case 2:
                putText(image, "c", Point(recs[i].x, recs[i].y), FONT_HERSHEY_SIMPLEX, 1.5, Scalar(0, 0, 255), 2); break;
            }
        }
        imshow("Sample", image);
        if (waitKey(30) >= 0)
            break;
    }

}
