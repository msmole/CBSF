// ImageCluster.h ---------------------------------------------//
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

#ifndef CBSF_IMAGECLUSTER_H_
#define CBSF_IMAGECLUSTER_H_


namespace codemm {
namespace cbsf {

namespace core { class ImageData; }

namespace clustering {

using namespace codemm::cbsf::core;

class ImageCluster
{
public:
	static ImageCluster* Create (ImageData* initialImage, int minClusterSize = ImageCluster::MinClusterSize);
	void Merge (ImageCluster* other);
	const std::string& GetId () const;
	const std::vector<core::ImageData*> GetImages () const;
	const ImageCluster* GetLeftTree () const;
	const ImageCluster* GetRightTree () const;

	bool operator< (const ImageCluster& other) const;
	~ImageCluster(void);

	static int MinClusterSize;

private:
	ImageCluster (const std::string& id, core::ImageData* initialImage, int minClusterSize);

	static int idCounter;
	std::string id;
	std::vector<core::ImageData*> images;
	int minClusterSize;
	ImageCluster* pLeft;
	ImageCluster* pRight;
};


} // namespace clustering
} // namespace cbsf
} // namespace codemm

#endif // CBSF_IMAGECLUSTER_H_
