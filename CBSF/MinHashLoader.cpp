// MinHashLoader.cpp ---------------------------------------------//
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
#include "MinHashLoader.h"

#include <set>
#include <opencv2\core\core.hpp>

#include "ImageData.h"
#include "FileSystemUtility.h"
#include "FSContext.h"


using namespace std;
using namespace codemm::cbsf::core;

namespace codemm {
namespace cbsf {
namespace loader {


MinHashLoader::MinHashLoader(const fs::path& repository)
{
	this->hashRepository = repository;
	this->imageRepository = repository;
	this->ImageFileExtension = ".mh";
	this->imageFilePosition = 0;
}


MinHashLoader::MinHashLoader(const fs::path& hashRepository, const fs::path& imageRepository)
{
	this->hashRepository = hashRepository;
	this->imageRepository = imageRepository;
	this->ImageFileExtension = ".mh";
	this->imageFilePosition = 0;
}


MinHashLoader::~MinHashLoader(void)
{
}


void MinHashLoader::GetImages (vector<ImageData*>& images)
{
	if (this->imageFiles.size() == 0)
	{
		FileSystemUtility::GetFiles (this->hashRepository, this->ImageFileExtension, this->imageFiles);
		this->imageFilePosition = 0;
	}

	while (this->HasNextImage ())
	{
		images.push_back (this->GetNextImage ());
	}	
}


bool MinHashLoader::HasNextImage ()
{
	if (this->imageFiles.size() == 0)
	{
		FileSystemUtility::GetFiles (this->hashRepository, this->ImageFileExtension, this->imageFiles);
		this->imageFilePosition = 0;
	}

	return (this->imageFilePosition) < this->imageFiles.size ();
}


ImageData* MinHashLoader::GetNextImage ()
{
	if (this->imageFiles.size() == 0)
	{
		FileSystemUtility::GetFiles (this->hashRepository, this->ImageFileExtension, this->imageFiles);
		this->imageFilePosition = 0;
	}

	if (this->imageFilePosition < this->imageFiles.size ())
	{
		fs::path hashFilePath = this->imageFiles.at(this->imageFilePosition);
		fs::path imageFilePath = hashFilePath;
		imageFilePath.replace_extension();
		string imageFileName = imageFilePath.filename().string();
		imageFilePath = this->imageRepository / imageFileName;
		ImageData* pImageData = new ImageData (imageFileName);
		pImageData->SetImageContext (new FSContext (imageFilePath));
		pImageData->SetHashContext (new FSContext (hashFilePath));
		pImageData->LoadSketches ();
	
		this->imageFilePosition ++;
		return pImageData;
	}

	return NULL;
}


int MinHashLoader::GetNumberImages ()
{
	if (this->imageFiles.size() == 0)
	{
		FileSystemUtility::GetFiles (this->hashRepository, this->ImageFileExtension, this->imageFiles);
		this->imageFilePosition = 0;
	}
	return (int) this->imageFiles.size();
}


void MinHashLoader::Reset ()
{
	this->imageFilePosition = 0;
}


} // namespace loader
} // namespace cbsf
} // namespace codemm
