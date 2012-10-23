// ImageWindow.cpp ---------------------------------------------//
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
#include "ImageWindow.h"

#include <opencv2\core\core.hpp>
#include <opencv2\highgui\highgui.hpp>


using namespace std;
using namespace cv;

namespace codemm {
namespace cbsf {
namespace ui {


ImageWindow::ImageWindow(string ID)
{
	this->windowID = ID;
	this->windowActive = false;
}


ImageWindow::~ImageWindow(void)
{
	this->Hide();
}


void ImageWindow::Show(Mat image)
{
	this->image = image;
	namedWindow (this->windowID);
	imshow(this->windowID, this->image);
	this->windowActive = true;
}


void ImageWindow::Hide()
{
	if (this->windowActive)
	{
		destroyWindow(this->windowID);
	}
}


const string& ImageWindow::GetID()
{
	return this->windowID;
}


Mat ImageWindow::GetImage()
{
	return this->image;
}


} // namespace ui
} // namespace cbsf
} // namespace codemm
