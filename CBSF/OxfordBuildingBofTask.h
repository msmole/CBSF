// OxfordBuildingBofTask.h ---------------------------------------------//
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

#ifndef CBSF_OXFORDBUILDINGBOFTASK_H_
#define CBSF_OXFORDBUILDINGBOFTASK_H_


#include "ITaskItem.h"

namespace codemm {
namespace cbsf {

class Configuration;

namespace tasks {

struct OxfordQuery;


class OxfordBuildingBofTask : public ITaskItem
{
public:
	OxfordBuildingBofTask(void);
	virtual ~OxfordBuildingBofTask(void);

	float GetMeanAP () const;
	bool Execute (Configuration& configuration);
	
private:
	float meanAP;
};


} // namespace tasks
} // namespace cbsf
} // namespace codemm

#endif // CBSF_OXFORDBUILDINGBOFTASK_H_
