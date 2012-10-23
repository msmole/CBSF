// MinHashUtility.h ---------------------------------------------//
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

#ifndef CBSF_MINHASHUTILITY_H_
#define CBSF_MINHASHUTILITY_H_


#include <set>

namespace codemm {
namespace cbsf {

namespace core { class ImageData; }
class Configuration;

namespace minhash {

template <class T>
class MinHashing;
class HistogramMinHashing;

class MinHashUtility
{
public:
	MinHashUtility(Configuration& configuration, const std::vector<float>& weights = std::vector<float>());
	virtual ~MinHashUtility(void);

	void Init ();
	void CalculateMinHash (core::ImageData& imageData);
	bool IsInitialized () const;

private:
	static void Mat2Vector (const cv::Mat& mat, std::vector<float>& vec);
	static void Mat2Set (const cv::Mat& mat, std::set<int>& wordSet);

	bool isInitialized;
	MinHashing<int>* pMinHash;
	HistogramMinHashing* pMinHashHist;
	Configuration& configuration;
	const std::vector<float>& weights;
};


} // namespace minhash
} // namespace cbsf
} // namespace codemm

#endif // CBSF_MINHASHUTILITY_H_
