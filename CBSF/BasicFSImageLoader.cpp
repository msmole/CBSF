// BasicFSImageLoader.cpp ---------------------------------------------//
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
#include "BasicFSImageLoader.h"

#include <set>
#include <map>
#include <opencv2\core\core.hpp>

#include "ImageData.h"
#include "FileSystemUtility.h"
#include "FSContext.h"


using namespace std;
using namespace codemm::cbsf::core;

namespace codemm {
namespace cbsf {
namespace loader {


BasicFSImageLoader::BasicFSImageLoader(const fs::path& imageRepository, const fs::path& hashRepository)
{
	this->imageRepository = imageRepository;
	this->hashRepository = hashRepository;
	this->ImageFileExtension = ".jpg";
	this->imageFilePosition = 0;
}


BasicFSImageLoader::~BasicFSImageLoader(void)
{
}


void BasicFSImageLoader::GetImages (vector<ImageData*>& images)
{
	if (this->imageFiles.size() == 0)
	{
		FileSystemUtility::GetFiles (this->imageRepository, this->ImageFileExtension, this->imageFiles);
		this->imageFilePosition = 0;
	}

	while (this->HasNextImage ())
	{
		images.push_back (this->GetNextImage ());
	}	
}


bool BasicFSImageLoader::HasNextImage ()
{
	if (this->imageFiles.size() == 0)
	{
		FileSystemUtility::GetFiles (this->imageRepository, this->ImageFileExtension, this->imageFiles);
		this->imageFilePosition = 0;
	}

	return (this->imageFilePosition) < this->imageFiles.size ();
}


ImageData* BasicFSImageLoader::GetNextImage ()
{
	if (this->HasNextImage ())
	{
		const fs::path& path = this->imageFiles.at(this->imageFilePosition);
		ImageData* pImageData = new ImageData (path.filename().string());
		pImageData->SetImageContext (new FSContext (path));
		if (!this->hashRepository.empty())
		{
			stringstream ss;
			ss << path.filename().string() << ".mh";
			fs::path hashPath = this->hashRepository / ss.str();
			pImageData->SetHashContext (new FSContext (hashPath));
		}

		this->imageFilePosition ++;
		return pImageData;
	}

	return NULL;
}


int BasicFSImageLoader::GetNumberImages ()
{
	if (this->imageFiles.size() == 0)
	{
		FileSystemUtility::GetFiles (this->imageRepository, this->ImageFileExtension, this->imageFiles);
		this->imageFilePosition = 0;
	}
	return (int) this->imageFiles.size();
}


void BasicFSImageLoader::Reset ()
{
	this->imageFilePosition = 0;
}


} // namespace loader
} // namespace cbsf
} // namespace codemm
