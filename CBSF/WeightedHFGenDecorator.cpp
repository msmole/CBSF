// WeightedHFGenDecorator.cpp ---------------------------------------------//
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
#include "WeightedHFGenDecorator.h"

#include <math.h>


using namespace std;

namespace codemm {
namespace cbsf {
namespace minhash {


WeightedHFGenDecorator::WeightedHFGenDecorator(IHashFunctionGenerator* pGenerator, const vector<float>& weights)
	: HFGenDecorator (pGenerator), weights (weights)
{
}


WeightedHFGenDecorator::~WeightedHFGenDecorator(void)
{
}


void WeightedHFGenDecorator::Generate (int numFunctions, int numDimensions, vector<HashFunction>& hashFunctions)
{
	HFGenDecorator::Generate (numFunctions, numDimensions, hashFunctions);

	assert (hashFunctions.size() == numFunctions);
	assert (this->weights.size() == numDimensions);

	for (vector<HashFunction>::iterator it = hashFunctions.begin(); it != hashFunctions.end(); ++it)
	{
		assert (it->size() == numDimensions);
		int i = 0;
		for (vector<float>::const_iterator itw = this->weights.begin(); itw != this->weights.end(); ++i, ++itw)
		{
			(*it)[i] = log((*it)[i]) * (-1) / *itw;
		}
	}
}


} // namespace minhash
} // namespace cbsf
} // namespace codemm
