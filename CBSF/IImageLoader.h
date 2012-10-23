// IImageLoader.h ---------------------------------------------//
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

#ifndef CBSF_IIMAGELOADER_H_
#define CBSF_IIMAGELOADER_H_


namespace codemm {
namespace cbsf {

namespace core { class ImageData; }

namespace loader {


class IImageLoader
{
public:
	virtual ~IImageLoader() {}
	virtual void GetImages (std::vector<core::ImageData*>& images) = 0;
	virtual bool HasNextImage () = 0;
	virtual core::ImageData* GetNextImage () = 0;
	virtual int GetNumberImages () = 0;
};

} // namespace loader
} // namespace cbsf
} // namespace codemm

#endif // CBSF_IIMAGELOADER_H_
