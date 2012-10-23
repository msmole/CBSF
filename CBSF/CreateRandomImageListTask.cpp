// CreateRandomImageListTask.cpp ---------------------------------------------//
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
#include "CreateRandomImageListTask.h"

#include "Configuration.h"
#include "BasicFSImageLoader.h"
#include "RandomImageLoader.h"
#include "ImageData.h"


using namespace std;

namespace codemm {
namespace cbsf {
namespace tasks {


CreateRandomImageListTask::CreateRandomImageListTask(void)
{
}


CreateRandomImageListTask::~CreateRandomImageListTask(void)
{
}


void CreateRandomImageListTask::SetImageListName (const string& imageListName)
{
	this->imageListName = imageListName;
}


const vector<fs::path>& CreateRandomImageListTask::GetImageFileList () const
{
	return this->imageFileList;
}


bool CreateRandomImageListTask::Execute (Configuration& configuration)
{
	using loader::BasicFSImageLoader;
	using loader::RandomImageLoader;
	using core::ImageData;

	BasicFSImageLoader imageLoader (configuration.Source);
	RandomImageLoader randomLoader (&imageLoader, configuration.TrainingSetSize);

	while (randomLoader.HasNextImage ())
	{
		ImageData* pImageData = randomLoader.GetNextImage ();
		this->imageFileList.push_back (pImageData->GetImageContext()->GetParentPath() / pImageData->GetImageContext()->GetContextName());
	}

	ofstream ofs ((configuration.Target / imageListName).string());
	for (vector<fs::path>::iterator it = imageFileList.begin(); it != imageFileList.end (); ++it)
	{
		ofs << it->string() << endl;
	}
	ofs.close ();

	return true;
}


} // namespace tasks
} // namespace cbsf
} // namespace codemm
