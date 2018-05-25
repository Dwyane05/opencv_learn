#include "Triangulation.h"


#include "Triangulation.h"
using namespace cv;
using namespace std;

cv::Mat_<double> LinearLSTriangulation(cv::Point3d u, //homogenous image point (u,v,1)
	cv::Matx34d P, // camera 1 matrix
	cv::Point3d u1, // homogenous image point in 2nd camera
	cv::Matx34d P1 // camera 2 matrix
	) {
	//build matrix A for homogenous equation system Ax = 0
	//assume X = (x,y,z,1), for Linear-LS method
	//which turns it into a AX = B system, where A is 4x3, X is 3x1 and B is 4x1
	Matx43d A(u.x*P(2, 0) - P(0, 0), u.x*P(2, 1) - P(0, 1), u.x*P(2, 2) - P(0, 2),
		u.y*P(2, 0) - P(1, 0), u.y*P(2, 1) - P(1, 1), u.y*P(2, 2) - P(1, 2),
		u1.x*P1(2, 0) - P1(0, 0), u1.x*P1(2, 1) - P1(0, 1), u1.x*P1(2, 2) - P1(0, 2),
		u1.y*P1(2, 0) - P1(1, 0), u1.y*P1(2, 1) - P1(1, 1), u1.y*P1(2, 2) - P1(1, 2)
		);
	Matx41d B(-(u.x*P(2, 3) - P(0, 3)),
		-(u.y*P(2, 3) - P(1, 3)),
		-(u1.x*P1(2, 3) - P1(0, 3)),
		-(u1.y*P1(2, 3) - P1(1, 3)));

	Mat_<double> X;
	solve(A, B, X, DECOMP_SVD);
	return X;
}

cv::Mat_<double> IterativeLiearLSTriangulation(cv::Point3d u,
	cv::Matx34d P,
	cv::Point3d u1,
	cv::Matx34d P1
	){

	double wi = 1, wi1 = 1;
	Mat_<double> X(4, 1);
	for (int i = 0; i<10; i++) { //Hartley suggests 10 iterations at most
		Mat_<double> X_ = LinearLSTriangulation(u, P, u1, P1);
		X(0) = X_(0); X(1) = X_(1); X(2) = X_(2); X(3) = 1.0;

		//recalculate weights
		double p2x = Mat_<double>(Mat_<double>(P).row(2)*X)(0);
		double p2x1 = Mat_<double>(Mat_<double>(P1).row(2)*X)(0);

		//breaking point
		if (fabsf(wi - p2x) <= EPSILON && fabsf(wi1 - p2x1) <= EPSILON) break;

		wi = p2x;
		wi1 = p2x1;

		//reweight equations and solve
		Matx43d A((u.x*P(2, 0) - P(0, 0)) / wi, (u.x*P(2, 1) - P(0, 1)) / wi, (u.x*P(2, 2) - P(0, 2)) / wi,
			(u.y*P(2, 0) - P(1, 0)) / wi, (u.y*P(2, 1) - P(1, 1)) / wi, (u.y*P(2, 2) - P(1, 2)) / wi,
			(u1.x*P1(2, 0) - P1(0, 0)) / wi1, (u1.x*P1(2, 1) - P1(0, 1)) / wi1, (u1.x*P1(2, 2) - P1(0, 2)) / wi1,
			(u1.y*P1(2, 0) - P1(1, 0)) / wi1, (u1.y*P1(2, 1) - P1(1, 1)) / wi1, (u1.y*P1(2, 2) - P1(1, 2)) / wi1
			);
		Mat_<double> B = (Mat_<double>(4, 1) << -(u.x*P(2, 3) - P(0, 3)) / wi,
			-(u.y*P(2, 3) - P(1, 3)) / wi,
			-(u1.x*P1(2, 3) - P1(0, 3)) / wi1,
			-(u1.y*P1(2, 3) - P1(1, 3)) / wi1
			);

		solve(A, B, X_, DECOMP_SVD);
		X(0) = X_(0); X(1) = X_(1); X(2) = X_(2); X(3) = 1.0;
	}
	return X;
}


double TriangulatePoints(const std::vector<cv::KeyPoint>& pt_set1,
	const std::vector<cv::KeyPoint>& pt_set2,
	const cv::Mat& k,
	const cv::Mat& kinv,
	const cv::Mat& distcoeff,
	const cv::Matx34d& p,
	const cv::Matx34d& p1,
	std::vector<CloudPoint>& pointcloud,
	std::vector<cv::KeyPoint>& correspImg1Pt
	){
	correspImg1Pt.clear();

	Matx44d p1_(
		p1(0, 0), p1(0, 1), p1(0, 2), p1(0, 3),
		p1(1, 0), p1(1, 1), p1(1, 2), p1(1, 3),
		p1(2, 0), p1(2, 1), p1(2, 2), p1(2, 3),
		0, 0, 0, 1);
	Matx44d Plinv(p1_.inv());

	vector<double> reproj_error;
	unsigned pts_size = pt_set1.size();

	Mat_<double> kp1 = k * Mat(p1);
	for (unsigned i = 0; i < pts_size; i++) {
		Point2f kp = pt_set1[i].pt;
		Point3d u(kp.x, kp.y, 1.0);
		Mat_<double> um = kinv * Mat_<double>(u);
		u.x = um(0);
		u.y = um(1);
		u.z = um(2);

		Point2f kp2 = pt_set2[i].pt;
		Point3d u1(kp2.x, kp2.y, 1.0);
		Mat_<double> um1 = kinv * Mat_<double>(u1);
		u1.x = um1(0);
		u1.y = um1(1);
		u1.z = um1(2);


		//计算得到3D点X
		Mat_<double> X = IterativeLiearLSTriangulation(u, p, u1, p1);

		//透视反投影
		Mat_<double> xPt_img = kp1 * X;
		Point2f xPt_img_(xPt_img(0) / xPt_img(2), xPt_img(1) / xPt_img(2));

		double reprj_err = norm(xPt_img_ - kp2);
		reproj_error.push_back(reprj_err);

		CloudPoint cp;
		cp.pt = Point3d(X(0), X(1), X(2));
		cp.reprojection_error = reprj_err;

		pointcloud.push_back(cp);
		correspImg1Pt.push_back(pt_set1[i]);

		Scalar mse = mean(reproj_error);

		return mse[0];
	}
}