#ifndef numberRecognition_H
#define numberRecognition_H

#include <cv.h>
#include <stdio.h>
#include <conio.h>
#include <fstream>

#include "opencv2/ml/ml.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"

using namespace std; 
using namespace cv;

#define n_digits 2
#define ROI_x 524//-50
#define ROI_y 79
#define ROI_height 22
#define ROI_width 25*n_digits

#define imgSize_x 25 
#define imgSize_y 22

ofstream out("name.txt");
CvMemStorage* mem = cvCreateMemStorage();
CvSeq* contours[10];

void print()
{
	printf("gotcha!"); 
	getch();
}

void train()
{
	char filename[255]; char destinationFile[255]; 
	for(int i = 0; i<10; i++)
	{
		sprintf(filename, "../images/new2/%d.jpg", i); 
		IplImage* img = cvLoadImage(filename, CV_LOAD_IMAGE_GRAYSCALE);
		cvSmooth(img, img, CV_MEDIAN);	
		cvCanny(img, img, 150, 255);

		sprintf(destinationFile, "../images/templates/%d.jpg", i);
		cvSaveImage(destinationFile, img);
	}
}


float get_time(IplImage* image)
{
	float time; 
	cvSetImageROI(image, cvRect(ROI_x, ROI_y, ROI_width, ROI_height));
	//cvShowImage("Clock", image); cvWaitKey(500); cvDestroyWindow("Clock");
	
	int digits[n_digits]; float d = 0; float d_max = 0;
	
	for(int i = 0; i<n_digits; i++)
	{
		IplImage *sample = cvCreateImage(cvGetSize(image), image->depth, image->nChannels);
		cvCopy(image, sample);
		cvSetImageROI(sample, cvRect(i*imgSize_x, 0, imgSize_x, imgSize_y));
		cvSmooth(sample, sample, CV_MEDIAN); 
		cvCanny(sample, sample, 150, 255);

		char templateFile[255]; IplImage* myTemplate; 
		CvMat* result = cvCreateMat(1, 1, CV_32FC1);
		for(int j = 0; j<10; j++)
		{
			sprintf(templateFile, "../images/templates/%d.jpg", j);
			myTemplate = cvLoadImage(templateFile, CV_LOAD_IMAGE_GRAYSCALE);
			cvMatchTemplate(sample, myTemplate, result, 5);
			d = CV_MAT_ELEM(*result, float, 0, 0);
			if(d > d_max)
			{
				d_max = d;
				digits[i] = j;
			}
		} 
	}
	time = digits[0]*10 + digits[1];
	return time;
}

#endif
