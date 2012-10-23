// Repository.h ---------------------------------------------//
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

#ifndef CBSF_REPOSITORY_H_
#define CBSF_REPOSITORY_H_

#include <boost\unordered_map.hpp>
#include "MinHashCore.h"


namespace codemm {
namespace cbsf {

namespace loader { class IImageLoader; }

namespace minhash {


std::size_t hash_value(const Sketch& sketch);


template <class T>
class Repository
{
	typedef boost::unordered_multimap<Sketch, T*> Hashmap;

public:
	Repository(int numSketches, int buckets = boost::unordered::detail::default_bucket_count);
	virtual ~Repository(void);

	void Add (const std::vector<Sketch>& sketch, T* object);
	void Find (const std::vector<Sketch>& sketch, int numHits, std::map<T*, int>& result) const;

private:
	int numSketches;
	std::vector<Hashmap> hashmaps;
	std::vector<T*> deleteVec;	// contains all objects to easily delete them in the destructor
};


template <class T>
Repository<T>::Repository(int numSketches, int buckets/*=boost::unordered::detail::default_bucket_count*/)
{
	assert (numSketches > 0);
	this->numSketches = numSketches;

	// initializing the hashmaps
	for (int i = 0; i < this->numSketches; ++i)
	{
		hashmaps.push_back (Hashmap(buckets));
	}
}


template <class T>
Repository<T>::~Repository(void)
{
	for (std::vector<T*>::iterator it = this->deleteVec.begin(); it != deleteVec.end(); ++it)
	{
		delete (*it);
	}
}


template <class T>
void Repository<T>::Add(const std::vector<Sketch>& sketchKey, T* object)
{
	assert (sketchKey.size() == numSketches);
	assert (object);

	this->deleteVec.push_back(object);

	std::vector<Hashmap>::iterator hashIt = this->hashmaps.begin();
	for (std::vector<Sketch>::const_iterator it = sketchKey.cbegin(); it != sketchKey.cend(); ++it, ++hashIt)
	{
		hashIt->insert(Hashmap::value_type(*it, object));
	}
}


template <class T>
void Repository<T>::Find(const std::vector<Sketch>& query, int numHits, std::map<T*, int>& result) const
{
	assert (query.size() == numSketches);
	
	// loop through every single sketch of the query and ask the corresponding hash table if it contains entries
	std::vector<Sketch>::const_iterator queryIt = query.cbegin();
	for (std::vector<Hashmap>::const_iterator it = this->hashmaps.begin(); it != this->hashmaps.end(); ++it, ++queryIt)
	{
		// get the range of equal sketches from the hash table
		std::pair<Hashmap::const_iterator, Hashmap::const_iterator> range = it->equal_range (*queryIt);
		
		// add one sketch hit for every match found
		for (; range.first != range.second; ++range.first)
		{
			result[range.first->second] ++;
		}
	}
	
	// after this step we have a result map that contain objects that have at least one sketch hit
	if (numHits > 1)
	{
		// if we are checking for more than one sketch hit, we need to remove entries from the result set that have less then "numHits" sketch hits.
		// NOTE: this operation could be expensive if the result set contains a huge amount of objects
		for (std::map<T*, int>::iterator it = result.begin(); it != result.end();)
		{
			if (it->second < numHits)
			{
				std::map<T*, int>::iterator eraseIt = it;
				++it;
				result.erase (eraseIt);
			}
			else
			{
				++it;
			}
		}
	}
}


} // namespace minhash
} // namespace cbsf
} // namespace codemm

#endif // CBSF_REPOSITORY_H_
