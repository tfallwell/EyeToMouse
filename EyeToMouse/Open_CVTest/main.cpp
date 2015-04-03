/***************************************************************************
 *Timm and Barth. Accurate eye centre localisation by means of gradients.  *
 *In Proceedings of the Int. Conference on Computer Theory and Applications* 
 *(VISAPP), volume 1, pages 125-130, Algarve, Portugal, 2011. INSTICC.	   *
 ***************************************************************************
 *Modified by:	Thomas Fallwell 
 *Date:			02/14/15
 */

#include <opencv2\objdetect\objdetect.hpp>
#include <opencv2\highgui\highgui.hpp>
#include <opencv2\imgproc\imgproc.hpp>

#include <iostream>
#include <queue>
#include <stdio.h>
#include <math.h>
//added by Thomas Fallwell 02/28/15
#include <Windows.h>
#include <WinDef.h>
#include <process.h>
#include "globals.h"
#include "main.h"
//END ADDED
#include "constants.h"
#include "findEyeCenter.h"
#include "findEyeCorner.h"
//the following were added 3/02/15 by Thomas Fallwell
#include "calibration.h"
#include "mouse.h"
#include "config.h"

#pragma comment(lib, "user32.lib")
using namespace std;
/** Global Variables */
GazePoint cursorPos;
GazePoint prevCursorPos;
MissCount missCount;
cv::String face_cascade_name = "../res/haarcascade_frontalface_alt.xml";
cv::CascadeClassifier face_cascade;
std::string main_window_name = "Capture - Face detection";
std::string face_window_name = "Capture - Face";
cv::RNG rng(12345);
cv::Mat debugImage;
cv::Mat skinCrCbHist = cv::Mat::zeros(cv::Size(256, 256), CV_8UC1);
CvCapture* capture;
cv::Mat frame;


bool debugMode = FALSE, isCapturing = TRUE, isCalibrating = TRUE;
/**
* @function main
*/
void init(int count, char** params) {

	//get all parameters
	for (int i = 0; i < count; i++) {
		if (strcmp(params[i], "debug") == 0) debugMode = TRUE;
		if (strcmp(params[i], "calibrate") == 0) isCalibrating = TRUE;
		if (strcmp(params[i], "capture") == 0) isCapturing = TRUE;
	}

	/*the following were commented out by Thomas Fallwell 02/14/15*/

	cv::namedWindow(main_window_name, CV_WINDOW_NORMAL);
	cv::moveWindow(main_window_name, 400, 100);
	cv::namedWindow(face_window_name, CV_WINDOW_NORMAL);
	cv::moveWindow(face_window_name, 10, 100);

	//cv::namedWindow("Right Eye",CV_WINDOW_NORMAL);
	//cv::moveWindow("Right Eye", 10, 600);
	//cv::namedWindow("Left Eye",CV_WINDOW_NORMAL);
	//cv::moveWindow("Left Eye", 10, 800);
	//cv::namedWindow("aa",CV_WINDOW_NORMAL);
	//cv::moveWindow("aa", 10, 800);
	//cv::namedWindow("aaa",CV_WINDOW_NORMAL);
	//cv::moveWindow("aaa", 10, 800);
	/*END COMMENTED*/

	createCornerKernels();
	ellipse(skinCrCbHist, cv::Point(113, 155.6), cv::Size(23.4, 15.2),
		43.0, 0.0, 360.0, cv::Scalar(255, 255, 255), -1);

}

int main( int argc, char** argv ) {

	//int f; //arbitrary input for debugging and attaching to process
	//cin >> f;
	init(argc, argv);
	char* path = "../res/cal.config";
	// Load the cascades
	if (!face_cascade.load(face_cascade_name)){ 
		printf("--(!)Error loading face cascade, please change face_cascade_name in source code.\n"); return -1; };
	// Read the video stream
	capture = cvCaptureFromCAM(0);
	if (capture) { //if can read from webcam
		//if called with "calibrate" param, calibrate and exit
		if (isCalibrating && isCapturing) {
			::cal.calibrate(); //defined in calibration.h
			Config::setConfig(path);

			if (!Config::getConfig(path))
				return -1; //if config not set
			initCapure();
		}
		else if (isCalibrating) {
			::cal.calibrate(); //defined in calibration.h
			Config::setConfig(path);
		}
		else if (isCapturing) { //capture gaze input until process is killed
			if (!Config::getConfig(path))
				return -1; //if config not set
			initCapure();
		}
		else { 
			return -1;
		}
	}
	releaseCornerKernels();
	return 0;
} //end main()

void initCapure() {

	while (1) {
		captureFace(NULL);
		int c = cv::waitKey(10);
		if ((char)c == 'c') { break; }
		if ((char)c == 'f') {
			imwrite("frame.png", frame);
		}
	}
}
//only captures one frame, should be called from loop for mouse tracking
int captureFace(char* arg) {

		//while (true) {
			frame = cvQueryFrame(capture);
			// mirror it
			cv::flip(frame, frame, 1);
			frame.copyTo(debugImage);
			int coordinate = 0;
			// Apply the classifier to the frame
			if (!frame.empty()) {
				coordinate = detectAndDisplay(frame, arg);
			}
			else {
				printf(" --(!) No captured frame -- Break!");
				//break;
			}
			//shows whole webcam capture
			imshow(main_window_name,debugImage);

			/*int c = cv::waitKey(10);
			if ((char)c == 'c') { break; }
			if ((char)c == 'f') {
				imwrite("frame.png", frame);
			}*/
		//}
	//}


	return coordinate;
}

int findEyes(cv::Mat frame_gray, cv::Rect face, char* arg) {
	cv::Mat faceROI = frame_gray(face);
	cv::Mat debugFace = faceROI;

	if (kSmoothFaceImage) {
		double sigma = kSmoothFaceFactor * face.width;
		GaussianBlur(faceROI, faceROI, cv::Size(0, 0), sigma);
	}
	//-- Find eye regions and draw them
	int eye_region_width = face.width * (kEyePercentWidth / 100.0);
	int eye_region_height = face.width * (kEyePercentHeight / 100.0);
	int eye_region_top = face.height * (kEyePercentTop / 100.0);
	cv::Rect leftEyeRegion(face.width*(kEyePercentSide / 100.0),
		eye_region_top, eye_region_width, eye_region_height);
	cv::Rect rightEyeRegion(face.width - eye_region_width - face.width*(kEyePercentSide / 100.0),
		eye_region_top, eye_region_width, eye_region_height);

	//-- Find Eye Centers
	cv::Point leftPupil = findEyeCenter(faceROI, leftEyeRegion, "Left Eye");
	cv::Point rightPupil = findEyeCenter(faceROI, rightEyeRegion, "Right Eye");
	// get corner regions
	cv::Rect leftRightCornerRegion(leftEyeRegion);
	leftRightCornerRegion.width -= leftPupil.x;
	leftRightCornerRegion.x += leftPupil.x;
	leftRightCornerRegion.height /= 2;
	leftRightCornerRegion.y += leftRightCornerRegion.height / 2;
	cv::Rect leftLeftCornerRegion(leftEyeRegion);
	leftLeftCornerRegion.width = leftPupil.x;
	leftLeftCornerRegion.height /= 2;
	leftLeftCornerRegion.y += leftLeftCornerRegion.height / 2;
	cv::Rect rightLeftCornerRegion(rightEyeRegion);
	rightLeftCornerRegion.width = rightPupil.x;
	rightLeftCornerRegion.height /= 2;
	rightLeftCornerRegion.y += rightLeftCornerRegion.height / 2;
	cv::Rect rightRightCornerRegion(rightEyeRegion);
	rightRightCornerRegion.width -= rightPupil.x;
	rightRightCornerRegion.x += rightPupil.x;
	rightRightCornerRegion.height /= 2;
	rightRightCornerRegion.y += rightRightCornerRegion.height / 2;
	rectangle(debugFace, leftRightCornerRegion, 200);
	rectangle(debugFace, leftLeftCornerRegion, 200);
	rectangle(debugFace, rightLeftCornerRegion, 200);
	rectangle(debugFace, rightRightCornerRegion, 200);
	// change eye centers to face coordinates

	//added by Thomas 2/22/2015
	//BEGIN
	int eye_x = rightPupil.x;
	int eye_y = rightPupil.y;

	//printf("max X = %d, max Y = %d \n", gaze.MAX_X, gaze.MAX_Y);
	//printf("min X = %d, min Y = %d \n", gaze.MIN_X, gaze.MIN_Y);
	//the following logic is only true during is for calibration
	if (arg == "left" || arg == "right") {
		return (eye_x);
	}
	else if (arg == "top" || arg == "bottom") {
		return eye_y;
	}
	else {
		Mouse m;
		//only map mouse if in area of interest
		if (eye_x <= gaze.MAX_X && eye_x >= gaze.MIN_X
			&& eye_y <= gaze.MAX_Y && eye_y >= gaze.MIN_Y){

			m.directMapMouse(eye_x, eye_y);
			//m.analogMouse(eye_x, eye_y);
		
		}
		else {
			//::cal.checkMiss(eye_x, eye_y);
		}
		//END ADDED

		rightPupil.x += rightEyeRegion.x;
		rightPupil.y += rightEyeRegion.y;
		leftPupil.x += leftEyeRegion.x;
		leftPupil.y += leftEyeRegion.y;
		// draw eye centers
		circle(debugFace, rightPupil, 3, 1234);
		circle(debugFace, leftPupil, 3, 1234);

		//The following was added 2-14-15 by Thomas Fallwell
		//BEGIN ADDED
		/*int eye_x = leftPupil.x - (leftEyeRegion.width / 2);
		int eye_y = leftPupil.y - (leftEyeRegion.height / 2);
		printf("Eye X = %d, Eye Y = %d \n", eye_x, eye_y);
		printf("Left Eye Region X = %d, Left Eye Region Y = %d \n", leftEyeRegion.width, leftEyeRegion.height);*/
		//END ADDED

	}
	//-- Find Eye Corners
	if (kEnableEyeCorner) {
		cv::Point2f leftRightCorner = findEyeCorner(faceROI(leftRightCornerRegion), true, false);
		leftRightCorner.x += leftRightCornerRegion.x;
		leftRightCorner.y += leftRightCornerRegion.y;
		cv::Point2f leftLeftCorner = findEyeCorner(faceROI(leftLeftCornerRegion), true, true);
		leftLeftCorner.x += leftLeftCornerRegion.x;
		leftLeftCorner.y += leftLeftCornerRegion.y;
		cv::Point2f rightLeftCorner = findEyeCorner(faceROI(rightLeftCornerRegion), false, true);
		rightLeftCorner.x += rightLeftCornerRegion.x;
		rightLeftCorner.y += rightLeftCornerRegion.y;
		cv::Point2f rightRightCorner = findEyeCorner(faceROI(rightRightCornerRegion), false, false);
		rightRightCorner.x += rightRightCornerRegion.x;
		rightRightCorner.y += rightRightCornerRegion.y;
		circle(faceROI, leftRightCorner, 3, 200);
		circle(faceROI, leftLeftCorner, 3, 200);
		circle(faceROI, rightLeftCorner, 3, 200);
		circle(faceROI, rightRightCorner, 3, 200);
	}



	imshow(face_window_name, faceROI);
	//  cv::Rect roi( cv::Point( 0, 0 ), faceROI.size());
	//  cv::Mat destinationROI = debugImage( roi );
	//  faceROI.copyTo( destinationROI );
	return 0;
} //end findEyes()


cv::Mat findSkin(cv::Mat &frame) {
	cv::Mat input;
	cv::Mat output = cv::Mat(frame.rows, frame.cols, CV_8U);

	cvtColor(frame, input, CV_BGR2YCrCb);

	for (int y = 0; y < input.rows; ++y) {
		const cv::Vec3b *Mr = input.ptr<cv::Vec3b>(y);
		//    uchar *Or = output.ptr<uchar>(y);
		cv::Vec3b *Or = frame.ptr<cv::Vec3b>(y);
		for (int x = 0; x < input.cols; ++x) {
			cv::Vec3b ycrcb = Mr[x];
			//      Or[x] = (skinCrCbHist.at<uchar>(ycrcb[1], ycrcb[2]) > 0) ? 255 : 0;
			if (skinCrCbHist.at<uchar>(ycrcb[1], ycrcb[2]) == 0) {
				Or[x] = cv::Vec3b(0, 0, 0);
			}
		}
	}
	return output;
}

/**
* @function detectAndDisplay
*/
int detectAndDisplay(cv::Mat frame, char* arg) {
	std::vector<cv::Rect> faces;
	//cv::Mat frame_gray;

	std::vector<cv::Mat> rgbChannels(3);
	cv::split(frame, rgbChannels);
	cv::Mat frame_gray = rgbChannels[2];
	int coordinate = 0;
	//cvtColor( frame, frame_gray, CV_BGR2GRAY );
	//equalizeHist( frame_gray, frame_gray );
	//cv::pow(frame_gray, CV_64F, frame_gray);
	//-- Detect faces
	face_cascade.detectMultiScale(frame_gray, faces, 1.1, 2, 0 | CV_HAAR_SCALE_IMAGE | CV_HAAR_FIND_BIGGEST_OBJECT, cv::Size(150, 150));
	//  findSkin(debugImage);

	for (int i = 0; i < faces.size(); i++)
	{
		rectangle(debugImage, faces[i], 1234);
	}
	//-- Show what you got
	if (faces.size() > 0) {
		coordinate = findEyes(frame_gray, faces[0], arg);
	}
	return coordinate;
} //dend detectAndDisplay()


