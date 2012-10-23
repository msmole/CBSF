// MinHashUtility.cpp ---------------------------------------------//
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
#include "MinHashUtility.h"

#include <opencv2\core\core.hpp>

#include "MinHashing.h"
#include "HistogramMinHashing.h"
#include "Configuration.h"
#include "WeightedHFGenDecorator.h"
#include "WeightedHistogramHFGenDecorator.h"
#include "BagOfFeatures.h"
#include "ImageData.h"
#include "Log.h"


using namespace std;
using namespace cv;

namespace codemm {
namespace cbsf {
namespace minhash {


MinHashUtility::MinHashUtility(Configuration& configuration, const vector<float>& weights /*= vector<float>()*/)
	: configuration(configuration), weights(weights)
{
	this->isInitialized = false;
	this->pMinHash = NULL;
	this->pMinHashHist = NULL;
}


MinHashUtility::~MinHashUtility(void)
{
	if (this->pMinHash)
	{
		delete this->pMinHash;
		this->pMinHash = NULL;
	}
	if (this->pMinHashHist)
	{
		delete this->pMinHashHist;
		this->pMinHashHist = NULL;
	}
}


void MinHashUtility::Init ()
{
	using namespace logging;

	if (this->configuration.UseBag)
	{
		this->pMinHashHist = new HistogramMinHashing (this->configuration.NumSketches, this->configuration.SketchSize, this->configuration.VisualWords, this->configuration.Accuracy);
		RandomUniformGen* pRandGen = new RandomUniformGen ();
		if (this->configuration.UseWeights)
		{
			Log().write("using weighted histogram min-hashing ...");
			WeightedHistogramHFGenDecorator* pGen = new WeightedHistogramHFGenDecorator (pRandGen, this->weights, this->configuration.Accuracy);
			this->pMinHashHist->SetHashFunctionGenerator (pGen);
		}
		else
		{
			Log().write("using histogram min-hashing ...");
			this->pMinHashHist->SetHashFunctionGenerator (pRandGen);
		}
		Log().write("loading hash functions ...");
		if (!this->pMinHashHist->LoadHashFunctions ((this->configuration.DataRepository / "hashfunctions.txt").string()))
		{
			Log().write ("no hash functions available ... creating them ...");
			this->pMinHashHist->GenerateHashFunctions();
			this->pMinHashHist->SaveHashFunctions ((this->configuration.DataRepository / "hashfunctions.txt").string());
		}	
	}
	else
	{
		this->pMinHash = new MinHashing<int> (this->configuration.NumSketches, this->configuration.SketchSize);
		set<int> universalSet;
		for (int i = 0; i < this->configuration.VisualWords; ++i)
		{
			universalSet.insert (i);
		}
		this->pMinHash->Init (universalSet);
		RandomUniformGen* pRandGen = new RandomUniformGen ();
		if (this->configuration.UseWeights)
		{
			Log().write("using weighted min-hashing ...");
			WeightedHFGenDecorator* pGen = new WeightedHFGenDecorator (pRandGen, this->weights);
			this->pMinHash->SetHashFunctionGenerator (pGen);
		}
		else
		{
			Log().write("using default min-hashing ...");
			this->pMinHash->SetHashFunctionGenerator (pRandGen);
		}
		Log().write("loading hash functions ...");
		if (!this->pMinHash->LoadHashFunctions ((this->configuration.DataRepository / "hashfunctions.txt").string()))
		{
			Log().write ("no hash functions available ... creating them ...");
			this->pMinHash->GenerateHashFunctions();
			this->pMinHash->SaveHashFunctions ((this->configuration.DataRepository / "hashfunctions.txt").string());
		}
	}
	this->isInitialized = true;
}


void MinHashUtility::CalculateMinHash (core::ImageData& imageData)
{
	assert (this->isInitialized);
	assert (this->pMinHash || this->pMinHashHist);
	
	if (!this->configuration.GetAndLoadBagOfFeatures().LoadOrComputeDescriptor (imageData))
	{
		stringstream ss;
		ss << "Descriptor for image '" << imageData.GetName() << "' computed!";
		logging::Log().write (ss.str());
	}

	if (this->pMinHashHist)
	{
		vector<float> hist;
		Mat2Vector(imageData.GetDescriptor(), hist);
		pMinHashHist->CalculateMinHash (hist, imageData.GetSketches());
	}
	else
	{
		set<int> wordSet;
		Mat2Set(imageData.GetDescriptor(), wordSet);
		pMinHash->CalculateMinHash (wordSet, imageData.GetSketches());
	}
}


bool MinHashUtility::IsInitialized () const
{
	return this->isInitialized;
}


void MinHashUtility::Mat2Vector (const Mat& mat, vector<float>& vec)
{
	for (int i = 0; i < mat.cols; i++)
	{
		const float& elem = mat.at<float>(0, i);
		vec.push_back (elem);
	}
}


void MinHashUtility::Mat2Set (const Mat& mat, set<int>& wordSet)
{
	for (int i = 0; i < mat.cols; i++)
	{
		const float& elem = mat.at<float>(0, i);
		if (elem > 0.0f)
		{
			wordSet.insert (i);
		}
	}
}


} // namespace minhash
} // namespace cbsf
} // namespace codemm
