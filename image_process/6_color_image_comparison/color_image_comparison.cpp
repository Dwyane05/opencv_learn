#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include <iostream>
#include <stdio.h>

using namespace cv;
using namespace std;

void histogram2Dcalculation( const Mat &src, Mat &histo2D )
{
	Mat hsv;

	cvtColor( src, hsv, CV_BGR2HSV);

	//quantized chromaticity 30~250
	//saturation 32-255
	int hbins = 255, sbins = 255;
	int histSize[] = { hbins, sbins };
	//chroma change 0-179
	float hranges[] = {0,180};
	//saturation change from 0 to 255
	float sranges[] = { 0, 256 };
	const float *ranges[] = {hranges, sranges};
	MatND hist, hist2;
	//equalize channel 0 1
	int channels[] = { 0, 1 };

	calcHist( &hsv, 1, channels, Mat(), hist, 1, histSize, ranges,
			true, false );
	double maxVal = 0;
	minMaxLoc( hist, 0, &maxVal, 0, 0);

	int scale = 1;
	Mat histImage = Mat::zeros( sbins*scale, hbins*scale, CV_8UC3 );

	for( int h = 0; h < hbins; h++ ){
		for( int s = 0; s < sbins; s++ ){
			float binVal = hist.at<float>(h,s);
			int intensity = cvRound( binVal*255/maxVal);
			rectangle( histImage, Point(h*scale, s*scale),
					Point( (h+1)*scale -1, (s+1)*scale - 1 ),
					Scalar::all(intensity),
					CV_FILLED );
		}
	}
	histo2D = histImage;
}

void histogramRGcalculation( const Mat &src, Mat &histoRG )
{
	//red use 50 bins, green use 60 bins
	int r_bins = 50;
	int g_bins = 60;
	int histSize[] = { r_bins, g_bins };

	//red change from 0-255, green change from 0-255
	float r_ranges[] = { 0, 255 };
	float g_ranges[] = { 0, 255 };
	const float *ranges[] = { r_ranges, g_ranges };

	//use channel 0 and 1
	int channels[] = {0,1};

	MatND hist_base;
	//calculate histogram for hsv
	calcHist( &src, 1, channels, Mat(), hist_base, 2, histSize,
			ranges, true, false );
	normalize( hist_base, hist_base, 0, 1, NORM_MINMAX, -1, Mat() );

	histoRG = hist_base;
}

int main( int argc, char *argv[] )
{
	Mat src, imageq;
	Mat histImg, histImgeq;
	Mat histHSorg, histHSeq;

	//read src image
	src = imread( "fruits.jpg" );
	if( src.empty() ){
		fprintf( stderr, "cann't load image\n" );
		exit(-1);
	}

	vector<Mat> bgr_planes;
	split( src, bgr_planes );

	//show result
	namedWindow( "Source Image", 0 );
	imshow( "Source Image", src);

	//calculate org equalize
	histogram2Dcalculation( src, histImg );

	//show every channel
	imshow( "H_S Histogram", histImg );

	//equalize image
	equalizeHist( bgr_planes[0], bgr_planes[0] );
	equalizeHist( bgr_planes[1], bgr_planes[1] );
	equalizeHist( bgr_planes[2], bgr_planes[2] );

	//merge equalized channel
	merge( bgr_planes, imageq);

	namedWindow( "Equalized image", 0 );
	imshow( "Equalized image", imageq );

	//calculate H S
	histogram2Dcalculation( imageq, histImgeq );

	imshow( "H_S Histogram Equalized", histImgeq);

	histogramRGcalculation( src, histHSorg );
	histogramRGcalculation( imageq, histHSeq );

	//compare
	for( int i = 0; i < 4; i++ ){
		int compare_method = i;
		double orig_orig = compareHist( histHSorg, histHSorg, compare_method );
		double orig_equ = compareHist( histHSorg, histHSeq, compare_method );
		printf( " Method [%d] Original-Original, Original-Equalized : %f,"
				"%f \n", i, orig_orig, orig_equ );
	}
	printf( "Done\n" );
	waitKey(0);
	return 0;
}



