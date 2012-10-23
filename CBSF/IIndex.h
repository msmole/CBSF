// IIndex.h ---------------------------------------------//
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

#ifndef CBSF_IINDEX_H_
#define CBSF_IINDEX_H_


#include <map>

namespace codemm {
namespace cbsf {

namespace loader { class IImageLoader; }

namespace core {


class ImageData;

class IIndex
{
public:
	virtual void Init (loader::IImageLoader& loader) = 0;
	virtual void Add (ImageData* pImageData) = 0;
	virtual void Find (core::ImageData& queryImage, std::multimap<float, core::ImageData*>& result) = 0;
};

} // namespace core
} // namespace cbsf
} // namespace codemm

#endif // CBSF_IINDEX_H_
