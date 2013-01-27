#include "opencv2/ml/ml.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <iostream>
#include <stdio.h>

using namespace cv;
using namespace std;

char* videoFile = "video.avi";
const int frame_rate = 30;

int main()
{
	CvCapture* capture = cvCreateFileCapture(videoFile);
	IplImage* frame;
	
	char imgFileName[32]; 
	int count = 0;
	while(1)
	{
		for (int i=0;i<2; i++)
		{
		frame = cvQueryFrame(capture); 
		if (frame==0) break;
		
		sprintf(imgFileName, "../images/%d.jpg", count+1); 
		cvSaveImage(imgFileName, frame); count++;
		}

		for (int i = 0; i<25;i++)
		{
			frame = cvQueryFrame(capture); 
			if (frame==0) break;
		}
		


	}
}
