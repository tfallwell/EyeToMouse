/*added by Thomas Fallwell 02/28/15
*this class is for mapping eyes to mouse
*/
#include <opencv2\highgui\highgui.hpp>

#include "mouse.h"

#include <iostream>
#include <queue>
#include <stdio.h>
#include <math.h>
#include "globals.h"

#include <Windows.h>
#include <WinDef.h>
#include <process.h>
//#include <thread>

using namespace std;
extern bool threadStarted = 0;
CursorSpeed cursor_speed;
HANDLE mouseThread;

Mouse::Mouse() {

}

Mouse::~Mouse() {

}
//this method was an experiment
//is not used in application
void Mouse::analogMouse(int x, int y) {

	if (x == 0 && y == 0)
		return;

	HDC hdc = GetDC(GetDesktopWindow()); //create handle for display
	int width = GetDeviceCaps(hdc, HORZRES); //get width in pixels
	int heihgt = GetDeviceCaps(hdc, VERTRES); //get height in pixles
	POINT pnt;
	GetCursorPos(&pnt); //get current curser position
	float delta = 0, alpha = 0;
	int dif = 0;

	if (x > 0) {
		delta = x * 1.5;
		while (delta > 1)
			delta /= 10;
		dif = width - pnt.x;
		delta *= dif;
		x = pnt.x + delta;
	}
	else if (x < 0) {
		delta = x * 1.5;
		while (abs(delta) > 1)
			delta /= 10;
		x = abs(pnt.x * delta);
	}

	if (y > 0) {
		delta = y * 1.5;
		while (delta > 1)
			delta /= 10;
		dif = heihgt - pnt.y;
		delta *= dif;
		y = pnt.y + delta;
	}
	else if (y < 0) {
		delta = y * 1.5;
		while (abs(delta) > 1)
			delta /= 10;
		y = abs(pnt.y * delta);
	}

	//printf("mouse x and y are: %d and %d\n", x, y);
	SetCursorPos(x, y);
}

void Mouse::checkCalibration(int width, int height) {


	if (::gaze.MAX_X > width) ::gaze.MAX_X--;
	else if (::missCount.X_MIN % 10 == 0) ::gaze.MIN_X--;
	else if (::missCount.Y_MAX % 10 == 0) ::gaze.MAX_Y++;
	else if (::missCount.Y_MIN % 10 == 0) ::gaze.MIN_Y--;
}

void Mouse::directMapMouse(int x, int y) {

	HDC hdc = GetDC(GetDesktopWindow()); //create handle for display
	int width = GetDeviceCaps(hdc, HORZRES); //get width in pixels
	int height = GetDeviceCaps(hdc, VERTRES); //get height in pixles

	//checkCalibration(width, height);
	//create ratio for mapping mouse from camera to monitor
	double xRatio = ((double)x - gaze.MIN_X) / ((double)gaze.MAX_X - gaze.MIN_X);
	double yRatio = ((double)y - gaze.MIN_Y) / ((double)gaze.MAX_Y - gaze.MIN_Y);
	int mouseX = width * xRatio;
	int mouseY = height * yRatio;

	//set new gaze points
	prevCursorPos = cursorPos;
	cursorPos.X = mouseX;
	cursorPos.Y = mouseY;

	//find distance between previous and current gaze point
	int xDistance = abs(prevCursorPos.X - cursorPos.X);
	int yDistance = abs(prevCursorPos.Y - cursorPos.Y);
	//set speed (at which the mouse moves) to new points
	//(velocity is proportional to distance)
	::cursor_speed.X = xDistance / (double) width;
	::cursor_speed.Y = yDistance / (double) height;
	//if only one gaze point (only happs on first iteration)
	if (prevCursorPos.X == -1) { 
		SetCursorPos(mouseX, mouseY);
	}
	else {
		//SetCursorPos(prevCursorPos.X, prevCursorPos.Y);
		//accelerate(width, height);

		if (::threadStarted == FALSE) { //if no other thread has been started
			::threadStarted == TRUE;
			//std::thread mouseThread(accelerate, width, height);
			//start thread
			HANDLE hndl = (HANDLE)_beginthreadex(0, 0, &accelerate, 0, 0, 0);
			
			//WaitForSingleObject(hndl, INFINITE);
		}
	}
}
//separate thread for moving mouse towards current gaze point
unsigned int __stdcall accelerate(void* data) {

	int x = prevCursorPos.X;
	int	y = prevCursorPos.Y;
	bool flag = false;
	while (1) {
		//wait for 100 ml seconds between mouse movements
		Sleep(100);
		//Mouse m; //in progress
		//m.sleepcp(100, x, y);
		if (x < cursorPos.X) {
			//x += ::cursor_speed.X;			
			x += (cursorPos.X - x) * ::cursor_speed.X;
			if (x >= cursorPos.X) flag = true;
		}
		else {
			//x -= ::cursor_speed.X;
			x -= abs(cursorPos.X - x) * ::cursor_speed.X;
			if (x <= cursorPos.X) flag = true;
		}

		if (y < cursorPos.Y) {
			//y += ::cursor_speed.Y;			
			y += (cursorPos.Y - y) * ::cursor_speed.Y;
			if (y >= cursorPos.Y) flag = true;
		}
		else {
			//y -= ::cursor_speed.flag = true		
			y -= abs(cursorPos.Y - y) * ::cursor_speed.Y;
			if (y <= cursorPos.Y) flag = true;
		}

		//for debuging
		SetCursorPos(x, y);
		int c = cv::waitKey(10);
		if ((char)c == 'c') { break; }
		if (flag) break;
	}
	//set threadStarted back to FALSE on exit
	return ::threadStarted = FALSE;
}
//this method slowly moves the mouse for a set time interval
//(still needs work, can cause divide by 0)
//void Mouse::sleepcp(int milliseconds, int x, int y) {
//
//	clock_t time_end;
//	time_end = clock() + milliseconds * CLOCKS_PER_SEC / 1000;
//
//	int xDistance = abs(x - cursorPos.X);
//	int yDistance = abs(y - cursorPos.Y);
//	int bigger = x > y ? x : y;
//	int smaller = x > y ? x : y;
//	int ratio = smaller / bigger;
//
//	while (clock() < time_end) {
//		if (x < cursorPos.X) {
//			x++;
//		}
//		else {
//			x--;
//		}
//
//		if (y < cursorPos.Y) {
//			y++;
//		}
//		else {
//			y--;
//		}
//		SetCursorPos(x, y);
//	}
//
//}