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

#include "stdafx.h"
#include "MinHashIndex.h"

#include "ImageData.h"
#include "MinHashUtility.h"
#include "Log.h"


using namespace codemm::cbsf::core;

namespace codemm {
namespace cbsf {
namespace minhash {


MinHashIndex::MinHashIndex(MinHashUtility& minHashUtility, int numSketches, int minHits /*= 1*/)
	: minHashUtility(minHashUtility), repository(numSketches)
{
	this->minHits = minHits;
}


MinHashIndex::MinHashIndex(MinHashUtility& minHashUtility, int numSketches, int minHits, int buckets)
	: minHashUtility(minHashUtility), repository(numSketches, buckets)
{
	this->minHits = minHits;
}


MinHashIndex::~MinHashIndex(void)
{
}


void MinHashIndex::Add (ImageData* pImageData)
{
	assert (pImageData);
	if (pImageData->GetSketches().size() == 0)
	{
		if (!pImageData->LoadSketches ())
		{
			this->minHashUtility.CalculateMinHash (*pImageData);
			pImageData->SaveSketches ();
			std::stringstream ss;
			ss << "Min-hash for image '" << pImageData->GetName() << "' computed!";
			logging::Log().write (ss.str());
		}
	}
	this->repository.Add (pImageData->GetSketches(), pImageData);
}


void MinHashIndex::Find (ImageData& queryImage, std::multimap<float, ImageData*>& result)
{
	queryImage.LoadSketches ();

	std::map<ImageData*, int> resultSketchHits;
	this->repository.Find (queryImage.GetSketches(), this->minHits, resultSketchHits);

	// rank results based on their estimated similarity using all min-Hash values
	for (std::map<ImageData*, int>::iterator it = resultSketchHits.begin(); it != resultSketchHits.end(); ++it)
	{
		float similarity = minhash::MinHashing<int>::Similarity (queryImage.GetSketches(), it->first->GetSketches());
		result.insert (std::multimap<float, ImageData*>::value_type(similarity, it->first));
	}
}


} // namespace minhash
} // namespace cbsf
} // namespace codemm
