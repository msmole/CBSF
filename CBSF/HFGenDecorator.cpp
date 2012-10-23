// HFGenDecorator.cpp ---------------------------------------------//
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
#include "HFGenDecorator.h"

namespace codemm {
namespace cbsf {
namespace minhash {

using namespace std;

HFGenDecorator::HFGenDecorator (IHashFunctionGenerator* pGenerator)
{
	assert (this->pGenerator);
	this->pGenerator = pGenerator;
}


HFGenDecorator::~HFGenDecorator ()
{
	if (this->pGenerator)
	{
		delete this->pGenerator;
	}
}


void HFGenDecorator::Generate (int numFunctions, int numDimensions, vector<HashFunction>& hashFunctions)
{
	this->pGenerator->Generate (numFunctions, numDimensions, hashFunctions);
}


} // namespace minhash
} // namespace cbsf
} // namespace codemm
