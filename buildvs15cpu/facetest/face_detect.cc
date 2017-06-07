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

		vector<char *> attribute = { "����ӷ������","��ü��","�����˵�","���۴���","ͺͷ","������","�����","�����","��ͷ��","��ͷ��","ģ����","��ɫͷ��","Ũü��","���ཱུ�","˫�°�","���۾�","С����","�ҷ�","Ũױ","��ȧ","����","���΢��","ɽ���","С�۾�","û��С����","Բ�ε���","��ɫ�԰�","�����","ͺ��ͷ","��ɫ����","������","΢Ц","ֱͷ��","��ͷ��","������","��ñ��","���ں�","������","�����","�����" };

		vector<char *> pos_attribute = { "����","��ɫͷ��", "ģ����","Ũü", "���۾�", "����", "û�к���","������", "΢Ц",  "�����" };
		vector<char *> neg_attribute = {  "û������", "���Ǻ�ɫͷ��","������", "üë�Ƚϵ�", "û���۾�",  "Ů��", "�к���",  "û��������", "û��΢Ц",  "���ϵ�" };

		std::vector<float> data;
		AttributeDetection(img, data);
		int number[] = {5,8,10,12,15,20,24,30,31,39};
		/*int count = -1;
		int dis[] = { 1,2,3,4,5,6,7,8,9,10 };
		int attri[] = { 4,13,15,20,38,39 };
		vector<char *>text1 = { "ͺͷ","΢�ֵ���","���۾�","����","�����","�����" };
		vector<char *>text2 = { "��ͷ����","��ƫ�ݵ�","û���۾�","Ů��","û�����","�����" };
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






