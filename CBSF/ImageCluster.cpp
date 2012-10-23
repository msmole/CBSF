// ImageCluster.cpp ---------------------------------------------//
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
#include "ImageCluster.h"

#include <set>
#include <opencv2\core\core.hpp>

#include "ImageData.h"


using namespace std;

namespace codemm {
namespace cbsf {
namespace clustering {

using namespace codemm::cbsf::core;
	
int ImageCluster::idCounter = 0;
int ImageCluster::MinClusterSize = 10;


ImageCluster::ImageCluster(const string& id, core::ImageData* initialImage, int minClusterSize)
	: id(id), minClusterSize(minClusterSize)
{
	this->pLeft = NULL;
	this->pRight = NULL;
	this->images.push_back (initialImage);
}


ImageCluster::~ImageCluster(void)
{
	if (this->pLeft)
	{
		delete this->pLeft;
	}
	if (this->pRight)
	{
		delete this->pRight;
	}

	for (vector<core::ImageData*>::iterator it = this->images.begin(); it != this->images.end(); ++it)
	{
		delete (*it);
	}
}


ImageCluster* ImageCluster::Create (ImageData* initialImage, int minClusterSize/* = ImageCluster::MinClusterSize*/)
{
	assert (initialImage);
	return new ImageCluster (initialImage->GetName(), initialImage, minClusterSize);
}


void ImageCluster::Merge (ImageCluster* other)
{
	assert (other);

	if (this->images.size() < this->minClusterSize && other->images.size() < this->minClusterSize
		&& !other->GetLeftTree() && !other->GetRightTree() && !this->GetLeftTree() && !this->GetRightTree())
	{
		for (vector<core::ImageData*>::iterator it = other->images.begin(); it != other->images.end(); ++it)
		{
			this->images.push_back (*it);
		}
		other->images.clear ();
		delete other;
	}
	else
	{
		ImageCluster* pNewLeft = new ImageCluster (*this);
		this->pLeft = pNewLeft;
		this->pRight = other;
		this->images.clear ();
	}
}


const string& ImageCluster::GetId () const
{
	return this->id;
}


const vector<core::ImageData*> ImageCluster::GetImages () const
{
	return this->images;
}


const ImageCluster* ImageCluster::GetLeftTree () const
{
	return this->pLeft;
}


const ImageCluster* ImageCluster::GetRightTree () const
{
	return this->pRight;
}


bool ImageCluster::operator< (const ImageCluster& other) const
{
	return this->GetId() < other.GetId();
}


} // namespace clustering
} // namespace cbsf
} // namespace codemm
