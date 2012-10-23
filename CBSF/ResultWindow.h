// ResultWindow.h ---------------------------------------------//
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

#ifndef CBSF_RESULTWINDOW_H_
#define CBSF_RESULTWINDOW_H_


#include "ImageWindow.h"

namespace codemm {
namespace cbsf {

class Configuration;

namespace ui {


class ResultWindow : ImageWindow
{
public:
	ResultWindow(std::string ID, Configuration& configuration);
	virtual ~ResultWindow(void);

	void ShowResult(cv::Mat queryImage, std::vector<std::string> images);
	
	int MaxWidth;
	int MaxHeight;
	int BorderSize;
	int NumColumns;
	int NumMatches;
	bool DrawKeypoints;
	bool DrawMatches;

private:
	cv::Mat LoadImage (cv::Mat queryImage, const std::string& path);

	Configuration& configuration;
};


} // namespace ui
} // namespace cbsf
} // namespace codemm

#endif // CBSF_RESULTWINDOW_H_
