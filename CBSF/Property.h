// Property.h ---------------------------------------------//
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

#ifndef CBSF_PROPERTY_H_
#define CBSF_PROPERTY_H_

namespace codemm {
namespace cbsf {
namespace core {


template <class T>
class DefaultPredicate
{
public:
	static inline bool CheckSetter (T value)
	{
		return true;
	}
	static inline void CheckGetter (T value)
	{
	}
};


template <class T, class Predicate = DefaultPredicate <T>>
class Property
{
public:
	inline operator T const & () const
	{
		Predicate::CheckGetter (m_storage);
		return m_storage;
	}
	inline Property <T, Predicate>& operator = (T rhs)
	{
		if (Predicate::CheckSetter (rhs))
		{
			m_storage = rhs;
		}
		return *this;
	}

private:
	T m_storage;
};


template <class T, class Predicate = DefaultPredicate <T>>
class ReferenceProperty
{
public:
	inline operator T & ()
	{
		Predicate::CheckGetter (m_storage);
		return m_storage;
	}
	inline Property <T, Predicate>& operator = (T rhs)
	{
		if (Predicate::CheckSetter (rhs))
		{
			m_storage = rhs;
		}
		return *this;
	}

private:
	T m_storage;
};


} // namespace core
} // namespace cbsf
} // namespace codemm

#endif // CBSF_PROPERTY_H_
