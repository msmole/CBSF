// RandomImageLoader.cpp ---------------------------------------------//
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
#include "RandomImageLoader.h"

#include "boost\random.hpp"


using namespace std;
using namespace codemm::cbsf::core;

namespace codemm {
namespace cbsf {
namespace loader {


RandomImageLoader::RandomImageLoader(IImageLoader* pImageLoader, unsigned int numberImages)
{
	assert (pImageLoader);
	this->pImageLoader = pImageLoader;
	this->numberImages = numberImages;
	this->imageCount = 0;
	this->loaderCount = 0;
}


RandomImageLoader::~RandomImageLoader(void)
{
}


void RandomImageLoader::GetImages (vector<ImageData*>& images)
{
	while (this->HasNextImage ())
	{
		images.push_back (this->GetNextImage ());
	}	
}


bool RandomImageLoader::HasNextImage ()
{
	return this->imageCount < this->numberImages && this->pImageLoader->HasNextImage ();
}


ImageData* RandomImageLoader::GetNextImage ()
{
	if (this->imageMap.empty())
	{
		boost::mt19937 generator((uint32_t)time(0));
		boost::uniform_int<> uniDist;
		boost::variate_generator<boost::mt19937&, boost::uniform_int<>> randomNumber(generator, uniDist);

		// initialize image map
		// the image map defines what images will be returned by GetNextImage
		int divisor = this->pImageLoader->GetNumberImages() / this->GetNumberImages();
		int diff = this->pImageLoader->GetNumberImages() % divisor;
		for (int i = 0; i < this->pImageLoader->GetNumberImages(); i++)
		{
			this->imageMap.push_back(i < diff || i % divisor == 0);
		}
		random_shuffle (this->imageMap.begin(), this->imageMap.end(), randomNumber);
	}

	ImageData* pImageData = NULL;
	while (!pImageData)
	{
		pImageData = this->pImageLoader->GetNextImage ();
		if (!this->imageMap[this->loaderCount])
		{
			pImageData = NULL;
		}
		this->loaderCount ++;
	}
	this->imageCount ++;
	return pImageData;
}


int RandomImageLoader::GetNumberImages ()
{
	return this->pImageLoader->GetNumberImages() < this->numberImages ? this->pImageLoader->GetNumberImages() : this->numberImages;
}


} // namespace loader
} // namespace cbsf
} // namespace codemm
