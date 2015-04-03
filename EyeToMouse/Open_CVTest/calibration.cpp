/*
Create by Thomas Fallwell 2/28/2015
For calibration of gaze points
*/
#include "calibration.h"

#include <iostream>
#include <queue>
#include <stdio.h>
#include <math.h>

#include <Windows.h>
#include <WinDef.h>
#include <process.h>
#include <time.h>
#include "findEyeCenter.h"
#include "globals.h"
#include "main.h"
//Gobal variables
Calibration cal;
Gaze gaze;
int number_of_columns = 6;
int number_of_rows = 3;

Calibration::Calibration() {
	
}

Calibration::~Calibration() {

}

void Calibration::checkMiss(int x, int y) {

	if (x > ::gaze.MAX_X) ::missCount.X_MAX++;
	if (x < ::gaze.MIN_X) ::missCount.X_MIN++;
	if (y > ::gaze.MAX_Y) ::missCount.Y_MAX++;
	if (y < ::gaze.MAX_Y) ::missCount.Y_MAX++;

	if ((::missCount.X_MAX % 10) == 0) {
		::gaze.MAX_X++;
		::gaze.MIN_X++;
	}
	if ((::missCount.X_MIN % 10) == 0) {
		::gaze.MIN_X--;
		::gaze.MAX_X--;
	}
	if ((::missCount.Y_MAX % 10) == 0) {
		::gaze.MAX_Y++;
		::gaze.MIN_Y++;
	}
	if ((::missCount.Y_MIN % 10) == 0) {
		::gaze.MIN_Y--;
		::gaze.MAX_Y--;
	}
}

void Calibration::sleepcp(int milliseconds, char* direction) {

	clock_t time_end;
	time_end = clock() + milliseconds * CLOCKS_PER_SEC / 1000;
	int i = 0, sum = 0;

	//while waiting for next calibration point to be drawn
	//capture summate gaze points
	while (clock() < time_end) {
		sum += captureFace(direction);
		i++;
	}
	if (i == 0) i = 1; //don't divide by 0
	//average where each min and max value are
	if (direction == "left") {
		::gaze.MIN_X = sum / i;
	}
	else if (direction == "top") {
		::gaze.MAX_Y = sum / i;
	}
	else if (direction == "right") {
		::gaze.MAX_X = sum / i;
	}
	else if (direction == "bottom") {
		::gaze.MIN_Y = sum / i;
	}
}

int* Calibration::checkCalibration(int min, int max, int range) {

	while ((max - min) < range) {
		max++;
		if ((max - min) < range)
			min--;
	}

	while ((max - min) > range) {
		max--;
		if ((max - min) > range)
			min++;
	}
	int arr[] = { min, max };
	return arr;
	////min can't be greater than max
	//int temp;
	//if (::gaze.MIN_X > ::gaze.MAX_X) {
	//	temp = ::gaze.MIN_X;
	//	::gaze.MIN_X = ::gaze.MAX_X;
	//	::gaze.MAX_X = temp;
	//}

	//if (::gaze.MIN_Y > ::gaze.MAX_Y) {
	//	temp = ::gaze.MIN_Y;
	//	::gaze.MIN_Y = ::gaze.MAX_Y;
	//	::gaze.MAX_Y = temp;
	//}

	//while ((::gaze.MAX_X - ::gaze.MIN_X) < ::number_of_columns) {
	//	::gaze.MAX_X++;
	//	::gaze.MIN_X--;
	//}

	//while ((::gaze.MAX_X - ::gaze.MIN_X) > ::number_of_columns) {
	//	::gaze.MAX_X--;
	//	::gaze.MIN_X++;
	//}

	//while ((::gaze.MAX_Y - ::gaze.MIN_Y) < ::number_of_rows)
	//	::gaze.MAX_Y++;

	//while ((::gaze.MAX_Y - ::gaze.MIN_Y) > ::number_of_rows)
	//	::gaze.MAX_Y--;
}

void Calibration::calibrate() {

	HDC hdc = GetDC(GetDesktopWindow()); //create handle for display
	int width = GetDeviceCaps(hdc, HORZRES); //get width in pixels
	int heihgt = GetDeviceCaps(hdc, VERTRES); //get height in pixles

	//draw calibration point and wait
	BOOL left = Ellipse(hdc, 0, (heihgt / 2) + 20, 40, (heihgt / 2) - 20);	//draw 40 px circle on left side of screen
	sleepcp(4000, "left");
	//top
	BOOL top = Ellipse(hdc, (width / 2) - 20, heihgt, (width / 2) + 20, heihgt - 40);
	sleepcp(4000, "top");
	//draw 40 px circle on right side of screen
	BOOL right = Ellipse(hdc, width - 40, (heihgt / 2) + 20, width, (heihgt / 2) - 20);
	sleepcp(4000, "right");
	//bottom
	BOOL bottom = Ellipse(hdc, (width / 2) - 20, 0, (width / 2) + 20, 40);
	sleepcp(4000, "bottom");

	//adjust calibration if points are way off
	int *x = checkCalibration(::gaze.MIN_X, ::gaze.MAX_X, ::number_of_columns);
	::gaze.MIN_X = *(x++);
	::gaze.MAX_X = *x;

	int *y = checkCalibration(::gaze.MIN_Y, ::gaze.MAX_Y, ::number_of_rows);
	::gaze.MIN_Y = *(y++);
	::gaze.MAX_Y = *y;
}