// MinHashing.h ---------------------------------------------//
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

#ifndef CBSF_MINHASHING_H_
#define CBSF_MINHASHING_H_


#include <set>

#include "FileSystemUtility.h"
#include "RandomUniformGen.h"

namespace codemm {
namespace cbsf {
namespace minhash {

class IHashFunctionGenerator;

template <class T>
class MinHashing
{
public:
	MinHashing(int numSketches, int sketchSize);
	virtual ~MinHashing(void);

	void Init (const std::set<T>& universalSet);

	void SetHashFunctionGenerator (IHashFunctionGenerator* pHFGen);

	void GenerateHashFunctions ();
	void SaveHashFunctions (const fs::path& path);
	bool LoadHashFunctions (const fs::path& path);
	int GetNumDimensions () const;
	int GetNumSketches () const;
	int GetSketchSize () const;

	void CalculateMinHash (const std::set<T>& t, std::vector<Sketch>& minHash);
	static float Similarity (const std::vector<Sketch>& s1, const std::vector<Sketch>& s2);
	static int IdenticalSketches (const std::vector<Sketch>& s1, const std::vector<Sketch>& s2);

private:
	IHashFunctionGenerator* pHashFunctionGenerator;
	std::vector<HashFunction> hashFunctions;
	std::map<T, int> tablePositionMap;
	bool initialized;
	bool hashFunctionsAvailable;
	int numSketches;
	int numDimensions;
	int sketchSize;
};



template <class T>
MinHashing<T>::MinHashing(int numSketches, int sketchSize)
{
	assert (numSketches > 0);
	assert (sketchSize > 0);

	this->initialized = false;
	this->hashFunctionsAvailable = false;
	this->numSketches = numSketches;
	this->sketchSize = sketchSize;

	this->pHashFunctionGenerator = new RandomUniformGen ();
}


template <class T>
MinHashing<T>::~MinHashing(void)
{
	if (this->pHashFunctionGenerator)
	{
		delete this->pHashFunctionGenerator;
	}
}


// Usually, the created tablePositionMap must be stored with the hash functions.
// As the stl set internally is stored as a biniary search tree there is no need to
// store the tablePositionMap with the hash functions. This method will always
// result in the same tablePositionMap because the set is "ordered".
template <class T>
void MinHashing<T>::Init (const std::set<T>& universalSet)
{
	this->numDimensions = (int) universalSet.size();

	int i = 0;
	for (std::set<T>::iterator it = universalSet.begin(); it != universalSet.end(); ++it, ++i)
	{
		this->tablePositionMap[*it] = i;		
	}

	this->initialized = true;
}


template <class T>
void MinHashing<T>::SetHashFunctionGenerator (IHashFunctionGenerator* pHFGen)
{
	assert (pHFGen);
	if (this->pHashFunctionGenerator)
	{
		delete this->pHashFunctionGenerator;
	}
	this->pHashFunctionGenerator = pHFGen;
}


template <class T>
void MinHashing<T>::GenerateHashFunctions ()
{
	assert (this->initialized);
	this->pHashFunctionGenerator->Generate (this->numSketches * this->sketchSize, this->numDimensions, this->hashFunctions);
	this->hashFunctionsAvailable = true;
}


template <class T>
void MinHashing<T>::SaveHashFunctions (const fs::path& path)
{
	assert (this->hashFunctionsAvailable);
	core::FileSystemUtility::SaveMatrix (path, this->hashFunctions);
}


template <class T>
bool MinHashing<T>::LoadHashFunctions (const fs::path& path)
{
	assert (this->initialized);
	if (core::FileSystemUtility::LoadMatrix (path, this->hashFunctions))
	{
		this->hashFunctionsAvailable = true;
		return true;
	}
	return false;
}


template <class T>
void MinHashing<T>::CalculateMinHash (const std::set<T>& t, std::vector<Sketch>& sketches)
{
	assert (this->hashFunctionsAvailable);
	sketches.clear ();

	Sketch sketch;
	for (unsigned int hashFunctionIndex = 0; hashFunctionIndex < this->hashFunctions.size(); ++hashFunctionIndex)
	{
		HashFunction currentHashFunction = this->hashFunctions.at (hashFunctionIndex);
		
		// calculate the min hash for the given set t
		int minHashPos = INT_MAX;
		double minHashValue = DBL_MAX;
		for (std::set<T>::const_iterator it = t.begin(); it != t.end(); ++it)
		{
			map<T, int>::const_iterator tableEntry = this->tablePositionMap.find (*it);
			if (tableEntry != this->tablePositionMap.end())
			{
				 double value = currentHashFunction.at (tableEntry->second);
				 if (value < minHashValue)
				 {
					 minHashValue = value;
					 minHashPos = tableEntry->second;
				 }
			}
		}

		// put the min hash pos to the sketches
		sketch.push_back(minHashPos);
		if (hashFunctionIndex % this->sketchSize == this->sketchSize - 1)
		{
			sketches.push_back (sketch);
			sketch.clear ();
		}
	}
}


template <class T>
int MinHashing<T>::GetNumDimensions () const
{
	return this->numDimensions;
}


template <class T>
int MinHashing<T>::GetNumSketches () const
{
	return this->numSketches;
}


template <class T>
int MinHashing<T>::GetSketchSize () const
{
	return this->sketchSize;
}


template <class T>
float MinHashing<T>::Similarity (const std::vector<Sketch>& s1, const std::vector<Sketch>& s2)
{
	assert (s1.size() == s2.size());

	int hits = 0;
	int count = 0;
	std::vector<Sketch>::const_iterator it1, it2;
	for(it1 = s1.cbegin(), it2 = s2.cbegin(); it1 != s1.cend(); ++it1, ++it2)
	{
		assert (it1->size() == it2->size());
		
		Sketch::const_iterator sit1, sit2;
		for (sit1 = it1->cbegin(), sit2 = it2->cbegin(); sit1 != it1->cend(); ++sit1, ++sit2)
		{
			if (*sit1 == *sit2)
			{
				hits ++;
			}
			count ++;
		}
	}

	return ((float) hits) / ((float) count);
}


template <class T>
int MinHashing<T>::IdenticalSketches (const std::vector<Sketch>& s1, const std::vector<Sketch>& s2)
{
	size_t size = s1.size ();
	if (s1.size() != s2.size())
	{
		assert (s1.size() == 0 || s2.size() == 0);
		size = 0;
	}
	int numIdentical = 0;
	for (size_t i = 0; i < size; i++)
	{
		if (s1.at(i) == s2.at(i))
		{
			numIdentical ++;
		}
	}
	return numIdentical;
}


} // namespace minhash
} // namespace cbsf
} // namespace codemm

#endif // CBSF_MINHASHING_H_
