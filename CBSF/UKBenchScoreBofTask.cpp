// UKBenchScoreBofTask.cpp ---------------------------------------------//
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
#include "UKBenchScoreBofTask.h"

#include <boost/date_time/posix_time/posix_time.hpp>

#include "Configuration.h"
#include "BagOfFeatures.h"
#include "Configuration.h"
#include "BasicFSImageLoader.h"
#include "ImageData.h"
#include "FileSystemUtility.h"
#include "Log.h"


using namespace std;

namespace codemm {
namespace cbsf {
namespace tasks {


UKBenchScoreBofTask::UKBenchScoreBofTask(void)
{
}


UKBenchScoreBofTask::~UKBenchScoreBofTask(void)
{
}


float UKBenchScoreBofTask::GetTotalScore () const
{
	return this->totalScore;
}


bool UKBenchScoreBofTask::Execute (Configuration& configuration)
{
	using logging::Log;
	using core::ImageData;

	bof::BagOfFeatures& bagOfFeatures = configuration.GetAndLoadBagOfFeatures ();
	if (!bagOfFeatures.IsVocabularyLoaded ())
	{
		Log().write("No vocabulary available! A vocabulary must exist before UKBench score can be calculated!");
		return false;
	}

	Log().write ("Start calculating UKBench score for pure histogram intersection ...");
	loader::BasicFSImageLoader loader (configuration.Source, configuration.HashRepository);
	
	Log().write ("Loading images ...");
	vector<ImageData*> images;
	loader.GetImages (images);

	stringstream ss;
	Log().write ("Checking descriptor file availability ...");
	for (vector<ImageData*>::iterator it = images.begin(); it != images.end(); ++it)
	{
		ImageData* pImageData = *it;
		if (!bagOfFeatures.LoadOrComputeDescriptor(*pImageData))
		{
			ss.str("");
			ss << "Calculated descriptor for '" << pImageData->GetName() << "' ...";
			Log().write (ss.str ());
		}
	}

	Log().write ("Start quering each image ...");
	float score = 0.0f;
	int i = 0;
	boost::posix_time::time_duration sumDuration;
	for (vector<ImageData*>::iterator it = images.begin(); it != images.end(); ++it, ++i)
	{
		ImageData* pQueryImage = *it;
		
		ss.str("");
		ss << "Query for '" << pQueryImage->GetName() << "' ...";
		Log().write (ss.str ());

		multimap<float, ImageData*> orderedResult;

		boost::posix_time::ptime start, stop;
		start = boost::posix_time::microsec_clock::local_time();
		
		for (vector<ImageData*>::iterator it1 = images.begin(); it1 != images.end(); ++it1)
		{
			ImageData* pImageData = *it1;
			float similarity;
			if (configuration.UseBag)
			{
				similarity = bof::BagOfFeatures::Similarity(pQueryImage->GetDescriptor(), pImageData->GetDescriptor(), bagOfFeatures.GetIdfWeights());
			}
			else
			{
				similarity = bof::BagOfFeatures::SetSimilarity(pQueryImage->GetDescriptor(), pImageData->GetDescriptor(), bagOfFeatures.GetIdfWeights());
			}

			if (orderedResult.size() < 4 || similarity > orderedResult.begin()->first)
			{
				// add new element to results
				if (orderedResult.size() >= 4)
				{
					orderedResult.erase(orderedResult.begin());
				}
				orderedResult.insert (multimap<float, ImageData*>::value_type(similarity, pImageData));
			}
		}

		stop = boost::posix_time::microsec_clock::local_time();
		boost::posix_time::time_duration dur = stop - start;
		sumDuration += dur;

		int resultCount = 0;
		int currentBlock = i / 4;
		int queryScore = 0;
		stringstream queryResult;
		for (multimap<float, ImageData*>::reverse_iterator it = orderedResult.rbegin(); resultCount < 4 && it != orderedResult.rend(); ++it, ++resultCount)
		{
			const string& imageName = it->second->GetName ();
			queryResult << imageName << " [" << it->first << "], ";

			string number = imageName.substr (7, 5);
			int imageNumber = atoi (number.c_str());

			if (imageNumber / 4 == currentBlock)
			{
				queryScore ++;
			}
		}
		score += queryScore;
		ss.str("");
		ss << "Score: " << queryScore << ", Top 4: " << queryResult.str();
		Log().write (ss.str ());
	}

	for (vector<ImageData*>::iterator it = images.begin(); it != images.end(); ++it)
	{
		delete *it;
	}

	float totalScore = score / i;
	ss.str("");
	ss << "Total Score: " << totalScore;
	Log().write (ss.str());

	ss.str("");
	ss << "Avg query time: " << (sumDuration / i).total_milliseconds() << " ms";
	Log().write (ss.str());

	return true;
}


//bool UKBenchScoreBofTask::Execute (Configuration& configuration)
//{
//	using namespace logging;
//	using core::ImageData;
//	using namespace core;
//
//	bof::BagOfFeatures& bagOfFeatures = configuration.GetAndLoadBagOfFeatures ();
//	if (!bagOfFeatures.IsVocabularyLoaded ())
//	{
//		Log().write("No vocabulary available! A vocabulary must exist before UKBench score can be calculated!");
//		return false;
//	}
//
//	Log().write ("Start calculating UKBench score for pure histogram intersection ...");
//	loader::BasicFSImageLoader loader (configuration.Source, configuration.HashRepository);
//	
//	Log().write ("Loading images ...");
//	vector<ImageData*> images;
//	loader.GetImages (images);
//
//	vector<vector<float>> similarityMatrix;
//	Log().write ("Loading similarity matrix ...");
//	ifstream ifs ((configuration.HashRepository.parent_path().parent_path() / "similarityMatrix.csv").string());
//	ifs.imbue(std::locale(""));
//	ifs >> similarityMatrix;
//	ifs.close();
//
//	ofstream ofs ((configuration.HashRepository.parent_path().parent_path() / "similarityMatrix.csv").string());
//	ofs.imbue (std::locale(""));
//
//	stringstream ss;
//	similarityMatrix.resize(images.size());
//	Log().write ("Building similarity matrix ...");
//	int block = 5000;
//	for (unsigned int i = 0; i < images.size(); ++i)
//	{
//		if (similarityMatrix[i].size() >= images.size() - i)
//		{
//			ofs << similarityMatrix[i] << endl;
//			continue;
//		}
//
//		if (!bagOfFeatures.LoadOrComputeDescriptor(*images[i]))
//		{
//			ss.str("");
//			ss << "Calculated descriptor for '" << images[i]->GetName() << "' ...";
//			Log().write (ss.str ());
//		}
//
//		ss.str("");
//		ss << "Calculate similarities for '" << images[i]->GetName() << "' ...";
//		Log().write (ss.str());
//
//		similarityMatrix[i].resize(images.size() - i);
//		for (unsigned int j = 0; j < images.size() - i; ++j)
//		{
//			int index = j + i;
//			if (!bagOfFeatures.LoadOrComputeDescriptor(*images[index]))
//			{
//				ss.str("");
//				ss << "Calculated descriptor for '" << images[index]->GetName() << "' ...";
//				Log().write (ss.str ());
//			}
//
//			float similarity;
//			if (configuration.UseBag)
//			{
//				similarity = bof::BagOfFeatures::Similarity(images[i]->GetDescriptor(), images[index]->GetDescriptor(), bagOfFeatures.GetIdfWeights());
//			}
//			else
//			{
//				similarity = bof::BagOfFeatures::SetSimilarity(images[i]->GetDescriptor(), images[index]->GetDescriptor(), bagOfFeatures.GetIdfWeights());
//			}
//			similarityMatrix[i][j] = similarity;
//			
//			// if the cluster count is too big we have to release some of the descriptors
//			// in order to not use to much memory
//			if (configuration.VisualWords > 30000 && index > block + (int) i)
//			{
//				images[index]->GetDescriptor().release();
//			}
//		}
//		ofs << similarityMatrix[i] << endl;
//		ofs.flush();
//
//		images[i]->GetDescriptor().release();
//	}
//	ofs.close();
//
//	Log().write ("Start quering each image ...");
//	float score = 0.0f;
//	for (unsigned int i = 0; i < images.size(); ++i)
//	{
//		ss.str("");
//		ss << "Query for '" << images[i]->GetName() << "' ...";
//		Log().write (ss.str ());
//
//		multimap<float, ImageData*> orderedResult;
//		for (unsigned int j = 0; j < images.size(); ++j)
//		{
//			float similarity;
//			if (j < i)
//			{
//				similarity = similarityMatrix[j][i - j];
//			}
//			else
//			{
//				similarity = similarityMatrix[i][j - i];
//			}
//			
//			if (orderedResult.size() < 4 || similarity > orderedResult.begin()->first)
//			{
//				// add new element to results
//				if (orderedResult.size() >= 4)
//				{
//					orderedResult.erase(orderedResult.begin());
//				}
//				orderedResult.insert (multimap<float, ImageData*>::value_type(similarity, images[j]));
//			}
//		}
//
//		int resultCount = 0;
//		int currentBlock = i / 4;
//		int queryScore = 0;
//		stringstream queryResult;
//		for (multimap<float, ImageData*>::reverse_iterator it = orderedResult.rbegin(); resultCount < 4 && it != orderedResult.rend(); ++it, ++resultCount)
//		{
//			const string& imageName = it->second->GetName ();
//			queryResult << imageName << " [" << it->first << "], ";
//
//			string number = imageName.substr (7, 5);
//			int imageNumber = atoi (number.c_str());
//
//			if (imageNumber / 4 == currentBlock)
//			{
//				queryScore ++;
//			}
//		}
//		score += queryScore;
//		ss.str("");
//		ss << "Score: " << queryScore << ", Top 4: " << queryResult.str();
//		Log().write (ss.str ());
//	}
//
//	for (vector<ImageData*>::iterator it = images.begin(); it != images.end(); ++it)
//	{
//		delete *it;
//	}
//
//	this->totalScore = score / images.size();
//	ss.str("");
//	ss << "Total Score: " << totalScore;
//	Log().write (ss.str());
//
//	return true;
//}


} // namespace tasks
} // namespace cbsf
} // namespace codemm
