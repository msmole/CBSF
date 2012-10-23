// ImageData.cpp ---------------------------------------------//
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
#include "ImageData.h"

#include <opencv2\core\core.hpp>

#include "IContext.h"
#include "FileSystemUtility.h"


using namespace std;
using namespace cv;
using namespace codemm::cbsf::minhash;

namespace codemm {
namespace cbsf {
namespace core {


ImageData::ImageData(string name)
{
	this->name = name;
	this->pImageContext = NULL;
	this->pHashContext = NULL;
	this->pImage = NULL;
	this->useROI = false;
}


ImageData::~ImageData(void)
{
	if (this->pImageContext)
	{
		delete this->pImageContext;
	}
	if (this->pHashContext)
	{
		delete this->pHashContext;
	}
	if (this->pImage)
	{
		delete this->pImage;
	}
}


const string& ImageData::GetName() const
{
	return this->name;
}


const Mat& ImageData::GetImage ()
{
	if (!this->pImage && this->pImageContext)
	{
		this->pImage = this->pImageContext->LoadImage ();
	}
	assert (this->pImage);

	if (this->useROI)
	{
		return this->imageROI;
	}
	return *(this->pImage);
}


void ImageData::SetImage (const Mat& pImage)
{
	if (this->pImage)
	{
		delete this->pImage;
	}
	this->pImage = new Mat (pImage);
}


void ImageData::SetROI (const Rect& roi)
{
	this->useROI = false;
	this->imageROI = this->GetImage()(roi);
	this->useROI = true;
}


void ImageData::ResetROI ()
{
	this->useROI = false;
}


const Mat& ImageData::GetDescriptor () const
{
	return this->descriptor;
}


Mat& ImageData::GetDescriptor ()
{
	return this->descriptor;
}


bool ImageData::SaveDescriptor () const
{
	if (!fs::exists(this->GetDescriptorPath().parent_path()))
	{
		fs::create_directories(this->GetDescriptorPath().parent_path());
	}

	FileSystemUtility::Save1DMat (this->GetDescriptorPath(), this->descriptor);
	return true;
}


bool ImageData::LoadDescriptor ()
{
	return FileSystemUtility::Load1DMat (this->GetDescriptorPath(), this->descriptor);
}


int ImageData::GetNumVisualWords () const
{
	float numVisualWords = 0.0f;
	for (int i = 0; i < this->descriptor.cols; i++)
	{
		const float& elem = this->descriptor.at<float>(0, i);
		numVisualWords += elem;
	}
	return (int) floor (numVisualWords + 0.5f);
}


vector<Sketch>& ImageData::GetSketches ()
{
	return this->sketches;
}


const vector<Sketch>& ImageData::GetSketches () const
{
	return this->sketches;
}


void ImageData::SetImageContext (IContext* pContext)
{
	assert (pContext);
	this->pImageContext = pContext;
}


IContext* ImageData::GetImageContext ()
{
	return this->pImageContext;
}


void ImageData::SetHashContext (IContext* pContext)
{
	assert (pContext);
	this->pHashContext = pContext;
}


IContext* ImageData::GetHashContext ()
{
	return this->pHashContext;
}


void ImageData::SaveSketches () const
{
	FileSystemUtility::SaveMatrix (this->GetHashPath(), this->sketches);
}


bool ImageData::LoadSketches ()
{
	if (this->sketches.size() == 0)
	{
		return FileSystemUtility::LoadMatrix (this->GetHashPath(), this->sketches);
	}
	return true;
}


fs::path ImageData::GetHashPath () const
{
	fs::path path;
	if (this->pHashContext)
	{
		path = this->pHashContext->GetParentPath() / this->pHashContext->GetContextName();
	}
	else
	{
		stringstream ss;
		ss << this->pImageContext->GetContextName() << ".mh";
		path = this->pImageContext->GetParentPath() / ss.str();
	}
	return path;
}


fs::path ImageData::GetDescriptorPath () const
{
	stringstream filename;
	filename << this->GetName() << ".bd";
	if (this->pHashContext)
	{
		return this->GetHashPath().parent_path().parent_path().parent_path() / "desc" / filename.str();
	}
	else
	{
		return this->pImageContext->GetParentPath() / filename.str();
	}
}


void ImageData::ReleaseImage ()
{
	if (this->pImage)
	{
		delete this->pImage;
		this->pImage = NULL;
	}
}


ImageData* ImageData::Clone () const
{
	ImageData* pNewImageData = new ImageData (*this);
	pNewImageData->pImageContext = this->pImageContext->Clone();
	pNewImageData->pHashContext = this->pHashContext->Clone();
	pNewImageData->pImage = NULL;
	return pNewImageData;
}


} // namespace core
} // namespace cbsf
} // namespace codemm
