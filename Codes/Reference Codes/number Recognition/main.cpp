#include <iostream>
#include <stdio.h>
#include "numberRecognition.h"

using namespace cv;
using namespace std;

char* videoFile = "video3.avi";
const int frame_rate = 25;

int main()
{
	train();

	IplImage *frame, *frame_gray;
	CvCapture* capture = cvCreateFileCapture(videoFile);
	int frame_count = 0;
	while(1)
	{
		frame = cvQueryFrame(capture); 
		if(!frame) break;
		cvShowImage("Video", frame); cvWaitKey(1000);
		frame_gray = cvCreateImage(cvGetSize(frame), IPL_DEPTH_8U, 1);
		cvCvtColor(frame, frame_gray, CV_RGB2GRAY);
		float x = get_time(frame_gray); cout<<x<<endl; 

		for(int i = 0; i<frame_rate-1; i++)
			frame = cvQueryFrame(capture);
	}
	cvDestroyWindow("Video");
	print();

	return 0;
}