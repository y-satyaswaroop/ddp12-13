#include <cv.h>
#include <fstream>
#include <iostream>
#include <highgui.h>

#define fx 705.808250
#define fy 705.634250
#define cx 314.275667
#define cy 241.660750

#define pi 3.14159265359
#define frame_rate 30
#define radius_earth 6371000

#define videoFile "video.avi"
#define sensorFile "measurements.txt"

using namespace std;
ifstream inputs(sensorFile); 

float measurements[10]; CvPoint myPoint; 

void get_measurements()
{
	inputs>>measurements[0]; // time
	inputs>>measurements[1]; inputs>>measurements[2]; inputs>>measurements[3]; // phi, theta, psi
	inputs>>measurements[4]; inputs>>measurements[5]; inputs>>measurements[6]; // location of uav
	inputs>>measurements[7]; inputs>>measurements[8]; inputs>>measurements[9]; // location of target (actual)
}

void skip(CvCapture* capture, int n_frames)
{
	IplImage* frame;
	for(int i=0; i<n_frames; i++)
	{
		frame = cvQueryFrame(capture);
		get_measurements();
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

CvMat* findTargetPosition()
{
	float t, phi, theta, psi, X_UAV, Y_UAV, H_UAV, x, y, z;
	int u, v;

	t = measurements[0];
	phi = measurements[1]*pi/180.0f; 
	theta = measurements[2]*pi/180.0f; 
	psi = pi/2; // psi = measurements[3]*pi/180.0f;
	X_UAV = (measurements[4] - measurements[7])*radius_earth*pi/180; 
	Y_UAV = (measurements[5] - measurements[8])*radius_earth*pi/180;
	H_UAV = measurements[6] - measurements[9];
	x = 0.0f; y = 0.0f; z = 0.0f;

	u = myPoint.x; v = myPoint.y; // Image Data - Pixel Location of the Target
	cout<<t<<" "<<phi*180.0/pi<<" "<<theta*180.0/pi<<" "<<psi*180.0/pi<<" "<<X_UAV<<" "<<Y_UAV<<" "<<H_UAV<<" "<<u<<" "<<v<<endl;

	CvMat* C = cvCreateMat(4,4,CV_32FC1); // Camera Calibration Matrix
	CV_MAT_ELEM(*C, float,0,0) = fx;
	CV_MAT_ELEM(*C, float,0,1) = 0;
	CV_MAT_ELEM(*C, float,0,2) = cx;
	CV_MAT_ELEM(*C, float,0,3) = 0;
	CV_MAT_ELEM(*C, float,1,0) = 0;
	CV_MAT_ELEM(*C, float,1,1) = fy;
	CV_MAT_ELEM(*C, float,1,2) = cy;
	CV_MAT_ELEM(*C, float,1,3) = 0;
	CV_MAT_ELEM(*C, float,2,0) = 0;
	CV_MAT_ELEM(*C, float,2,1) = 0;
	CV_MAT_ELEM(*C, float,2,2) = 1;
	CV_MAT_ELEM(*C, float,2,3) = 0;
	CV_MAT_ELEM(*C, float,3,0) = 0;
	CV_MAT_ELEM(*C, float,3,1) = 0;
	CV_MAT_ELEM(*C, float,3,2) = 0;
	CV_MAT_ELEM(*C, float,3,3) = 1;
		
	CvMat* T_b_c = cvCreateMat(4,4,CV_32FC1);
	CV_MAT_ELEM(*T_b_c, float,0,0) = 0;
	CV_MAT_ELEM(*T_b_c, float,0,1) = 1;
	CV_MAT_ELEM(*T_b_c, float,0,2) = 0;
	CV_MAT_ELEM(*T_b_c, float,0,3) = x;
	CV_MAT_ELEM(*T_b_c, float,1,0) = -1;
	CV_MAT_ELEM(*T_b_c, float,1,1) = 0;
	CV_MAT_ELEM(*T_b_c, float,1,2) = 0;
	CV_MAT_ELEM(*T_b_c, float,1,3) = y;
	CV_MAT_ELEM(*T_b_c, float,2,0) = 0;
	CV_MAT_ELEM(*T_b_c, float,2,1) = 0;
	CV_MAT_ELEM(*T_b_c, float,2,2) = 1;
	CV_MAT_ELEM(*T_b_c, float,2,3) = z;
	CV_MAT_ELEM(*T_b_c, float,3,0) = 0;
	CV_MAT_ELEM(*T_b_c, float,3,1) = 0;
	CV_MAT_ELEM(*T_b_c, float,3,2) = 0;
	CV_MAT_ELEM(*T_b_c, float,3,3) = 1;

	CvMat* T_V_b = cvCreateMat(4,4,CV_32FC1);
	CV_MAT_ELEM(*T_V_b, float,0,0) = cos(theta)*cos(psi);
	CV_MAT_ELEM(*T_V_b, float,0,1) = cos(theta)*sin(psi);
	CV_MAT_ELEM(*T_V_b, float,0,2) = -sin(theta);
	CV_MAT_ELEM(*T_V_b, float,0,3) = 0;
	CV_MAT_ELEM(*T_V_b, float,1,0) = sin(phi)*sin(theta)*cos(psi)-cos(phi)*sin(psi);
	CV_MAT_ELEM(*T_V_b, float,1,1) = sin(phi)*sin(theta)*sin(psi)+cos(phi)*cos(psi);
	CV_MAT_ELEM(*T_V_b, float,1,2) = sin(phi)*cos(theta);
	CV_MAT_ELEM(*T_V_b, float,1,3) = 0;
	CV_MAT_ELEM(*T_V_b, float,2,0) = cos(phi)*sin(theta)*cos(psi)+sin(phi)*sin(psi);
	CV_MAT_ELEM(*T_V_b, float,2,1) = cos(phi)*sin(theta)*sin(psi)-sin(phi)*cos(psi);
	CV_MAT_ELEM(*T_V_b, float,2,2) = cos(phi)*cos(theta);
	CV_MAT_ELEM(*T_V_b, float,2,3) = 0;
	CV_MAT_ELEM(*T_V_b, float,3,0) = 0;
	CV_MAT_ELEM(*T_V_b, float,3,1) = 0;
	CV_MAT_ELEM(*T_V_b, float,3,2) = 0;
	CV_MAT_ELEM(*T_V_b, float,3,3) = 1;

	CvMat* T_I_v = cvCreateMat(4,4,CV_32FC1);
	CV_MAT_ELEM(*T_I_v, float,0,0) = 1;
	CV_MAT_ELEM(*T_I_v, float,0,1) = 0;
	CV_MAT_ELEM(*T_I_v, float,0,2) = 0;
	CV_MAT_ELEM(*T_I_v, float,0,3) = -X_UAV;
	CV_MAT_ELEM(*T_I_v, float,1,0) = 0;
	CV_MAT_ELEM(*T_I_v, float,1,1) = 1;
	CV_MAT_ELEM(*T_I_v, float,1,2) = 0;
	CV_MAT_ELEM(*T_I_v, float,1,3) = -Y_UAV;
	CV_MAT_ELEM(*T_I_v, float,2,0) = 0;
	CV_MAT_ELEM(*T_I_v, float,2,1) = 0;
	CV_MAT_ELEM(*T_I_v, float,2,2) = 1;
	CV_MAT_ELEM(*T_I_v, float,2,3) = H_UAV;
	CV_MAT_ELEM(*T_I_v, float,3,0) = 0;
	CV_MAT_ELEM(*T_I_v, float,3,1) = 0;
	CV_MAT_ELEM(*T_I_v, float,3,2) = 0;
	CV_MAT_ELEM(*T_I_v, float,3,3) = 1;

	CvMat* q = cvCreateMat(4,1,CV_32FC1);
	CV_MAT_ELEM(*q, float,0,0) = u;
	CV_MAT_ELEM(*q, float,1,0) = v;
	CV_MAT_ELEM(*q, float,2,0) = 1;
	CV_MAT_ELEM(*q, float,3,0) = 1;

	CvMat* TbcTvb = cvCreateMat(4,4,CV_32FC1);
	cvMatMul(T_b_c, T_V_b, TbcTvb);
		
	CvMat* TbcTvbTiv = cvCreateMat(4,4,CV_32FC1);
	cvMatMul(TbcTvb,T_I_v, TbcTvbTiv);

	CvMat* CTbcTvbTiv = cvCreateMat(4,4,CV_32FC1);
	cvMatMul(C, TbcTvbTiv, CTbcTvbTiv);
		
	CvMat* inv_TbcTvbTiv  = cvCreateMat(4,4,CV_32FC1);
	CvMat* inv_CTbcTvbTiv = cvCreateMat(4,4,CV_32FC1);
	cvInvert(TbcTvbTiv, inv_TbcTvbTiv,0);
	cvInvert(CTbcTvbTiv, inv_CTbcTvbTiv,0);
		
	CvMat* e4 = cvCreateMat(4,1,CV_32FC1);
	CV_MAT_ELEM(*e4, float,0,0) = 0;
	CV_MAT_ELEM(*e4, float,1,0) = 0;
	CV_MAT_ELEM(*e4, float,2,0) = 0;
	CV_MAT_ELEM(*e4, float,3,0) = 1;
	CvMat* p_cc = cvCreateMat(4,1, CV_32FC1);
	cvMatMul(inv_TbcTvbTiv, e4, p_cc); 

	CvMat* p_obj_bar = cvCreateMat(4,1, CV_32FC1);
	cvMatMul(inv_CTbcTvbTiv, q, p_obj_bar); 

	float p_cc_z      = CV_MAT_ELEM(*p_cc, float,2,0);
	float p_obj_bar_z = CV_MAT_ELEM(*p_obj_bar, float,2,0);
		
	float lambda = -p_cc_z/(p_obj_bar_z - p_cc_z); 
	CvMat* p_obj  = cvCreateMat(4, 1, CV_32FC1);
	CvMat* p_dash = cvCreateMat(4, 1, CV_32FC1);
	cvAddWeighted(p_obj_bar, lambda, p_cc, -lambda, 0, p_dash);
	cvAdd(p_cc, p_dash, p_obj);
	
	return p_obj;
}

int main()
{
	CvMat* targetPosition = cvCreateMat(4,1,CV_32FC1); 
	float X_tgt, Y_tgt, Z_tgt, err_X, err_Y, err_Z, err_net;

	CvCapture* capture = cvCreateFileCapture(videoFile);
	IplImage* frame;
	
	while(1)
	{
		frame = cvQueryFrame(capture); 
		cvShowImage("Video", frame);
		cvSetMouseCallback("Video", my_mouse_callback, (void*) frame);
		char c = cvWaitKey(0);
		if(c == 's')
			skip(capture, frame_rate);
		else
		{
			get_measurements();
			//for(int i=0;i<7;i++) cout<<measurements[i]<<" "; cout<<endl;

			targetPosition = findTargetPosition();
			X_tgt = CV_MAT_ELEM(*targetPosition, float,0,0);
			Y_tgt = CV_MAT_ELEM(*targetPosition, float,1,0);
			Z_tgt = CV_MAT_ELEM(*targetPosition, float,2,0);

			err_X = X_tgt - measurements[7]*pi/180.0f;
			err_Y = Y_tgt - measurements[8]*pi/180.0f;
			err_Z = Z_tgt - measurements[9]*pi/180.0f;

			err_net = sqrt(err_X*err_X + err_Y*err_Y + err_Z*err_Z);
			cout<<"Errors are : "<<err_X<<" "<<err_Y<<" "<<err_Z<<endl;
			cout<<"Net Error is "<<err_net<<endl;
			skip(capture, frame_rate-1);
		}
	}
}