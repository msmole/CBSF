// QueryImageWindow.h ---------------------------------------------//
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

#ifndef CBSF_QUERYIMAGEWINDOW_H_
#define CBSF_QUERYIMAGEWINDOW_H_


#include "ImageWindow.h"

namespace codemm {
namespace cbsf {
namespace ui {


class QueryImageWindow : ImageWindow
{
public:
	QueryImageWindow(std::string ID);
	virtual ~QueryImageWindow(void);

	void ShowQuery(cv::Mat image);
	cv::Rect* GetSelection ();
	
private:
	void MouseCallback(int event, int x, int y, int flags);
	static void MouseCallbackHandler(int event, int x, int y, int flags, void* param);

	bool mouseActive;
	bool rectVisible;
	cv::Point startPoint;
	cv::Rect* pRect;
};


} // namespace ui
} // namespace cbsf
} // namespace codemm

#endif // CBSF_QUERYIMAGEWINDOW_H_
