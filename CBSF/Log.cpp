// Log.cpp ---------------------------------------------//
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
#include "Log.h"

#include <boost/date_time/posix_time/posix_time.hpp>


using namespace std;

namespace codemm {
namespace cbsf {
namespace logging {

_Log& Log ()
{
	static _Log log;
	return log;
}

_Log::_Log ()
{
	this->pOS = NULL;
}


_Log::~_Log()
{
	if (this->pOS)
	{
		delete this->pOS;
		this->pOS = NULL;
	}
}


void _Log::init (const string& path)
{
	this->pOS = new ofstream (path);
}

_Log& _Log::operator<< (const string& msg)
{
	this->write (msg);
	return *this;
}

inline void _Log::write (const string& msg)
{
	assert (this->pOS);
	boost::posix_time::ptime now = boost::posix_time::microsec_clock::local_time();
	(*pOS) << to_iso_extended_string(now) << " | " << msg << endl;
	cout << to_iso_extended_string(now) << " | " << msg << endl;
}


} // namespace logging
} // namespace cbsf
} // namespace codemm
