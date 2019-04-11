#include <opencv2/opencv.hpp>

#define throDark 20
#define throMid 80
#define throBright 170

static void color_transfer_with_spilt( cv::Mat &input, std::vector<cv::Mat> &chls )
{
    cv::cvtColor( input, input, cv::COLOR_BGR2YCrCb);
    cv::split( input, chls );
}

static void color_retransfer_with_merge( cv::Mat &output, std::vector<cv::Mat> &chls )
{
    cv::merge( chls, output );
    cv::cvtColor( output, output, cv::COLOR_YCrCb2BGR );
}

static void to_be_normal(cv::Mat &input, cv::Mat &output )
{
    cv::normalize(input, input, 255, 0, cv::NORM_MINMAX);//归一化处理
    cv::convertScaleAbs(input, output);
}

//γ变换
cv::Mat gammaTran(const cv::Mat src, double gamma, double comp)
{
    cv::Mat dst(src);
    int M = 0;
    int N = 0;
    if (src.empty()){
        std::cout << "Src pic is empty" << std::endl;
        return src;
    }
    M = src.rows;
    N = src.cols*src.channels();
    for (int i = 0; i < M; i++){
        const float *p1 = src.ptr<const float>(i);
        float *p2 = dst.ptr<float>(i);
        for (int j = 0; j < N; j++){
            p2[j] = pow(p1[j], gamma) * comp;
        }
    }
    return dst;
}

cv::Mat mixed_space_enhancement( cv::Mat &src_img )
{
    cv::Mat ycrcb = src_img.clone();
    std::vector<cv::Mat> channels;
    color_transfer_with_spilt(ycrcb, channels);

    cv::Mat input = channels[0].clone();
    input.convertTo(input, CV_32F, 1.0 / 255, 0);//把图片转化为float类型，这样子可以直接进行加减而不会溢出

    //Laplacian变换
    cv::Mat kern = (cv::Mat_<float>(3, 3) << 1, 1, 1,       //滤波器
        1, -8, 1,
        1, 1, 1);
    cv::Mat laplas;
    cv::Mat output, R0;
    cv::filter2D(input, laplas, input.depth(), kern);//使用滤波器kern对input进行相关操作，结果存储在laplas中
    output = input - laplas;//如果中间的值是正的则是加号，负值则是减号
    output.convertTo(R0, CV_32F, 1.0/255.0, 0);
    //Sobel梯度
    cv::Mat kern2 = (cv::Mat_<float>(3, 3) << -1, -2, -1,
        0, 0, 0,
        1, 2, 1);
    cv::Mat kern3 = (cv::Mat_<float>(3, 3) << -1, 0, 1,
        -2, 0, 2,
        -1, 0, 1);
    cv::Mat gx, gy;
    cv::filter2D(input, gx, input.depth(), kern2);
    cv::filter2D(input, gy, input.depth(), kern3);
    cv::Mat Soutput = abs(gx) + abs(gy);

    cv::Mat R2;
    cv::blur(Soutput, R2, cv::Size(5, 5));
    cv::multiply(R0, R2, R2);               //相乘
    R2 = input + R2;                     //锐化增强

    cv::Mat r5, R5;
    r5 = gammaTran(R2, 0.4, 1);             //γ变换
    to_be_normal(r5, R5);
    channels[0].release();
    R5.copyTo(channels[0]);
    color_retransfer_with_merge( ycrcb, channels );
    return ycrcb;
}

cv::Mat opencv_detail_enhance( cv::Mat &src_img )
{
    cv::Mat src = src_img.clone();
    cv::Mat enhance_img;
    cv::detailEnhance(src,enhance_img,10,0.15);
    return enhance_img;
}

cv::Mat img_equalize( cv::Mat &src )
{
    cv::Mat ycrcb = src.clone();
    cv::Mat result;
    std::vector<cv::Mat> channels;

    color_transfer_with_spilt(ycrcb, channels);
    // Equalize the Y channel only
    equalizeHist( channels[0], channels[0] );
    color_retransfer_with_merge(result, channels);
    return result;
}


static double getUdark(double num){
    double tmpUdark;
    if(num <= throDark){
        tmpUdark = 1;
    }else if((num > throDark) && (num <= throMid)){
        tmpUdark  = ((double)(throMid - num)) / ((double)(throMid - throDark));
    }else{
        tmpUdark = 0;
    }
    return tmpUdark;
}

static double getUmid(double num){
    double tmpUmid;
    if((num > throDark) && (num < throMid)){
        tmpUmid  = (num - throDark) / (throMid - throDark);
    }else if((num >= throMid) && (num < throBright)){
        tmpUmid  = (throBright - num) / (throBright - throMid);
    }
    else{
        tmpUmid = 0;
    }
    return tmpUmid;
}

static double getUbright(double num){
    double tmpUbright;
    if(num <= throMid){
        tmpUbright = 0;
    }else if((num > throMid) && (num <= throBright)){
        tmpUbright  = (num - throMid) / (throBright - throMid);
    }else{
        tmpUbright = 1;
    }
    return tmpUbright;
}

static cv::Mat fuzzy_deal( cv::Mat &srcImage )
{
    cv::Mat resultImage = cv::Mat::zeros(srcImage.size(), srcImage.type());
    uchar val = 0;
    double Udark, Umid, Ubright;
    for (int i = 0; i < srcImage.rows; i++){
        for (int j = 0; j < srcImage.cols; j++){
            val = srcImage.at<uchar>(i, j);
            Udark   = getUdark(val);
            Umid    = getUmid(val);
            Ubright = getUbright(val);
            val = (0 * Udark + 127 * Umid + 255 * Ubright) / (Udark + Umid + Ubright);
            resultImage.at<uchar>(i, j) = cv::saturate_cast<uchar>(val);
        }
    }
    return resultImage;
}

cv::Mat imag_fuzzy_deal(cv::Mat &src )
{
    cv::Mat ycrcb = src.clone();
    std::vector<cv::Mat> channels;

    color_transfer_with_spilt(ycrcb, channels);
    cv::Mat temp = channels[0].clone();
    cv::Mat dst_img = fuzzy_deal(temp);
    channels[0].release();
    dst_img.copyTo(channels[0]);
    color_retransfer_with_merge(ycrcb, channels);
    return ycrcb;
}

cv::Mat img_laplacian( cv::Mat &src )
{
    cv::Mat src_img = src.clone();
    cv::Mat dst_img;

    cv::Mat kernel = (cv::Mat_<float>(3, 3) << 0, -1, 0, -1, 4, -1, 0, -1, 0);
    filter2D(src_img, dst_img, CV_8UC3, kernel);
    cv::add(src_img,dst_img, dst_img );
    return dst_img;
}

using namespace std;
using namespace cv;
unsigned char lut[256];
bool lut_init = false;

cv::Mat  img_gama( cv::Mat &src )
{
    cv::Mat ycrcb = src.clone();
    std::vector<cv::Mat> channels;

    color_transfer_with_spilt(ycrcb, channels);
    cv::Scalar mean_value = cv::mean(channels[0]);
    float gama = 1;
    if( mean_value.val[0] > 160 )gama = 2.5;
    if( mean_value.val[0] < 85 ) gama = 0.4;
    if( mean_value.val[0] >= 85 && mean_value.val[0] <= 160 )
        return src;

    if( !lut_init ){
        for( int i = 0; i < 256; i++ ){
            lut[i] = saturate_cast<uchar>(pow((float)(i/255.0), gama) * 255.0f);
        }
        lut_init = true;
    }

    cv::Mat dst = channels[0].clone();
    MatIterator_<u_char> it, end;
    for( it = dst.begin<u_char>(), end = dst.end<u_char>(); it != end; it++ ){
        (*it) = lut[((*it))];
    }
    channels[0].release();
    dst.copyTo(channels[0]);
    color_retransfer_with_merge(ycrcb, channels);

    return ycrcb;
}

cv::Mat clahe_deal( cv::Mat &src)
{
    cv::Mat ycrcb = src.clone();
    std::vector<cv::Mat> channels;

    color_transfer_with_spilt(ycrcb, channels);

    cv::Mat clahe_img;
    cv::Ptr<cv::CLAHE> clahe = cv::createCLAHE();
    // 直方图的柱子高度大于计算后的ClipLimit的部分被裁剪掉，然后将其平均分配给整张直方图
    // 从而提升整个图像
    clahe->setClipLimit(4.);	// (int)(4.*(8*8)/256)
    clahe->setTilesGridSize(Size(8, 8)); // 将图像分为8*8块
    clahe->apply(channels[0], clahe_img);
    channels[0].release();
    clahe_img.copyTo(channels[0]);
    color_retransfer_with_merge(ycrcb, channels);
    return ycrcb;

}
cv::Mat highboost_filter(Mat &src)
{
    Mat gaussi;
    GaussianBlur(src, gaussi, Size(5, 5), 3, 3);

    Mat mask;
    cv::subtract(src, gaussi, mask);

    Mat imageEnhance;
    cv::add(src,5*mask, imageEnhance );
    return imageEnhance;

}


//--------------------
//函数功能：获取图像的局部均值与局部标准差的图
//函数名称：getVarianceMean
//函数参数：Mat &scr：输入图像，为单通道；
//函数参数：Mat &meansDst：计算得到的均值的图，均值的值与输入图像中的点一一对应，为单通道；
//函数参数：Mat &varianceDst：计算得到的标准差的图，标准差的值与输入图像中的点一一对应，为单通道；
//函数参数：int winSize：局部均值的窗口大小，应为单数；
//返回类型：bool
//--------------------

bool getVarianceMean(Mat &scr, Mat &meansDst, Mat &varianceDst, int winSize)
{
    if (!scr.data)  //判断图像是否被正确读取；
    {
        cerr << "获取方差与均值的函数读入图片有误";
        return false;
    }

    if (winSize % 2 == 0)
    {
        cerr << "计算局部均值与标准差的窗口大小应该为单数";
        return false;
    }

    Mat copyBorder_yChannels;                        //扩充图像边界；
    int copyBorderSize = (winSize - 1) / 2;
    copyMakeBorder(scr, copyBorder_yChannels, copyBorderSize, copyBorderSize, copyBorderSize, copyBorderSize, BORDER_REFLECT);

    for (int i = (winSize - 1) / 2; i < copyBorder_yChannels.rows - (winSize - 1) / 2; i++)
    {
        for (int j = (winSize - 1) / 2; j < copyBorder_yChannels.cols - (winSize - 1) / 2; j++)
        {

            Mat temp = copyBorder_yChannels(Rect(j - (winSize - 1) / 2, i - (winSize - 1) / 2, winSize, winSize));   //截取扩展后的图像中的一个方块；

            Scalar  mean;
            Scalar  dev;
            meanStdDev(temp, mean, dev);

            varianceDst.at<float>(i - (winSize - 1) / 2, j - (winSize - 1) / 2) = dev.val[0];     ///一一对应赋值；
            meansDst.at<float>(i - (winSize - 1) / 2, j - (winSize - 1) / 2) = mean.val[0];
        }
    }


    return true;
}

//--------------------------
//函数功能：获取图像的局部均值与局部标准差的图
//函数名称：adaptContrastEnhancement
//函数参数：Mat &scr：输入图像，为三通道RGB图像；
//函数参数：Mat &dst：增强后的输出图像，为三通道RGB图像；
//函数参数：int winSize：局部均值的窗口大小，应为单数；
//函数参数：int maxCg：增强幅度的上限；
//返回类型：bool
//--------------------

bool adaptContrastEnhancement(Mat &scr, Mat &dst, int winSize,int maxCg)
{
    Mat ycc = scr.clone();                        //转换空间到YCrCb；
    cvtColor(ycc, ycc, COLOR_BGR2YCrCb);

    vector<Mat> channels(3);        //分离通道；
    split(ycc, channels);


    Mat localMeansMatrix(scr.rows , scr.cols , CV_32FC1);
    Mat localVarianceMatrix(scr.rows , scr.cols , CV_32FC1);

    if (!getVarianceMean(channels[0], localMeansMatrix, localVarianceMatrix, winSize))   //对Y通道进行增强；
    {
        cerr << "计算图像均值与标准差过程中发生错误";
        return false;
    }

    Mat temp = channels[0].clone();

    Scalar  mean;
    Scalar  dev;
    meanStdDev(temp, mean, dev);

    float meansGlobal = mean.val[0];
    Mat enhanceMatrix(scr.rows, scr.cols, CV_8UC1);

    for (int i = 0; i < scr.rows; i++)            //遍历，对每个点进行自适应调节
    {
        for (int j = 0; j < scr.cols; j++)
        {
            if (localVarianceMatrix.at<float>(i, j) >= 0.01)
            {
                float cg = 0.2*meansGlobal / localVarianceMatrix.at<float>(i, j);
                float cgs = cg > maxCg ? maxCg : cg;
                cgs = cgs < 1 ? 1 : cgs;

                int e = localMeansMatrix.at<float>(i, j) + cgs* (temp.at<uchar>(i, j) - localMeansMatrix.at<float>(i, j));
                if (e > 255){ e = 255; }
                else if (e < 0){ e = 0; }
                enhanceMatrix.at<uchar>(i, j) = e;
            }
            else
            {
                enhanceMatrix.at<uchar>(i, j) = temp.at<uchar>(i, j);
            }
        }

    }

    channels[0] = enhanceMatrix;    //合并通道，转换颜色空间回到RGB
    merge(channels, ycc);

    cvtColor(ycc, dst, COLOR_YCrCb2BGR);
    return true;
}

//-------------------------------
//函数名：adaptHDR；参照作者脚本实现
//函数功能：全局自适应光照度提升
//参数：Mat &scr，输入图像
//参数：Mat &dst，输出图像
//------------------------------
bool adaptHDR(Mat &scr, Mat &dst)
{

    if (!scr.data)  //判断图像是否被正确读取；
    {
        cerr << "输入图像有误"<<endl;
        return false;
    }

    int row = scr.rows;
    int col = scr.cols;


    Mat ycc;                        //转换空间到YUV；
    cvtColor(scr, ycc, COLOR_RGB2YUV);

    vector<Mat> channels(3);        //分离通道，取channels[0]；
    split(ycc, channels);


    Mat Luminance(row, col, CV_32FC1);
    for (int i = 0; i < row; i++)
    {
        for (int j = 0; j < col; j++)
        {
            Luminance.at<float>(i, j) =(float)channels[0].at<uchar>(i, j)/ 255;
        }
    }


    double log_Ave = 0;
    double sum = 0;
    for (int i = 0; i < row; i++)                 //求对数均值
    {
        for (int j = 0; j < col; j++)
        {
            sum += log(0.001 + Luminance.at<float>(i, j));
        }
    }
    log_Ave = exp(sum / (row*col));

    double MaxValue, MinValue;      //获取亮度最大值为MaxValue；
    minMaxLoc(Luminance, &MinValue, &MaxValue);

    Mat hdr_L (row,col,CV_32FC1);
    for (int i = 0; i < row; i++)
    {
        for (int j = 0; j < col; j++)
        {
            hdr_L.at<float>(i, j) = log(1 + Luminance.at<float>(i, j) / log_Ave) / log(1 + MaxValue / log_Ave);


            if (channels[0].at<uchar>(i, j) == 0)   //对应作者代码中的gain = Lg ./ Lw;gain(find(Lw == 0)) = 0;
            {
                hdr_L.at<float>(i, j) = 0;
            }
            else
            {
                hdr_L.at<float>(i, j) /= Luminance.at<float>(i, j);
            }

        }
    }

    vector<Mat> rgb_channels;        //分别对RGB三个通道进行提升
    split(scr, rgb_channels);
    for (int i = 0; i < row; i++)
    {
        for (int j = 0; j < col; j++)
        {
            int r = rgb_channels[0].at<uchar>(i, j) *hdr_L.at<float>(i, j); if ( r> 255){r = 255; }
            rgb_channels[0].at<uchar>(i, j) = r;

            int g = rgb_channels[1].at<uchar>(i, j) *hdr_L.at<float>(i, j); if (g> 255){ g = 255; }
            rgb_channels[1].at<uchar>(i, j) = g;

            int b = rgb_channels[2].at<uchar>(i, j) *hdr_L.at<float>(i, j); if (b> 255){ b = 255; }
            rgb_channels[2].at<uchar>(i, j) = b;
        }
    }
    merge(rgb_channels, dst);

    return true;
}


int main(int argc , char* argv[])
{
    if(argc < 2){
        std::cerr << "Please input picture!\n" << std::endl;
        return 0;
    }
    cv::Mat image;

    image = cv::imread(argv[1], cv::IMREAD_COLOR);
    imshow( "src", image);

    double Time = (double)cvGetTickCount();
    Mat temp;
    for( int k = 0; k < 100; k++){
        temp = mixed_space_enhancement(image);
//        imshow("mixed_space_enhancement", temp);

        temp = opencv_detail_enhance(image);
//        imshow("opencv_detail_enhance", temp);

        temp = img_equalize(image);
//        imshow("img_equalize", temp);

        temp = imag_fuzzy_deal(image);
//        imshow("imag_fuzzy_deal", temp);

        temp = img_laplacian(image);
//        imshow("img_laplacian", temp);

        temp = img_gama(image);
//        imshow("img_gama", temp);

        temp = clahe_deal(image);
//        imshow("clahe_deal", temp);
        const int WINSIZE = 15;      //WINSIZE表示求均值与方差的窗口大小，应该是单数
        const int MAXCG = 10;        //设定最大的增强比例

        adaptContrastEnhancement(image, temp, WINSIZE,MAXCG);

        adaptHDR(image, temp);

    }


    Time = (double)cvGetTickCount() - Time ;

    printf( "run time = %gms\n", Time /(cvGetTickFrequency()*1000) );//毫秒

    printf( "run time = %gs\n", Time /(cvGetTickFrequency()*1000000) );//秒



    imshow("clahe_deal", temp);
    waitKey();
    return 0;
}
