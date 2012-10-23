// MinHashCore.h ---------------------------------------------//
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

#ifndef CBSF_MINHASHCORE_H_
#define CBSF_MINHASHCORE_H_


namespace codemm {
namespace cbsf {
namespace minhash {


typedef std::vector<float> HashFunction;

// a sketch contains the positions of the min-hash for every hash function in that sketch
typedef std::vector<int> Sketch;



} // namespace minhash
} // namespace cbsf
} // namespace codemm

#endif // CBSF_MINHASHCORE_H_