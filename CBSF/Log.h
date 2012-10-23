// Log.h ---------------------------------------------//
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

#ifndef CBSF_LOG_H_
#define CBSF_LOG_H_


namespace codemm {
namespace cbsf {
namespace logging {


class _Log
{
public:
	_Log ();
	virtual ~_Log();

	void init (const std::string& path);
	_Log& operator<< (const std::string& msg);
	inline void write (const std::string& msg);

private:

	std::ofstream* pOS;
};

_Log& Log ();

} // namespace logging
} // namespace cbsf
} // namespace codemm

#endif // CBSF_LOG_H_
