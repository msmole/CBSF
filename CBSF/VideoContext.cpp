// VideoContext.cpp ---------------------------------------------//
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
#include "VideoContext.h"

#include <opencv2/highgui/highgui.hpp>

#include "Log.h"


using namespace std;
using namespace cv;

namespace codemm {
namespace cbsf {
namespace core {


VideoContext::VideoContext(const fs::path& videoFile, long frameNumber)
{
	this->videoFile = videoFile;
	this->frameNumber = frameNumber;
}


VideoContext::~VideoContext(void)
{
}


const fs::path& VideoContext::GetVideoFile () const
{
	return this->videoFile;
}


long VideoContext::GetFrameNumber () const
{
	return this->frameNumber;
}


string VideoContext::GetContextName () const
{
	stringstream ss;
	ss << this->videoFile.filename().string() << "-" << this->frameNumber;
	return ss.str();
}


fs::path VideoContext::GetParentPath () const
{
	return this->videoFile.parent_path();
}


Mat* VideoContext::LoadImage () const
{
	VideoCapture capture (videoFile.string());
	if (!capture.isOpened ())
	{
		stringstream ss;
		ss << "Could not open video '" << videoFile.string() << "'!";
		logging::Log().write (ss.str());
		return false;
	}
	
	long frameCount = static_cast<long> (capture.get (CV_CAP_PROP_FRAME_COUNT));
	
	if (this->frameNumber >= frameCount)
	{
		logging::Log().write ("Key frame position exceeds frame count!");
		return false;
	}
	capture.set (CV_CAP_PROP_POS_FRAMES, this->frameNumber);

	Mat* pImage = new Mat();
	capture.read (*pImage);
	return pImage;
}


IContext* VideoContext::Clone () const
{
	VideoContext* pNewContext = new VideoContext (*this);
	return pNewContext;
}


} // namespace core
} // namespace cbsf
} // namespace codemm
