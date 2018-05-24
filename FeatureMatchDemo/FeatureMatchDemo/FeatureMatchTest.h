#pragma once

#include <opencv.hpp>
#include <memory>
#include <opencv2\features2d\features2d.hpp>

struct Pattern
{
	cv::Mat image;
	std::vector<cv::KeyPoint>  keypoints;
	cv::Mat descriptors;

	Pattern(cv::Mat& img) :
		image(img) {}
};


class FeatureMatchTest
{
public:
	FeatureMatchTest(std::shared_ptr<Pattern> left, std::shared_ptr<Pattern> right, std::shared_ptr<cv::DescriptorMatcher> matcher);

	void match(std::vector<cv::DMatch>& matches);

	void knnMatch(std::vector<cv::DMatch>& matches);

	void refineMatcheswithHomography(std::vector<cv::DMatch>& matches, double reprojectionThreshold, cv::Mat& homography);

	void refineMatchesWithFundmentalMatrix(std::vector<cv::DMatch>& matches, cv::Mat& F);

	//void showMatches(const std::vector<cv::DMatch>& matches,cv::Mat& matchesImg, const string& windowName);

private:
	std::shared_ptr<cv::DescriptorMatcher> matcher;
	std::shared_ptr<Pattern> leftPattern;
	std::shared_ptr<Pattern> rightPattern;
};

