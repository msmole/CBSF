// ListImageLoader.h ---------------------------------------------//
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

#ifndef CBSF_LISTIMAGELOADER_H_
#define CBSF_LISTIMAGELOADER_H_


#include "IImageLoader.h"

namespace codemm {
namespace cbsf {
namespace loader {


class ListImageLoader : public loader::IImageLoader
{
public:
	ListImageLoader(const fs::path& imageListPath, const fs::path& hashRepository = fs::path());
	ListImageLoader(const std::vector<fs::path>& imageList, const fs::path& hashRepository = fs::path());
	virtual ~ListImageLoader(void);

	virtual void GetImages (std::vector<core::ImageData*>& images);
	virtual bool HasNextImage ();
	virtual core::ImageData* GetNextImage ();
	virtual int GetNumberImages ();

	static bool LoadImageList (const fs::path& imageListPath, std::vector<fs::path>& imageList);

private:
	const fs::path& hashRepository;
	const std::vector<fs::path>& imageList;
	unsigned int imageFilePosition;
};


} // namespace loader
} // namespace cbsf
} // namespace codemm

#endif // CBSF_LISTIMAGELOADER_H_
