// HistogramMinHashing.cpp ---------------------------------------------//
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
#include "HistogramMinHashing.h"

#include <set>
#include <opencv2\core\core.hpp>


using namespace std;
using namespace codemm::cbsf::minhash;


HistogramMinHashing::HistogramMinHashing(int numSketches, int sketchSize, int dimensions, int accuracy)
	: MinHashing<int> (numSketches, sketchSize)
{
	this->accuracy = accuracy;

	set<int> universalSet;
	for (int i = 1; i <= dimensions * accuracy; i++)
	{
		universalSet.insert (i);
	}
	this->Init (universalSet);
}


HistogramMinHashing::~HistogramMinHashing(void)
{
}


void HistogramMinHashing::CalculateMinHash (const vector<float>& hist, vector<Sketch>& minHash)
{
	assert (hist.size() == this->GetNumDimensions() / this->accuracy);

	// build set
	set<int> tmpSet;
	int setElem = 1;
	for (vector<float>::const_iterator it = hist.begin(); it != hist.end(); ++it)
	{
		// need to round to the closest int. numbers like 0.99999999 could be present in the histogram and should be treated like 1.
		int slots = (int) floor((*it) + 0.5);
		for (int i = 1; i <= this->accuracy; ++i, ++setElem)
		{
			if (i <= slots)
			{
				tmpSet.insert(setElem);
			}
		}
	}

	MinHashing<int>::CalculateMinHash (tmpSet, minHash);
}
