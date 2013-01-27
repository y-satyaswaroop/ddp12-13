#include "opencv2/ml/ml.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <iostream>
#include <stdio.h>

using namespace cv;
using namespace std;

char* videoFile = "video.avi";
const int frame_rate = 30;

CvPoint pt1, pt2, myPoint;

// Displaying only the Region of Interest as defined by user at t = 0 (using mouse)
void my_mouse_callback(int event, int x, int y, int flags, void* param);

int main()
{
	CvCapture* capture = cvCreateFileCapture(videoFile);
	IplImage *frame; IplImage *character;

	char imgFileName[32]; 
	int count = 0;

	while(1)
	{
		frame = cvQueryFrame(capture); 
		cvShowImage("Video", frame); cvWaitKey(30);

		if (frame==0) break;

		if (count==0)
		{
			cvSetMouseCallback("Video", my_mouse_callback, (void*) frame);
			// Ask user to give region of interest using a rectanglar box
			cout<<"Select 1st point and press enter"<<endl;
			cvWaitKey(0);
			pt1.x = myPoint.x; pt1.y = myPoint.y; 
			cout<<"Select 2nd point and press enter"<<endl;
			char c = cvWaitKey(0);
			pt2.x = myPoint.x; pt2.y = myPoint.y; 
			count++;
			if (c=='s') count = 0;
		}

		if (count >= 1)
		{
			character = frame;
			int width = abs(pt2.x-pt1.x);
			int height = abs(pt2.y-pt1.y);

			cvSetImageROI(character, cvRect(pt1.x, pt1.y, width, height));
			cvShowImage("Digit", character); cvWaitKey(30);
			sprintf(imgFileName, "../images/PIC00%d.jpg", count); 
			cvSaveImage(imgFileName, character); count++;
		}
	}
}

void my_mouse_callback(int event, int x, int y, int flags, void* param)
{
	IplImage* image = (IplImage*) param;
	switch(event)
	{
	case CV_EVENT_LBUTTONDOWN:
		{
			myPoint = cvPoint(x,y); //cout<<x<<" "<<y<<endl;
		}
	}
}
