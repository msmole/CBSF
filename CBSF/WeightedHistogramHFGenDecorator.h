// WeightedHistogramHFGenDecorator.h ---------------------------------------------//
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

#ifndef CBSF_WEIGHTEDHISTOGRAMHFGENDECORATOR_H_
#define CBSF_WEIGHTEDHISTOGRAMHFGENDECORATOR_H_


#include "HFGenDecorator.h"

namespace codemm {
namespace cbsf {
namespace minhash {


class WeightedHistogramHFGenDecorator : public HFGenDecorator
{
public:
	WeightedHistogramHFGenDecorator(IHashFunctionGenerator* pGenerator, const std::vector<float>& weights, int accuracy);
	virtual ~WeightedHistogramHFGenDecorator(void);

	virtual void Generate (int numFunctions, int numDimensions, std::vector<HashFunction>& hashFunctions);

private:
	int accuracy;
	const std::vector<float>& weights;
};


} // namespace minhash
} // namespace cbsf
} // namespace codemm


#endif // CBSF_WEIGHTEDHISTOGRAMHFGENDECORATOR_H_
