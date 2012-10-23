// BasicScreenFrameExtractor.h ---------------------------------------------//
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

#ifndef CBSF_BASICSCREENFRAMEEXTRACTOR_H_
#define CBSF_BASICSCREENFRAMEEXTRACTOR_H_


#include "IImageLoader.h"
#include "Property.h"

namespace codemm {
namespace cbsf {
namespace loader {

class BasicScreenFrameExtractor : public loader::IImageLoader
{
public:
	BasicScreenFrameExtractor(const fs::path& videoRepository);
	virtual ~BasicScreenFrameExtractor(void);

	virtual void GetImages (std::vector<core::ImageData*>& images);
	virtual bool HasNextImage ();
	virtual core::ImageData* GetNextImage ();
	virtual int GetNumberImages ();

	core::Property <std::string> VideoFileExtension;
	core::Property <std::string> DataFileExtension;

private:
	bool ExtractKeyFrames (const fs::path& videoFile, const std::vector<long>& keyFramePositions, std::vector<core::ImageData*>& keyFrames) const;
	void ReadSegmentPositions (const fs::path& file, std::vector<long>& segmentPositions) const;
	bool GetKeyFrames (const fs::path& videoFile, const std::string& segmentFileExtension, std::vector<core::ImageData*>& keyFrames) const;

	fs::path videoRepository;
	std::vector<fs::path> videoFiles;
	std::vector<core::ImageData*> tmpVideoImages;
	unsigned int videoFilePosition;
};


} // namespace loader
} // namespace cbsf
} // namespace codemm

#endif // CBSF_BASICSCREENFRAMEEXTRACTOR_H_
