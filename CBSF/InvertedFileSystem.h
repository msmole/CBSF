// InvertedFileSystem.h ---------------------------------------------//
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

#ifndef CBSF_INVERTEDFILESYSTEM_H_
#define CBSF_INVERTEDFILESYSTEM_H_

#include <set>
#include "AbstractIndex.h"

namespace codemm {
namespace cbsf {

namespace core { class ImageData; }
namespace loader { class IImageLoader; }

namespace bof {

class BagOfFeatures;

class InvertedFileSystem : public core::AbstractIndex
{
public:
	InvertedFileSystem(BagOfFeatures& bagOfFeatures, const std::vector<float>& weights, bool isBagSimilarity = true);
	virtual ~InvertedFileSystem(void);

	virtual void Add (core::ImageData* pImageData);
	virtual void Find (core::ImageData& queryImage, std::multimap<float, core::ImageData*>& result);

private:
	void NormalizeL2 (cv::Mat& descriptor) const;

	BagOfFeatures& bagOfFeatures;
	bool isBagSimilarity;
	const std::vector<float>& weights;
	std::vector<core::ImageData*> images; // contains all objects to easily delete them in the destructor
	std::vector<std::vector<std::pair<core::ImageData*,float>>> invertedFile;
};


} // namespace bof
} // namespace cbsf
} // namespace codemm

#endif // CBSF_INVERTEDFILESYSTEM_H_
