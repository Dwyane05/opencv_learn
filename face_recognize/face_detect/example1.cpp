#include "opencv2/objdetect.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"

#include <iostream>

using namespace std;
using namespace cv;

/** Function Headers */
void detectAndDisplay( Mat frame );

/** Global variables */
CascadeClassifier face_cascade;
CascadeClassifier eyes_cascade;
cv::Size original_size;

/** @function main */
int main( int argc, const char** argv )
{
    CommandLineParser parser(argc, argv,
                             "{help h||}"
                             "{face_cascade| cascade.xml|Path to face cascade.}");

    parser.about( "\nThis program demonstrates using the cv::CascadeClassifier class to detect objects (Face + eyes) in a video stream.\n"
                  "You can use Haar or LBP features.\n\n" );
    parser.printMessage();

    String face_cascade_name = parser.get<String>("face_cascade");

    //-- 1. Load the cascades
    if( !face_cascade.load( face_cascade_name ) )
    {
        cout << "--(!)Error loading face cascade\n";
        return -1;
    };

    //获取训练集的原始尺寸，作为分类器的最小尺寸，这样能得到最佳的检测效果（不是必须的）
    original_size = face_cascade.getOriginalWindowSize();

    Mat image = imread("test.jpg", IMREAD_COLOR);
    if( image.empty() ){
        printf("image load error\n");
        return 0;
    }

    detectAndDisplay(image);

    waitKey(0);
    return 0;
}

/** @function detectAndDisplay */
void detectAndDisplay( Mat frame )
{
    Mat frame_gray;
    cvtColor( frame, frame_gray, COLOR_BGR2GRAY );
    equalizeHist( frame_gray, frame_gray );

    //-- Detect faces
    std::vector<Rect> faces;
    face_cascade.detectMultiScale( frame_gray, faces, 1.1, 3, 0 | cv::CASCADE_SCALE_IMAGE, original_size );

    for ( size_t i = 0; i < faces.size(); i++ ){
        cv::rectangle(frame, faces[i], cv::Scalar(0, 0, 255), 2, 8, 0);
    }

    //-- Show what you got
    imshow( "Capture - Face detection", frame );
}


