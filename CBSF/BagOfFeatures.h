// BagOfFeatures.h ---------------------------------------------//
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

#ifndef CBSF_BAGOFFEATURES_H_
#define CBSF_BAGOFFEATURES_H_

#include <opencv2\features2d\features2d.hpp>

namespace codemm {
namespace cbsf {

namespace core { class ImageData; }
namespace loader { class IImageLoader; }
	
namespace bof {


class BagOfFeatures
{
public:
	BagOfFeatures(cv::Ptr<cv::FeatureDetector> ptrFeatureDetector, cv::Ptr<cv::DescriptorExtractor> ptrDescriptorExtractor, cv::Ptr<cv::DescriptorMatcher> ptrDescriptorMatcher);
	virtual ~BagOfFeatures(void);

	bool ReadVocabulary (const fs::path& filename);
	bool WriteVocabulary (const fs::path& filename);

	bool ReadWeights (const fs::path& filename);
	void WriteWeights (const fs::path& filename);

	bool ReadMaxWordFrequencies (const fs::path& filename);
	void WriteMaxWordFrequencies (const fs::path& filename);

	void ComputeVocabulary (loader::IImageLoader& imageLoader, int clusterCount, int attemps, int maxIterations, float epsilon);
	void ComputeWeights (loader::IImageLoader& imageLoader);

	void ComputeDescriptor (core::ImageData& imageData);
	void ComputeDescriptors (std::vector<core::ImageData*>& images);
	bool LoadOrComputeDescriptor (core::ImageData& imageData);
	
	bool IsVocabularyLoaded () const;
	int GetVocabularySize () const;
	int GetDescriptorDimension () const;
	const std::vector<float>& GetIdfWeights ();
	const std::vector<float>& GetMaxWordFrequencies () const;

	cv::FeatureDetector* GetFeatureDetector ();
	cv::DescriptorExtractor* GetDescriptorExtractor ();
	cv::DescriptorMatcher* GetDescriptorMatcher ();

	static float Similarity (const cv::Mat& d1, const cv::Mat& d2, const std::vector<float>& idfWeights);
	static float SetSimilarity (const cv::Mat& d1, const cv::Mat& d2, const std::vector<float>& idfWeights);

private:
	cv::BOWImgDescriptorExtractor& GetBowImgDE ();

	cv::Ptr<cv::FeatureDetector> ptrFeatureDetector;
	cv::Ptr<cv::DescriptorExtractor> ptrDescriptorExtractor;
	cv::Ptr<cv::DescriptorMatcher> ptrDescriptorMatcher;
	cv::Ptr<cv::BOWImgDescriptorExtractor> ptrBowImgDE;
	cv::Mat vocabulary;
	std::vector<float> idfWeights;
	std::vector<float> maxDescValues;
	bool vocabularyLoaded;
};


} // namespace bof
} // namespace cbsf
} // namespace codemm


#endif // CBSF_BAGOFFEATURES_H_