/*
 * geometry.cpp
 *
 *  Created on: May 21, 2018
 *      Author: cui
 */

#include <opencv2/opencv.hpp>
#include <math.h>

using namespace cv;
using namespace std;

int main( int argc, char *argv[] )
{
	Mat src;
	src = imread( argv[1] );
	if( src.empty() ){
		cout << "Couldn't load image" << endl;
		return -1;
	}

	namedWindow( "ORIGINAL", WINDOW_AUTOSIZE );
	imshow("ORIGINAL", src);

	//apply resize
	Mat dst1;
	resize( src, dst1, Size(0,0), 0.5, 0.5 );

//	namedWindow( "SCALED", WINDOW_AUTOSIZE );
//	imshow("SCALED", dst1 );

	//应用平移
	Mat dst2;
	int hori_shift = 200;
	int ver_shift = 150;
	Mat M = (Mat_<double>(2,3) << 1, 0, hori_shift, 0, 1, ver_shift);
	warpAffine( src, dst2, M, src.size() );

//	namedWindow( "TRANSLATED", WINDOW_AUTOSIZE );
//	imshow("TRANSLATED", dst2 );

	//image rotate
	//getRotationMatrix2D( 旋转中心, 角度, 缩放因子 )
	Mat dst3;
	Mat M1 = getRotationMatrix2D( Point2f(src.cols/2, src.rows/2), 45, 1);
	warpAffine( src, dst3, M1, src.size() );
//	namedWindow( "ROTATE", WINDOW_AUTOSIZE );
//	imshow("ROTATE", dst3 );

	//应用倾斜
	Mat dst4;
	double m = 1/tan(M_PI/3);
	Mat M2 = (Mat_<double>(2,3) << 1, m, 0, 0, 1, 0 );
	warpAffine( src, dst4, M2, Size(src.cols+0.5*src.cols, src.rows) );
//	namedWindow( "SKEWED", WINDOW_AUTOSIZE );
//	imshow("SKEWED", dst4 );

	//应用水平、垂直及组合反射
	Mat dstv, dsth, dst5;
	Mat Mh  = (Mat_<double>(2,3) << -1, 0, src.cols, 0, 1, 0);//Horizontal
	Mat Mv  = (Mat_<double>(2,3) << 1, 0, 0, 0, -1, src.rows);//Vertical
	Mat M3  = (Mat_<double>(2,3) << -1, 0, src.cols, 0, -1, src.rows);//H-V
	warpAffine(src,dsth,Mh,src.size());
	warpAffine(src,dstv,Mv,src.size());
	warpAffine(src,dst5,M3,src.size());

//	namedWindow( " H-REFLECTION ", WINDOW_AUTOSIZE );
//	imshow( " H-REFLECTION ", dsth );
//	namedWindow( " V-REFLECTION ", WINDOW_AUTOSIZE );
//	imshow( " V-REFLECTION ", dstv );
//	namedWindow( " REFLECTION ", WINDOW_AUTOSIZE );
//	imshow( " REFLECTION ", dst5 );


	//透视变换
	 Mat dst6;
	Point2f src_verts[4];
	src_verts[2] = Point(195, 140);
	src_verts[3] = Point(410, 120);
	src_verts[1] = Point(220, 750);
	src_verts[0] = Point(400, 750);
	Point2f dst_verts[4];
	dst_verts[2] = Point(160, 100);
	dst_verts[3] = Point(530, 120);
	dst_verts[1] = Point(220, 750);
	dst_verts[0] = Point(400, 750);

	// Obtain and Apply the perspective transformation
	Mat M4 = getPerspectiveTransform(src_verts,dst_verts);
	warpPerspective(src,dst6,M4,src.size());

	// Show the results
	namedWindow( " PERSPECTIVE ", WINDOW_AUTOSIZE );
	imshow( " PERSPECTIVE ", dst6 );
	waitKey(0);
	return 0;


}

