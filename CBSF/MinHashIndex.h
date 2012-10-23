// MinHashIndex.h ---------------------------------------------//
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

#ifndef CBSF_MINHASHINDEX_H_
#define CBSF_MINHASHINDEX_H_


#include "AbstractIndex.h"
#include "Repository.h"

namespace codemm {
namespace cbsf {

namespace core { class ImageData; }

namespace minhash {


class MinHashUtility;

class MinHashIndex : public core::AbstractIndex
{
public:
	MinHashIndex(MinHashUtility& minHashUtility, int numSketches, int minHits = 1);
	MinHashIndex(MinHashUtility& minHashUtility, int numSketches, int minHits, int buckets);
	virtual ~MinHashIndex(void);

	virtual void Add (core::ImageData* pImageData);
	virtual void Find (core::ImageData& queryImage, std::multimap<float, core::ImageData*>& result);

private:
	int minHits;
	Repository<core::ImageData> repository;
	MinHashUtility& minHashUtility;
};

} // namespace minhash
} // namespace cbsf
} // namespace codemm

#endif // CBSF_MINHASHINDEX_H_
