
#include <iostream>
#include <sstream>
#include <cmath>
#include <string>
#include <stdio.h>
using namespace std;

#include "opencv2/core.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/highgui.hpp"
using namespace cv;

Mat img;

// OpenCV command line parser functions
// Keys accecpted by command line parser
const char* keys =
{
	"{help h usage ? | | print this message}"
    	"{@image | | Image to process}"
};


void equalizeCallback( int state, void *userData )
{
	Mat result;
	Mat ycrcb;

	cvtColor( img, ycrcb, COLOR_BGR2YCrCb);
	
	//channel devide
	vector<Mat> channels;
	split( ycrcb, channels );
	
	//only equalize y
	equalizeHist( channels[0], channels[0] );

	//merge channels
	merge( channels, ycrcb );
	
	cvtColor( ycrcb, result, COLOR_YCrCb2BGR );
 	imshow( "Equalize", result );
}

void showHistoCallback( int state, void *userData )
{
	//split image to 3 channels
	vector<Mat> bgr;
	split( img, bgr );

	//create 256 subinterval 
	//the number of possibles values
	int numbins = 256;

	//set the range for BGR(0-256)
	float range[] = { 0, 256}; 
	const float* histRange = { range };

	Mat b_hist, g_hist, r_hist;
	
	calcHist( &bgr[0], 1, 0, Mat(), b_hist, 1, &numbins, &histRange );
	calcHist( &bgr[1], 1, 0, Mat(), g_hist, 1, &numbins, &histRange );
	calcHist( &bgr[2], 1, 0, Mat(), r_hist, 1, &numbins, &histRange );
	
	//draw histogram
	//draw lines for each channels
	int width = 512;
	int height = 300;

	// Create image with gray base
	Mat histImage( height, width, CV_8UC3, Scalar(20, 20, 20) );
	
	// Normalize the histograms to height of image
	normalize(b_hist, b_hist, 0, height, NORM_MINMAX );
	normalize(g_hist, g_hist, 0, height, NORM_MINMAX );
	normalize(r_hist, r_hist, 0, height, NORM_MINMAX );

	int binStep= cvRound((float)width/(float)numbins);
	for( int i=1; i< numbins; i++)
    	{
        	line( histImage,
 			Point( binStep*(i-1), height-cvRound(b_hist.at<float>(i-1) ) ),
			Point( binStep*(i), height-cvRound(b_hist.at<float>(i) ) ),
                  	Scalar(255,0,0)
		);
		line( histImage,
 			Point( binStep*(i-1), height-cvRound(g_hist.at<float>(i-1) ) ),
 			Point( binStep*(i), height-cvRound(g_hist.at<float>(i) ) ),
 			Scalar(0,255,0)	
		);
 		line( histImage,
 			Point( binStep*(i-1), height-cvRound(r_hist.at<float>(i-1) ) ),
 			Point( binStep*(i), height-cvRound(r_hist.at<float>(i) ) ),
 			Scalar(0,0,255) 
		);
    	}
 
     	imshow("Histogram", histImage);
}

void lemoCallback( int state, void *userData )
{
	Mat result;
	//create lut table	
	const double E = std::exp(1.0);
	Mat lut( 1, 256, CV_8UC1 );
	float s = 0.1;		//change s value can get different result
	for( int i = 0; i < 256; i++ ){
		float x = (float)i/256.0;
		lut.at<uchar>(i) = cvRound( 256 * (1/(1 + pow(E, -((x-0.5)/s)) )) );
	}
	
	// Split the image channels and apply curve transform only to red channel
	vector<Mat> bgr;
	split( img, bgr );
	LUT(bgr[2], lut, bgr[2]);
	//merge result
	merge(bgr, result);
	//imshow( "result", result );
	
	//create image for halo dark
	Mat halo( img.rows, img.cols, CV_32FC3, Scalar(0.3,0.3,0.3) );
	//imshow( "halo", halo );
	
	// Create circle
	circle(halo, Point(img.cols/2, img.rows/2), img.cols/3, Scalar(1,1,1), -1);
	//imshow( "halo_circle", halo );
	blur(halo, halo, Size(img.cols/3, img.cols/3));
	//imshow( "halo_blur", halo );

	// Convert the result to float to allow multiply by 1 factor
	Mat resultf;
	result.convertTo(resultf, CV_32FC3);
	
	// Multiply our result with halo
	multiply(resultf, halo, resultf);

	// convert to 8 bits
	resultf.convertTo(result, CV_8UC3);

	// show result
	imshow("Lomograpy", result);

	// Release mat memory
	halo.release();
	resultf.release();
	lut.release();
	bgr[0].release();
	bgr[1].release();
	bgr[2].release();
}

void cartoonCallback( int state, void *userData )
{
	/* EDGES */
	//apply median filter ro remove possible noise
	Mat imgMedian;
	medianBlur( img, imgMedian, 7 );
	
	//Detect edges with canny
	Mat imgCanny;
	Canny( imgMedian, imgCanny, 50, 150 );
	
	//Dilate the edges
	Mat kernel = getStructuringElement(MORPH_RECT, Size(2,2));
	dilate(imgCanny, imgCanny, kernel );
	
	// Scale edges values to 1 and invert values
	imgCanny= imgCanny/255;
	imgCanny= 1-imgCanny;
	
	// Use float values to allow multiply between 0 and 1
    	Mat imgCannyf;
    	imgCanny.convertTo(imgCannyf, CV_32FC3);

    	// Blur the edgest to do smooth effect
    	blur(imgCannyf, imgCannyf, Size(5,5));

    	/** COLOR **/
    	// Apply bilateral filter to homogenizes color
    	Mat imgBF;
    	bilateralFilter(img, imgBF, 9, 150.0, 150.0);

    	// truncate colors
   	Mat result= imgBF/25;
    	result= result*25;

    	/** MERGES COLOR + EDGES **/
    	// Create a 3 channles for edges
    	Mat imgCanny3c;
    	Mat cannyChannels[]={ imgCannyf, imgCannyf, imgCannyf};
    	merge(cannyChannels, 3, imgCanny3c);

    	// Convert color result to float 
    	Mat resultf;
    	result.convertTo(resultf, CV_32FC3);

    	// Multiply color and edges matrices
    	multiply(resultf, imgCanny3c, resultf);

    	// convert to 8 bits color
   	resultf.convertTo(result, CV_8UC3);

    	// Show image
    	imshow("Cartoon", result);
}

int main( int argc, const char** argv )
{
	CommandLineParser parser(argc, argv, keys);
    parser.about("Chapter 4. PhotoTool v1.0.0");
    //If requires help show
    if (parser.has("help"))
	{
	    parser.printMessage();
	    return 0;
	}

	String imgFile= parser.get<String>(0);

	// Check if params are correctly parsed in his variables
	if (!parser.check())
	{
	    parser.printErrors();
	    return 0;
	}
    
    // Load image to process
    img= imread(imgFile);
    	if( !img.data ){
		fprintf( stderr, "Can't read image\n" );
		return -1;	
	}
    // Create window
    namedWindow("Input");

    // Create UI buttons
 //   createButton("Show histogram", showHistoCallback, NULL, QT_PUSH_BUTTON, 0);
//    createButton("Equalize histogram", equalizeCallback, NULL, QT_PUSH_BUTTON, 0);
   // createButton("Lomography effect", lomoCallback, NULL, QT_PUSH_BUTTON, 0);
    //createButton("Cartonize effect", cartoonCallback, NULL, QT_PUSH_BUTTON, 0);
	showHistoCallback(0,0);
	equalizeCallback(0,0);
	lemoCallback(0,0);
	cartoonCallback(0,0);
    // Show image
    imshow("Input", img);

    waitKey(0);
	return 0;

}
