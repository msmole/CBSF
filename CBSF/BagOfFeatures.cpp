// BagOfFeatures.cpp ---------------------------------------------//
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
#include "BagOfFeatures.h"

#include <math.h>
#include <set>
#include <boost/date_time/posix_time/posix_time.hpp>

#include "IImageLoader.h"
#include "ImageData.h"
#include "Log.h"


using namespace std;
using namespace cv;
using namespace codemm::cbsf::core;
using namespace codemm::cbsf::logging;


namespace codemm {
namespace cbsf {
namespace bof {


BagOfFeatures::BagOfFeatures(Ptr<FeatureDetector> ptrFeatureDetector, Ptr<DescriptorExtractor> ptrDescriptorExtractor, Ptr<DescriptorMatcher> ptrDescriptorMatcher)
{
	this->ptrFeatureDetector = ptrFeatureDetector;
	this->ptrDescriptorExtractor = ptrDescriptorExtractor;
	this->ptrDescriptorMatcher = ptrDescriptorMatcher;
	this->vocabularyLoaded = false;
}


BagOfFeatures::~BagOfFeatures(void)
{
}


bool BagOfFeatures::ReadVocabulary (const fs::path& filename)
{
	FileStorage fs (filename.string(), FileStorage::READ);
    if (fs.isOpened())
    {
        fs["vocabulary"] >> this->vocabulary;
		this->vocabularyLoaded = true;
		if (this->GetDescriptorDimension() != this->ptrDescriptorExtractor->descriptorSize())
		{
			stringstream ss;
			ss << "dimension of vocabulary (" << this->GetDescriptorDimension() << ") and dimension of descriptor extractor (" << this->ptrDescriptorExtractor->descriptorSize() << ") does not match!";
			throw exception (ss.str().c_str());
		}
        return true;
    }
    return false;
}



bool BagOfFeatures::WriteVocabulary (const fs::path& filename)
{
	FileStorage fs (filename.string(), FileStorage::WRITE);
    if (fs.isOpened())
    {
        fs << "vocabulary" << this->vocabulary;
        return true;
    }
    return false;
}


bool BagOfFeatures::ReadWeights (const fs::path& filename)
{
	return FileSystemUtility::LoadVector (filename, this->idfWeights);
}


void BagOfFeatures::WriteWeights (const fs::path& filename)
{
	FileSystemUtility::SaveVector (filename, this->idfWeights);
}


bool BagOfFeatures::ReadMaxWordFrequencies (const fs::path& filename)
{
	return FileSystemUtility::LoadVector (filename, this->maxDescValues);
}


void BagOfFeatures::WriteMaxWordFrequencies (const fs::path& filename)
{
	FileSystemUtility::SaveVector (filename, this->maxDescValues);
}


void BagOfFeatures::ComputeVocabulary (loader::IImageLoader& imageLoader, int clusterCount, int attemps, int maxIterations, float epsilon)
{
	TermCriteria tc(CV_TERMCRIT_EPS + CV_TERMCRIT_ITER, maxIterations, epsilon);
	BOWKMeansTrainer bowTrainer (clusterCount, tc, attemps, KMEANS_PP_CENTERS);

	stringstream ss;
	int i = 0;
	double numKeyPoints = 0.0;
	for (;imageLoader.HasNextImage (); ++i)
	{
		vector<KeyPoint> keyPoints;
		Mat descriptor;
		ImageData* pImageData = imageLoader.GetNextImage ();
		
		this->ptrFeatureDetector->detect(pImageData->GetImage (), keyPoints);
		if (keyPoints.size() > 0)
		{
			this->ptrDescriptorExtractor->compute (pImageData->GetImage(), keyPoints, descriptor);
			if (descriptor.rows > 0)
			{
				numKeyPoints += descriptor.rows;
				bowTrainer.add (descriptor);
				ss.str("");
				ss << "Trained " << pImageData->GetImageContext()->GetContextName() << ", " << keyPoints.size() << " keypoints found.";
				Log().write(ss.str());
			}
			else
			{
				ss.str("");
				ss << "WARNING: No descriptor extracted for " << pImageData->GetImageContext()->GetContextName() << ", " << keyPoints.size() << " keypoints found.";
				Log().write(ss.str());
			}
		}
		else
		{
			ss.str("");
			ss << "WARNING: No keypoints detected for " << pImageData->GetImageContext()->GetContextName() << ".";
			Log().write(ss.str());
		}

		
		delete pImageData;
	}

	ss.str("");
	ss << "Detected " << (numKeyPoints / i) << " keypoints per image in average.";
	Log().write (ss.str());

	ss.str("");
	ss << "Clustering " << bowTrainer.descripotorsCount() << " descriptors ...";
	Log().write (ss.str());

	boost::posix_time::ptime start, stop;
	start = boost::posix_time::microsec_clock::local_time();
	this->vocabulary = bowTrainer.cluster ();
	stop = boost::posix_time::microsec_clock::local_time();
	boost::posix_time::time_duration dur = stop - start;
	Log().write ("Clustering finished ...");
	
	ss.str("");
	ss << "Needed " << dur.total_seconds() / 60 << " minutes, " << dur.total_seconds() % 60 << " secs ";
	Log().write (ss.str());

	this->vocabularyLoaded = true;
}


void BagOfFeatures::ComputeWeights (loader::IImageLoader& imageLoader)
{
	assert (this->vocabularyLoaded);

	Log().write ("Calculate idf weights ...");

	stringstream ss;
	this->idfWeights.clear ();

	int clusterCount = this->GetVocabularySize();
	this->idfWeights.resize (clusterCount);
	this->maxDescValues.resize (clusterCount);

	int count = 0;
	for (;imageLoader.HasNextImage (); ++count)
	{
		ImageData* pImageData = imageLoader.GetNextImage ();
		if (!this->LoadOrComputeDescriptor (*pImageData))
		{
			ss.str("");
			ss << "Descriptor for image '" << pImageData->GetName () << "' computed.";
			Log().write (ss.str ());
		}

		const Mat& desc = pImageData->GetDescriptor();
		for (int i = 0; i < clusterCount; ++i)
		{

			const float& descElem = desc.at<float>(0, i);
			if (descElem > 0)
			{
				this->idfWeights[i] ++;
			}

			if (descElem > this->maxDescValues[i])
			{
				this->maxDescValues[i] = descElem;
			}
		}

		delete pImageData;
	}

	for (int i = 0; i < clusterCount; ++i)
	{
		// just in case that there doesn't exist any image that contains that visual word we add 1 the term
		this->idfWeights[i] = log(count / (1 + this->idfWeights[i]));
	}

	Log().write ("Idf weights calculated!");
}


bool BagOfFeatures::LoadOrComputeDescriptor (core::ImageData& imageData)
{
	if (imageData.GetDescriptor().data == NULL)
	{
		if (!imageData.LoadDescriptor())
		{
			this->ComputeDescriptor (imageData);
			imageData.SaveDescriptor ();
			return false;
		}
	}
	return true;
}


void BagOfFeatures::ComputeDescriptor (ImageData& imageData)
{
	assert (this->vocabularyLoaded);
	vector<KeyPoint> keyPoints;
	this->ptrFeatureDetector->detect (imageData.GetImage(), keyPoints);

	if (keyPoints.size() > 0)
	{
		this->GetBowImgDE().compute (imageData.GetImage(), keyPoints, imageData.GetDescriptor());
	}
	else
	{
		imageData.GetDescriptor() = Mat::zeros(1, this->vocabulary.rows, CV_32F);
	}

	// denormalize
	imageData.GetDescriptor() *= ((double) keyPoints.size());
}



void BagOfFeatures::ComputeDescriptors (vector<ImageData*>& images)
{
	assert (this->vocabularyLoaded);
	for (vector<ImageData*>::iterator it = images.begin(); it != images.end(); ++it)
	{
		this->ComputeDescriptor (**it);
	}
}


BOWImgDescriptorExtractor& BagOfFeatures::GetBowImgDE ()
{
	if (!this->ptrBowImgDE)
	{
		this->ptrBowImgDE = new BOWImgDescriptorExtractor (this->ptrDescriptorExtractor, this->ptrDescriptorMatcher);
		this->ptrBowImgDE->setVocabulary (this->vocabulary);
	}
	return *(this->ptrBowImgDE);
}


bool BagOfFeatures::IsVocabularyLoaded () const
{
	return this->vocabularyLoaded;
}


int BagOfFeatures::GetVocabularySize () const
{
	assert (this->vocabularyLoaded);
	return this->vocabulary.rows;
}


int BagOfFeatures::GetDescriptorDimension () const
{
	assert (this->vocabularyLoaded);
	return this->vocabulary.cols;
}


const vector<float>& BagOfFeatures::GetIdfWeights ()
{
	if (this->idfWeights.size() != this->GetVocabularySize())
	{
		// set default weights in case the weights have not been loaded!
		this->idfWeights.resize (this->GetVocabularySize(), 1.0f);
	}
	return this->idfWeights;
}


const vector<float>& BagOfFeatures::GetMaxWordFrequencies () const
{
	return this->maxDescValues;
}


cv::FeatureDetector* BagOfFeatures::GetFeatureDetector ()
{
	return this->ptrFeatureDetector;
}


cv::DescriptorExtractor* BagOfFeatures::GetDescriptorExtractor ()
{
	return this->ptrDescriptorExtractor;
}


cv::DescriptorMatcher* BagOfFeatures::GetDescriptorMatcher ()
{
	return this->ptrDescriptorMatcher;
}


float BagOfFeatures::Similarity (const Mat& d1, const Mat& d2, const vector<float>& idfWeights)
{
	assert (d1.cols == d2.cols);

	double sumMin = 0.0;
	double sumMax = 0.0;
	for (int i = 0; i < d1.cols; i++)
	{
		const float& elem1 = d1.at<float>(0, i);
		const float& elem2 = d2.at<float>(0, i);

		float t1 = (elem1 < elem2 ? elem1 : elem2);
		float t2 = (elem1 < elem2 ? elem2 : elem1);
		
		sumMin += t1 * idfWeights[i];
		sumMax += t2 * idfWeights[i];
	}
	if (sumMin > 0 && sumMax > 0)
	{
		return (float) (sumMin / sumMax);
	}
	return 0;
}


float BagOfFeatures::SetSimilarity (const Mat& d1, const Mat& d2, const vector<float>& idfWeights)
{
	assert (d1.cols == d2.cols);

	double sumMin = 0.0;
	double sumMax = 0.0;
	for (int i = 0; i < d1.cols; i++)
	{
		const float& elem1 = d1.at<float>(0, i);
		const float& elem2 = d2.at<float>(0, i);

		float t1 = (elem1 > 0 && elem2 > 0) ? 1.0f : 0.0f;
		float t2 = (elem1 > 0 || elem2 > 0) ? 1.0f : 0.0f;
		
		sumMin += t1 * idfWeights[i];
		sumMax += t2 * idfWeights[i];
	}
	if (sumMin > 0 && sumMax > 0)
	{
		return (float) (sumMin / sumMax);
	}
	return 0;
}


} // namespace bof
} // namespace cbsf
} // namespace codemm
