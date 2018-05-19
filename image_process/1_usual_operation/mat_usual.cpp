
//pixel visit
//first, use at<>
Mat src1 = imread( "lena.jpg", IMREAD_GRAYSCALE );
uchar pixel1 = src.at<uchar>(0,0);
cout << "pixel1: " << pixel1 << endl;

//second
uchar B, G, R;
Mat src2 = imread( "lena.jpg", IMREAD_COLOR ); 
for( int i = 0; i < src2.rows; i++ ){
	Vec3b *pixrow = src2.ptr<Vec3b>(i);
	for( int j = 0; j < src2.cols; j++ ){
		B = pixrow[j][0];
		G = pixrow[j][1];
		R = pixrow[j][2];
	}
}


//set
img.setTo(0);		//signal channel
img.setTo( Scalar(b,g,r) );

//matlab style
Mat m1 = Mat::eye( 100, 100, CV_64F);
Mat m2 = Mat::zeros( 100, 100, CV_8UC1 );
Mat m3 = Mat::ones( 100, 100, CV_8UC1)*255;

//rand init
Mat m1 = Mat(100, 100, CV_8UC1 );
randu( m1, 0, 255 );

//create clone
Mat img1 = img.clone();

//create copy with mask
Mat img1;
img.copy(img1, mask);

//queto not copy
Mat img1 = img( Range(r1,r2), Range(c1,c2) );

//copy submatrix, roi 
Rect roi(r1,c2,width,heigth);
Mat img1 = img(roi).clone();		//data copy

//resize image
resize( imgsrc, imgdst, Size(), 0.5, 0.5 );		//1/2

//flip
flip( imgsrc, imgdst, code );
/* code==0 	vectical
 * code >0 	horizontal
 * code <0	vectical && horizontal
 */

//split channels
Mat channel[3];		//Vecter<Mat> bgr;
split( img, channel);	//split( img, bgr);

//merge
merge(channel, img);

//count non zore
int nz = countNonZero(img);

//min max
double min, max;
Point min_loc, max_loc;
minMaxLoc( img, &min, &max, &min_loc, &max_loc);

//pixel mean
Scalar m, stdd;
meanStdDev( img, m, stdd );
uint mean_pxl = mean.value[0];

//check image empty
if( img.empty() ){
	cout << "Couldn't load image" << endl;
}






