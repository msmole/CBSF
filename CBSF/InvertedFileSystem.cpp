// InvertedFileSystem.cpp ---------------------------------------------//
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
#include "InvertedFileSystem.h"

#include <opencv2/features2d/features2d.hpp>

#include "BagOfFeatures.h"
#include "IImageLoader.h"
#include "ImageData.h"
#include "Log.h"

using namespace std;

namespace codemm {
namespace cbsf {
namespace bof {

InvertedFileSystem::InvertedFileSystem(BagOfFeatures& bagOfFeatures, const std::vector<float>& weights, bool isBagSimilarity /*= true*/)
	: bagOfFeatures(bagOfFeatures), weights(weights)
{
	this->isBagSimilarity = isBagSimilarity;
	this->invertedFile.resize (this->bagOfFeatures.GetVocabularySize());
}


InvertedFileSystem::~InvertedFileSystem(void)
{
	for (vector<core::ImageData*>::iterator it = this->images.begin(); it != this->images.end(); ++it)
	{
		delete *it;
	}
}


void InvertedFileSystem::Add (core::ImageData* pImageData)
{
	if (!this->bagOfFeatures.LoadOrComputeDescriptor (*pImageData))
	{
		stringstream ss;
		ss << "Descriptor for image '" << pImageData->GetName() << "' computed!";
		logging::Log().write (ss.str());
		pImageData->ReleaseImage ();
	}

	// normalize the descriptor
	cv::Mat& descriptor = pImageData->GetDescriptor ();
	this->NormalizeL2 (descriptor);
	
	for (int i = 0; i < descriptor.cols; i++)
	{
		const float& wordFrequency = descriptor.at<float>(0, i);
		if (wordFrequency > 0)
		{
			this->invertedFile[i].push_back (pair<core::ImageData*,float>(pImageData, wordFrequency));
		}
	}
	this->images.push_back(pImageData);
	pImageData->GetDescriptor().release ();
}


void InvertedFileSystem::Find (core::ImageData& queryImage, std::multimap<float, core::ImageData*>& result)
{
	this->bagOfFeatures.LoadOrComputeDescriptor (queryImage);

	// normalize query descriptor (need to copy descriptor because we don't have
	// ownership of the query image and thus can not change the descriptor)
	cv::Mat queryDesc = queryImage.GetDescriptor().clone();
	this->NormalizeL2 (queryDesc);

	std::map<core::ImageData*, float> unrankedResult;
	for (int i = 0; i < queryDesc.cols; i++)
	{
		const float& wordFrequency = queryDesc.at<float>(0, i);
		if (wordFrequency > 0)
		{
			for (vector<pair<core::ImageData*,float>>::iterator it = this->invertedFile[i].begin(); it != this->invertedFile[i].end(); ++it)
			{
				unrankedResult[it->first] += wordFrequency * it->second;
			}
		}
	}

	// rank result
	for (std::map<core::ImageData*, float>::iterator it = unrankedResult.begin(); it != unrankedResult.end(); ++it)
	{
		result.insert (std::multimap<float, core::ImageData*>::value_type(it->second, it->first));
	}
}


void InvertedFileSystem::NormalizeL2 (cv::Mat& descriptor) const
{
	// tf-idf
	int numWords = 0;
	for (int i = 0; i < descriptor.cols; i++)
	{
		float& wordFrequency = descriptor.at<float>(0, i);
		numWords += (wordFrequency > 0 ? 1 : 0);
		wordFrequency *= this->weights[i];
	}
	descriptor /= numWords;

	// l2 normalization
	float squaredSum = 0.0f;
	for (int i = 0; i < descriptor.cols; i++)
	{
		const float& wordFrequency = descriptor.at<float>(0, i);
		squaredSum += (wordFrequency * wordFrequency * this->weights[i]);
	}
	descriptor /= sqrt(squaredSum);
}


} // namespace bof
} // namespace cbsf
} // namespace codemm
