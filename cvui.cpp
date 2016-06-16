/*
 A (very) simple UI lib built on top of OpenCV drawing primitives.

 Copyright (c) 2016 Fernando Bevilacqua <dovyski@gmail.com>
 Licensed under the MIT license.
*/

#include <iostream>
#include "opencv2/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "cvui.h"

namespace cvui
{

// This is an internal namespace with all functions
// that actually render each one of the UI components
namespace render {
	const int IDLE = 0;
	const int OVER = 1;
	const int PRESSED = 2;

	void button(int theState, cv::Mat& theWhere, cv::Rect& theShape) {
		// Outline
		cv::rectangle(theWhere, theShape, cv::Scalar(0x33, 0x33, 0x33));

		// Border
		theShape.x++; theShape.y++; theShape.width -= 2; theShape.height -= 2;
		cv::rectangle(theWhere, theShape, cv::Scalar(0x52, 0x52, 0x52));

		// Inside
		theShape.x++; theShape.y++; theShape.width -= 2; theShape.height -= 2;
		cv::rectangle(theWhere, theShape, cv::Scalar(0x42, 0x42, 0x42), cv::FILLED);		
	}
}

// Variables to keep track of mouse events and stuff
static bool mouseJustReleased = false;
static bool mousePressed = false;
static cv::Point mouse;
static char buffer[200];
	
void init(const cv::String& theWindowName) {
	cv::setMouseCallback(theWindowName, handleMouse, NULL);
}

bool button(cv::Mat& theWhere, int theX, int theY, const cv::String& theLabel) {
	cv::Rect aRect(theX, theY, theLabel.length() * 10, 20);
	bool aMouseIsOver = aRect.contains(mouse);

	if (aMouseIsOver) {
		if (mousePressed) {
			cv::rectangle(theWhere, aRect, cv::Scalar(0, 0, 255), cv::FILLED);
		} else {
			cv::rectangle(theWhere, aRect, cv::Scalar(255, 0, 0), cv::FILLED);
		}
	} else {
		render::button(render::IDLE, theWhere, aRect);
	}

	text(theWhere, theX + 5, theY + 10, theLabel, 0.4);

	return aMouseIsOver && mouseJustReleased;
}

bool checkbox(cv::Mat& theWhere, int theX, int theY, const cv::String& theLabel, bool *theState) {
	cv::Rect aRect(theX, theY, 20, 20);
	cv::Rect aHitArea(theX, theY, aRect.width + theLabel.length() * 8, 20);
	bool aMouseIsOver = aHitArea.contains(mouse);

	if (aMouseIsOver) {
		cv::rectangle(theWhere, aRect, cv::Scalar(255, 0, 0), cv::FILLED);

		if (mouseJustReleased) {
			*theState = !(*theState);
		}
	} else {
		cv::rectangle(theWhere, aRect, cv::Scalar(190, 0, 0), cv::FILLED);
	}

	if (*theState) {
		text(theWhere, theX + 5, theY + 15, "X", 0.5);
	}

	text(theWhere, theX + 25, theY + 15, theLabel, 0.4);

	return *theState;
}

void text(cv::Mat& theWhere, int theX, int theY, const cv::String& theText, double theFontScale) {
	cv::Point aPos(theX, theY);
	cv::putText(theWhere, theText, aPos, cv::FONT_HERSHEY_SIMPLEX, theFontScale, cv::Scalar(0xCE, 0xCE, 0xCE), 1, cv::LINE_AA);
}

int counter(cv::Mat& theWhere, int theX, int theY, int *theValue) {
	cv::Rect aContent(theX + 30, theY, 60, 20);

	if (cvui::button(theWhere, theX, theY, " - ")) {
		(*theValue)--;
	}
	
	sprintf_s(buffer, "%d", *theValue);
	cv::rectangle(theWhere, aContent, cv::Scalar(220, 220, 220), cv::FILLED);
	text(theWhere, aContent.x + 20, aContent.y + 15, buffer, 0.4);

	if (cvui::button(theWhere, aContent.x + aContent.width, theY, " + ")) {
		(*theValue)++;
	}

	return *theValue;
}

void overlay(cv::Mat& theWhere, int theX, int theY, int theWidth, int theHeight, const cv::String& theTitle) {
	double aAlpha = 0.3;
	cv::Rect aTitleBar(theX, theY, theWidth, 20);
	cv::Mat aOverlay;

	// Render the title
	cv::rectangle(theWhere, aTitleBar, cv::Scalar(80, 80, 80), cv::FILLED);
	text(theWhere, aTitleBar.x + 5, aTitleBar.y + 15, theTitle, 0.4);

	// Render the body
	theWhere.copyTo(aOverlay);
	cv::rectangle(aOverlay, cv::Rect(theX, theY, theWidth, theHeight), cv::Scalar(255, 0, 0, 0.5), cv::FILLED);
	cv::addWeighted(aOverlay, aAlpha, theWhere, 1.0 - aAlpha, 0.0, theWhere);
}

void update() {
	mouseJustReleased = false;
}

void handleMouse(int theEvent, int theX, int theY, int theFlags, void* theData) {
	mouse.x = theX;
	mouse.y = theY;

	if (theEvent == cv::EVENT_LBUTTONDOWN || theEvent == cv::EVENT_RBUTTONDOWN)	{
		mousePressed = true;

	} else if (theEvent == cv::EVENT_LBUTTONUP || theEvent == cv::EVENT_RBUTTONUP)	{
		mouseJustReleased = true;
		mousePressed = false;
	}
}

} // namespace cvui