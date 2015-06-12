///////////////////////////////////////////////////////////////////////////////////
// This code sample demonstrates the use of DUO SDK in your own applications
// For updates and file downloads go to: http://duo3d.com/
// Copyright 2014 (c) Code Laboratories, Inc.  All rights reserved.

//Code modified by Kavinayan Sivakumar
///////////////////////////////////////////////////////////////////////////////////
#include "Sample.h"
#include <fstream>

#define WIDTH	640
#define HEIGHT	480
#define FPS		30

int main(int argc, char* argv[])
{
	printf("DUOLib Version:       v%s\n", GetLibVersion());

	// Open DUO camera and start capturing
	if(!OpenDUOCamera(WIDTH, HEIGHT, FPS))
	{
		printf("Could not open DUO camera\n");
		return 0;
	}
	// Create OpenCV windows
	cvNamedWindow("Left");
	cvNamedWindow("Right");

	// Set exposure and LED brightness
	SetExposure(80);
	SetLed(80);

	// Create image headers for left & right frames
	IplImage *left = cvCreateImageHeader(cvSize(WIDTH, HEIGHT), IPL_DEPTH_8U, 1);
	IplImage *right = cvCreateImageHeader(cvSize(WIDTH, HEIGHT), IPL_DEPTH_8U, 1);
    int imagenum = 0;
    std::ofstream myFile("data.csv");
    myFile << "Picture Index,Timestamp,Acceleration[x],Acceleration[y],Acceleration[z],Gyro[x],Gyro[y],Gyro[z],Temperature,Pitch,Roll"<< std::endl;
    
    float pitch, roll;
    float yaw = 0, yawdeg;
    double secs;
    int64 t0, t1;
    //float yawx, yawy, yaw;
    //float normmagx, normmagy, normmagz, magnorm;
    
	// Run capture loop until <Esc> key is pressed
	while((cvWaitKey(1) & 0xff) != 27)
	{
		// Capture DUO frame
		PDUOFrame pFrameData = GetDUOFrame();
		if(pFrameData == NULL) continue;
        
        t0 = cv::getTickCount();

		// Set the image data
		left->imageData = (char*)pFrameData->leftData;
		right->imageData = (char*)pFrameData->rightData;
        
        // Calculate oval metrics
        roll = atan(pFrameData->accelData[0]/sqrt(pow(pFrameData->accelData[1], 2) + pow(pFrameData->accelData[2], 2)));
        pitch = atan(pFrameData->accelData[1]/sqrt(pow(pFrameData->accelData[0], 2) + pow(pFrameData->accelData[2], 2)));
        /*magnorm = sqrt((pow(pFrameData->magData[0], 2)) + (pow(pFrameData->magData[1], 2)) + (pow(pFrameData->magData[2], 2)));
        normmagx = pFrameData->magData[0]/magnorm;
        normmagy = pFrameData->magData[1]/magnorm;
        normmagz = pFrameData->magData[2]/magnorm;
        yawx = normmagx*cos(pitch) + normmagy*sin(pitch)*sin(roll) + normmagz*sin(pitch)*cos(roll);
        yawy = -normmagy*cos(roll) + normmagz*sin(roll);
        yaw = atan(yawx/yawy);*/
        pitch = pitch * (180.0/(atan(1)*4));
        roll = roll * (180.0/(atan(1)*4));
        printf("  pitch: %10.1f \n", pitch);
        printf("  roll: %10.1f \n", roll);

		// Process images here (optional)
        Mat leftmat = left;
        Mat rightmat = right;

		// Display images
		cvShowImage("Left", left);
		cvShowImage("Right", right);
        
        t1 = cv::getTickCount();
        secs = (t1-t0)/cv::getTickFrequency();
        yaw = pFrameData->gyroData[2]*secs + yaw;
        
        // Manual constant
        //yawdeg = yaw * (180.0/(atan(1)*4)) * 12.465;
        //printf("  yaw: %10.1f \n", yawdeg);

        //Save images
        std::stringstream leftfile;
        std::stringstream rightfile;
        leftfile << "sampleimages/leftimage" << imagenum << ".bmp";
        rightfile << "sampleimages/rightimage" << imagenum << ".bmp";
        
        cv::imwrite(leftfile.str(), leftmat);
        cv::imwrite(rightfile.str(), rightmat);
        
        // Export data information to csv
        myFile << imagenum << "," << pFrameData->timeStamp/10.0f << "," << pFrameData->accelData[0] << "," << pFrameData->accelData[1] << "," << pFrameData->accelData[2] << "," << pFrameData->gyroData[0] << "," << pFrameData->gyroData[1] << "," << pFrameData->gyroData[2] << "," << pFrameData->tempData << "," << pitch << "," << roll << std::endl;
        printf("DUO Frame Timestamp: %10.1f ms\n", pFrameData->timeStamp/10.0f);
        printf("  Accelerometer: [%8.5f, %8.5f, %8.5f]\n", pFrameData->accelData[0],
               pFrameData->accelData[1],
               pFrameData->accelData[2]);
        printf("  Gyro:          [%8.5f, %8.5f, %8.5f]\n", pFrameData->gyroData[0],
               pFrameData->gyroData[1],
               pFrameData->gyroData[2]);
        printf("  Temperature:   %8.6f C\n", pFrameData->tempData);
        printf("------------------------------------------------------\n");
        imagenum++;
	}
	// Release image headers
	cvReleaseImageHeader(&left);
	cvReleaseImageHeader(&right);

	// Close DUO camera
	CloseDUOCamera();
    
    std::cout << "Saved " << imagenum << " files ";
	return 0;
}
