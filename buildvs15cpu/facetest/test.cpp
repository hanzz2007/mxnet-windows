#include <opencv2/opencv.hpp>
#include <iostream>
#include <string>
#include<opencv2/freetype.hpp>
using namespace cv;
int main() {
	Mat img(500, 500, CV_8UC3, Scalar(0, 0, 0));
	cv::Ptr<cv::freetype::FreeType2> ft2 = cv::freetype::createFreeType2();
	ft2->loadFontData("C:/Users/feifei/Desktop/mxnet/buildvs15cpu/facetest/simhei.ttf", 0);
	ft2->putText(img, "测试", Point(100,100), 20, cv::Scalar(255, 0, 0), -1, cv::LINE_AA, false);
	imshow("test", img);
	waitKey(0);
	return 0;
}