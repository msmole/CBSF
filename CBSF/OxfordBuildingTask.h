// OxfordBuildingTask.h ---------------------------------------------//
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

#ifndef CBSF_OXFORDBUILDINGTASK_H_
#define CBSF_OXFORDBUILDINGTASK_H_

#include <set>
#include "ITaskItem.h"

namespace codemm {
namespace cbsf {

class Configuration;

namespace tasks {

struct OxfordQuery
{
public:
	std::string ID;
	std::string FileName;
	int X;
	int Y;
	int Width;
	int Height;
};


class OxfordBuildingTask : public ITaskItem
{
public:
	OxfordBuildingTask(void);
	virtual ~OxfordBuildingTask(void);

	float GetMeanAP () const;
	bool Execute (Configuration& configuration);
	
	static OxfordQuery CreateOxfordQuery (const fs::path& query);
	static void LoadQueries (const fs::path& path, std::vector<OxfordQuery>& queries);

private:
	float meanAP;
};


// the following evaluation code is from http://www.robots.ox.ac.uk/~vgg/data/oxbuildings/compute_ap.cpp

std::vector<std::string>
load_list(const std::string& fname);

template<class T>
std::set<T> vector_to_set(const std::vector<T>& vec)
{ return set<T>(vec.begin(), vec.end()); }

float
compute_ap(const std::set<std::string>& pos, const std::set<std::string>& amb, const std::vector<std::string>& ranked_list);

} // namespace tasks
} // namespace cbsf
} // namespace codemm

#endif // CBSF_OXFORDBUILDINGTASK_H_
