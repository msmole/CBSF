// RandomImageLoader.h ---------------------------------------------//
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

#ifndef CBSF_RANDOMIMAGELOADER_H_
#define CBSF_RANDOMIMAGELOADER_H_


#include "IImageLoader.h"

namespace codemm {
namespace cbsf {
namespace loader {


class RandomImageLoader : public loader::IImageLoader
{
public:
	RandomImageLoader(IImageLoader* pImageLoader, unsigned int numberImages);
	virtual ~RandomImageLoader(void);

	virtual void GetImages (std::vector<core::ImageData*>& images);
	virtual bool HasNextImage ();
	virtual core::ImageData* GetNextImage ();
	virtual int GetNumberImages ();

private:
	IImageLoader* pImageLoader;
	int numberImages;
	int imageCount;
	int loaderCount;
	std::vector<bool> imageMap;
};


} // namespace loader
} // namespace cbsf
} // namespace codemm

#endif // CBSF_RANDOMIMAGELOADER_H_
