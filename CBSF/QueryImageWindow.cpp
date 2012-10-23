// QueryImageWindow.cpp ---------------------------------------------//
/*
 * Copyright (c) 2012, Martin Smole. All rights reserved.
 *
 * This file is part of the CBSF.
 *
 * The CBSF is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 3 of the License, or (at your option) any later version.
 *
 * The CBSF is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with the CBSF. If not, see <http://www.gnu.org/licenses/>.
 */

#include "stdafx.h"
#include "QueryImageWindow.h"

#include <opencv2\highgui\highgui.hpp>


using namespace std;
using namespace cv;

namespace codemm {
namespace cbsf {
namespace ui {


QueryImageWindow::QueryImageWindow(string ID)
	: ImageWindow(ID)
{
	this->pRect = NULL;
	this->mouseActive = false;
	this->rectVisible = false;
}


QueryImageWindow::~QueryImageWindow(void)
{
	if (this->pRect)
	{
		delete this->pRect;
		this->pRect = NULL;
	}
}


void QueryImageWindow::ShowQuery(Mat image)
{
	this->Show (image);
	setMouseCallback(this->GetID(), QueryImageWindow::MouseCallbackHandler, this);
	while (waitKey () != 13);
	this->Hide();
}


Rect* QueryImageWindow::GetSelection ()
{
	return this->pRect;
}


void QueryImageWindow::MouseCallback(int event, int x, int y, int flags)
{
	if (event == EVENT_LBUTTONDOWN)
	{
		this->mouseActive = true;
		this->startPoint.x = x;
		this->startPoint.y = y;
	}
	else if (event == EVENT_LBUTTONUP)
	{
		mouseActive = false;
		if (this->startPoint.x < x && this->startPoint.y < y)
		{
			this->pRect = new Rect (this->startPoint, Point (x, y));
		}
		else if (this->pRect)
		{
			delete this->pRect;
			this->pRect = NULL;
		}
	}
	else if (mouseActive && event == EVENT_MOUSEMOVE)
	{
		if (this->startPoint.x < x && this->startPoint.y < y)
		{
			Mat draw = this->GetImage().clone();
			rectangle (draw, this->startPoint, Point(x, y), Scalar(0, 255, 255), 2);
			imshow(this->GetID(), draw);
			this->rectVisible = true;
		}
		else if (this->rectVisible)
		{
			imshow(this->GetID(), this->GetImage());
		}
	}
}


void QueryImageWindow::MouseCallbackHandler(int event, int x, int y, int flags, void* param)
{
	QueryImageWindow* pQueryImageWindow = (QueryImageWindow*)param;
	pQueryImageWindow->MouseCallback (event, x, y, flags);
}


} // namespace ui
} // namespace cbsf
} // namespace codemm
