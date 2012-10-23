// FSContext.h ---------------------------------------------//
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

#ifndef CBSF_FSCONTEXT_H_
#define CBSF_FSCONTEXT_H_


#include "IContext.h"

namespace codemm {
namespace cbsf {
namespace core {


class FSContext : public IContext
{
public:
	FSContext(fs::path fileName);
	virtual ~FSContext(void);

	const fs::path GetFileName () const;
	virtual std::string GetContextName () const;
	virtual fs::path GetParentPath () const;
	virtual cv::Mat* LoadImage () const;
	virtual IContext* Clone () const;

private:
	fs::path fileName;
};

} // namespace core
} // namespace cbsf
} // namespace codemm

#endif // CBSF_FSCONTEXT_H_
