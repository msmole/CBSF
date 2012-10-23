// ListImageLoader.cpp ---------------------------------------------//
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
#include "ListImageLoader.h"

#include <set>
#include <opencv2\core\core.hpp>

#include "ImageData.h"
#include "FSContext.h"


using namespace std;
using namespace codemm::cbsf::core;

namespace codemm {
namespace cbsf {
namespace loader {

ListImageLoader::ListImageLoader(const std::vector<fs::path>& imageList, const fs::path& hashRepository/* = fs::path()*/)
	: imageList (imageList), hashRepository (hashRepository)
{
	this->imageFilePosition = 0;
}


ListImageLoader::~ListImageLoader(void)
{
}


void ListImageLoader::GetImages (vector<ImageData*>& images)
{
	while (this->HasNextImage ())
	{
		images.push_back (this->GetNextImage ());
	}	
}


bool ListImageLoader::HasNextImage ()
{
	return this->imageFilePosition < this->imageList.size ();
}


ImageData* ListImageLoader::GetNextImage ()
{
	ImageData* pImageData = NULL;
	if (this->HasNextImage ())
	{
		const fs::path& path = this->imageList.at(this->imageFilePosition);
		pImageData = new ImageData (path.filename().string());
		pImageData->SetImageContext (new FSContext (path));
		if (!this->hashRepository.empty())
		{
			stringstream ss;
			ss << path.filename().string() << ".mh";
			fs::path hashPath = this->hashRepository / ss.str();
			pImageData->SetHashContext (new FSContext (hashPath));
		}

		this->imageFilePosition ++;
	}
	return pImageData;
}


int ListImageLoader::GetNumberImages ()
{
	return this->imageList.size ();
}


bool ListImageLoader::LoadImageList (const fs::path& imageListPath, vector<fs::path>& imageList)
{
	if (fs::exists (imageListPath))
	{
		ifstream ifs (imageListPath.string());
		string line;
		while (ifs.good ())
		{
			getline (ifs, line);
			if (!line.empty())
			{
				imageList.push_back (fs::path(line));
			}
		}
		return true;
	}
	return false;
}



} // namespace loader
} // namespace cbsf
} // namespace codemm
