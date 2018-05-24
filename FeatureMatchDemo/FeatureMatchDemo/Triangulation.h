#pragma once

#include <opencv.hpp>
#include <vector>

#define EPSILON 0.0001

struct CloudPoint {
	cv::Point3d pt;
	std::vector<int> imgpt_for_img;
	double reprojection_error;
};

cv::Mat_<double> LinearLSTriangulation(cv::Point3d u, //homogenous image point (u,v,1)
	cv::Matx34d p, // camera 1 matrix
	cv::Point3d u1, // homogenous image point in 2nd camera
	cv::Matx34d p1 // camera 2 matrix
	);

cv::Mat_<double> IterativeLiearLSTriangulation(cv::Point3d u,
	cv::Matx34d p,
	cv::Point3d u1,
	cv::Matx34d p1
	);


double TriangulatePoints(const std::vector<cv::KeyPoint>& pt_set1,
	const std::vector<cv::KeyPoint>& pt_set2,
	const cv::Mat& k,
	const cv::Mat& kinv,
	const cv::Mat& distcoeff,
	const cv::Matx34d& p,
	const cv::Matx34d& p1,
	std::vector<CloudPoint>& pointcloud,
	std::vector<cv::KeyPoint>& correspImg1Pt
	);
