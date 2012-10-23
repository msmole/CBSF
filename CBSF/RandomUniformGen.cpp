// RandomUniformGen.cpp ---------------------------------------------//
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
#include "RandomUniformGen.h"

#include <ctime>
#include "boost\random.hpp"


using namespace std;

namespace codemm {
namespace cbsf {
namespace minhash {


RandomUniformGen::RandomUniformGen(void)
{
}


RandomUniformGen::~RandomUniformGen(void)
{
}


void RandomUniformGen::Generate (int numFunctions, int numDimensions, vector<HashFunction>& hashFunctions)
{
	hashFunctions.clear ();

	boost::mt19937 gen((uint32_t)time(0));
	boost::uniform_01<boost::mt19937> getRand(gen);

	for (int i = 0; i < numFunctions; i++)
	{
		HashFunction hashFunction;
		for (int j = 0; j < numDimensions; ++j)
		{
			hashFunction.push_back ((float)getRand());
		}
		hashFunctions.push_back(hashFunction);
	}
}


} // namespace minhash
} // namespace cbsf
} // namespace codemm
