// KeyFrameExtractionTask.cpp ---------------------------------------------//
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
#include "KeyFrameExtractionTask.h"

#include <opencv2\highgui\highgui.hpp>

#include "Configuration.h"
#include "VideoContext.h"
#include "ImageData.h"
#include "BasicScreenFrameExtractor.h"
#include "Log.h"


using namespace std;

namespace codemm {
namespace cbsf {
namespace tasks {


KeyFrameExtractionTask::KeyFrameExtractionTask(void)
{
}


KeyFrameExtractionTask::~KeyFrameExtractionTask(void)
{
}


bool KeyFrameExtractionTask::Execute (Configuration& configuration)
{
	logging::Log().write ("extract keyframes ...");

	stringstream ss;
	loader::BasicScreenFrameExtractor loader (configuration.Source);
	while (loader.HasNextImage ())
	{
		vector<cv::Mat> vecImages;
		for (int i = 0; loader.HasNextImage (); i++)
		{
			core::ImageData* pImageData = loader.GetNextImage();
			core::VideoContext* pContext = dynamic_cast<core::VideoContext*> (pImageData->GetImageContext());
			stringstream fileName;
			fileName << pContext->GetVideoFile().filename().string() << "." << pContext->GetFrameNumber() << ".jpg";
			imwrite ((pContext->GetParentPath() / fileName.str()).string(), pImageData->GetImage());

			ss.str("");
			ss << fileName.str() << " extracted!";
			logging::Log().write (ss.str());
			
			delete pImageData;
		}
	}

	return true;
}


} // namespace tasks
} // namespace cbsf
} // namespace codemm
