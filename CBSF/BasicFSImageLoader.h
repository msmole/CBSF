// BasicFSImageLoader.h ---------------------------------------------//
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

#ifndef CBSF_BASICFSIMAGELOADER_H_
#define CBSF_BASICFSIMAGELOADER_H_


#include "IImageLoader.h"
#include "Property.h"

namespace codemm {
namespace cbsf {

namespace core { class ImageData; }
	
namespace loader {


class BasicFSImageLoader : public IImageLoader
{
public:
	BasicFSImageLoader(const fs::path& imageRepository, const fs::path& hashRepository = fs::path());
	virtual ~BasicFSImageLoader(void);

	virtual void GetImages (std::vector<core::ImageData*>& images);
	virtual bool HasNextImage ();
	virtual core::ImageData* GetNextImage ();
	virtual int GetNumberImages ();

	void Reset ();

	core::Property <std::string> ImageFileExtension;

protected:
	fs::path imageRepository;
	fs::path hashRepository;
	std::vector<fs::path> imageFiles;
	unsigned int imageFilePosition;
};


} // namespace loader
} // namespace cbsf
} // namespace codemm

#endif // CBSF_BASICFSIMAGELOADER_H_
