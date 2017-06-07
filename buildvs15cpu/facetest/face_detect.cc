#include <opencv2/opencv.hpp>
#include <iostream>
#include <string>
#include "i18nText.h"
#include <windows.h>
#include <vector>
#include "facedetect-dll.h"


using namespace cv;
using namespace std;
void AttributeDetection(const Mat img, std::vector<float> &data_cp);
int ToWcharWindows(const char* src, wchar_t* dest) {

	int nLen = MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, src, -1, NULL, 0);
	if (nLen == 0)
	{
		return NULL;
	}
	//dest = new wchar_t[nLen + 1];
	MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, src, -1, dest, nLen);
	return 0;
}
//#define MAX(a, b) (((a) > (b)) ? (a) : (b))

int main()
{
	cv::Mat frame;
	i18nText i18n;
	i18n.setFont("simhei.ttf");
	cv::VideoCapture cap(0);
	if(!cap.isOpened())  
	return -1;

	std::string winname = "camera";
	cv::namedWindow(winname,WINDOW_NORMAL);
	cv::setWindowProperty(winname, CV_WND_PROP_FULLSCREEN, CV_WINDOW_FULLSCREEN);	

	#define DETECT_BUFFER_SIZE 0x20000
	int * pResults = NULL;
	unsigned char * pBuffer = (unsigned char *)malloc(DETECT_BUFFER_SIZE);
	if (!pBuffer)
	{
		fprintf(stderr, "Can not alloc buffer.\n");
		return -1;
	}

	while (1)
	{
		cap >> frame;

		if (frame.empty())
		{
			printf("image null\n");
			continue;
		}
		Mat  img,gray;;
		cvtColor(frame, gray, CV_BGR2GRAY);
		pResults = facedetect_frontal_surveillance(pBuffer, (unsigned char*)(gray.ptr(0)), gray.cols, gray.rows, (int)gray.step,
			1.2f, 2, 48, 0, 0);
		printf("%d faces detected.\n", (pResults ? *pResults : 0));
		if (pResults == NULL)
			continue;
		int num = *pResults;
		vector<cv::Rect> rois;
		if (num == 0)
			continue;
		int maxface = 0, idx = 0;
		for (int i = 0; i < num; i++)
		{
			short * p = ((short*)(pResults + 1)) + 142 * i;
			int x = p[0];
			int y = p[1];
			int w = p[2];
			int h = p[3];
			int neighbors = p[4];
			int angle = p[5];
			/*int size = MAX(w, h) + 140;
			int w_diff = size - h;
			int h_diff = size - w + 100;
			int x2 = MAX(x - w_diff / 2., 0);
			int w2 = size;
			int y2 = MAX(y - h_diff / 2., 0);
			int h2 = size; */
			rectangle(frame, Rect(x, y, w, h), Scalar(0, 255, 0), 1);
			rois.push_back(Rect(x, y, w, h));
		}
		if (rois.size() > 0)
		{

			for (int i = 0; i < rois.size(); i++)
			{
				if (rois[i].width > maxface)
				{
					maxface = rois[i].width;
					idx = i;
				}
			}
			resize(frame(rois[idx]), img, cv::Size(128, 128));
			//faceRegion.push_back(rois[idx]);
			rectangle(frame, rois[idx], cv::Scalar(0, 0, 255), 2);
		}
		else
			continue;

		vector<char *> attribute = { "五点钟方向胡子","弯眉的","吸引人的","有眼袋的","秃头","有刘海","大嘴巴","大鼻子","黑头发","金头发","模糊的","棕色头发","浓眉的","胖嘟嘟的","双下巴","戴眼镜","小胡子","灰发","浓妆","高颧","男性","嘴巴微张","山羊胡","小眼睛","没有小胡子","圆形的脸","脸色苍白","尖鼻子","秃额头","面色红润","络腮胡","微笑","直头发","卷头发","戴耳环","戴帽子","擦口红","戴项链","打领带","年轻的" };

		vector<char *> pos_attribute = { "刘海","黑色头发", "模糊的","浓眉", "戴眼镜", "男性", "没有胡子","络腮胡", "微笑",  "年轻的" };
		vector<char *> neg_attribute = {  "没有刘海", "不是黑色头发","清晰的", "眉毛比较淡", "没戴眼镜",  "女性", "有胡子",  "没有络腮胡", "没有微笑",  "年老的" };

		std::vector<float> data;
		AttributeDetection(img, data);
		int number[] = {5,8,10,12,15,20,24,30,31,39};
		/*int count = -1;
		int dis[] = { 1,2,3,4,5,6,7,8,9,10 };
		int attri[] = { 4,13,15,20,38,39 };
		vector<char *>text1 = { "秃头","微胖的脸","戴眼镜","男性","打领带","年轻的" };
		vector<char *>text2 = { "有头发的","脸偏瘦的","没戴眼镜","女性","没打领带","成年的" };
		for (int i = 0; i <6 ; i++) 
		{
			if (data[attri[i]] > 0)
			{
				++count;
				wchar_t a[20];
				ToWcharWindows(text1[i], a);
				i18n.putText(frame, a, Point(MAX(rois[idx].x-20,0), rois[idx].y-dis[count]*15), CV_RGB(255, 0, 0));
			}
			else
			{   
				++count;
				wchar_t a[20];
				ToWcharWindows(text2[i], a);
				i18n.putText(frame, a, Point(MAX(rois[idx].x - 20, 0), rois[idx].y-dis[count]*15), CV_RGB(255, 0, 0));
			}
		}*/
		int distance = 1;
		for (int i = 0; i < 10; i++)
		{
			if ((i == 2) || (i == 5) || (i == 10))
			{
				if (data[number[i]] > 0)
				{
					wchar_t p[100];
					ToWcharWindows(pos_attribute[i], p);
					i18n.putText(frame, p, Point(MAX(rois[idx].x - 60, 0), rois[idx].y + distance * 17), CV_RGB(255, 0, 0));
					distance++;
				}
				else
				{
					wchar_t n[100];
					ToWcharWindows(neg_attribute[i], n);
					i18n.putText(frame, n, Point(MAX(rois[idx].x - 60, 0), rois[idx].y + distance * 17), CV_RGB(255, 0, 0));
					distance++;
				}
			}
			else
			{
				if (data[number[i]] > 0)
				{
					wchar_t a[100];
					ToWcharWindows(pos_attribute[i], a);
					i18n.putText(frame, a, Point(MAX(rois[idx].x - 60, 0), rois[idx].y + distance * 17), CV_RGB(255, 0, 0));
					distance++;
				}
			}
		}
     	cv::imshow(winname, frame);
		char c = cv::waitKey(5);
		if (c == 'c' || c == ' ')
			break;
	}

	free(pBuffer);

return 0;  
 
}






