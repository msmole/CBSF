// AbstractIndex.cpp ---------------------------------------------//
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
#include "AbstractIndex.h"

#include "IImageLoader.h"
#include "ImageData.h"


namespace codemm {
namespace cbsf {
namespace core {


void AbstractIndex::Init (loader::IImageLoader& loader)
{
	while (loader.HasNextImage())
	{
		core::ImageData* pImageData = loader.GetNextImage ();
		this->Add (pImageData);
	}
}


} // namespace core
} // namespace cbsf
} // namespace codemm
