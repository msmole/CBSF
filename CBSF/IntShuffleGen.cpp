// IntShuffleGen.cpp ---------------------------------------------//
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
#include "IntShuffleGen.h"


using namespace std;

namespace codemm {
namespace cbsf {
namespace minhash {


IntShuffleGen::IntShuffleGen(void)
{
}


IntShuffleGen::~IntShuffleGen(void)
{
}


void IntShuffleGen::Generate (int numFunctions, int numDimensions, vector<HashFunction>& hashFunctions)
{
	hashFunctions.clear ();

	HashFunction hashFunction;
	for (int i = 0; i < numDimensions; i++)
	{
		hashFunction.push_back ((float) (i + 1));
	}

	for (int i = 0; i < numFunctions; i++)
	{
		random_shuffle (hashFunction.begin(), hashFunction.end());
		hashFunctions.push_back (HashFunction(hashFunction));
	}
}


} // namespace minhash
} // namespace cbsf
} // namespace codemm