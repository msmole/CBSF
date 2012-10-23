// FSContext.cpp ---------------------------------------------//
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
#include "FSContext.h"

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

using namespace std;
using namespace cv;

namespace codemm {
namespace cbsf {
namespace core {


FSContext::FSContext(fs::path fileName)
{
	this->fileName = fileName;
}


FSContext::~FSContext(void)
{
}


const fs::path FSContext::GetFileName () const
{
	return this->fileName;
}


string FSContext::GetContextName () const
{
	return this->fileName.filename().string();
}


fs::path FSContext::GetParentPath () const
{
	return this->fileName.parent_path();
}


Mat* FSContext::LoadImage () const
{
	Mat* pImage = new Mat(imread (this->fileName.string()));
	return pImage;
}


IContext* FSContext::Clone () const
{
	FSContext* pNewContext = new FSContext (*this);
	return pNewContext;
}


} // namespace core
} // namespace cbsf
} // namespace codemm
