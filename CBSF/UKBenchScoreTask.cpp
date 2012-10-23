// UKBenchScoreTask.cpp ---------------------------------------------//
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
#include "UKBenchScoreTask.h"

#include <boost/date_time/posix_time/posix_time.hpp>

#include "Configuration.h"
#include "BagOfFeatures.h"
#include "Configuration.h"
#include "BasicFSImageLoader.h"
#include "BasicFSImageLoader.h"
#include "ImageData.h"
#include "FileSystemUtility.h"
#include "MinHashIndex.h"
#include "Log.h"


using namespace std;

namespace codemm {
namespace cbsf {
namespace tasks {


UKBenchScoreTask::UKBenchScoreTask(void)
{
}


UKBenchScoreTask::~UKBenchScoreTask(void)
{
}


float UKBenchScoreTask::GetTotalScore () const
{
	return this->totalScore;
}


bool UKBenchScoreTask::Execute (Configuration& configuration)
{
	using namespace logging;
	using core::ImageData;

	bof::BagOfFeatures& bagOfFeatures = configuration.GetAndLoadBagOfFeatures ();
	if (!bagOfFeatures.IsVocabularyLoaded ())
	{
		Log().write("No vocabulary available! A vocabulary must exist before Kentucky-Score can be calculated!");
		return false;
	}

	Log().write ("start calculating UKBench score ...");
	Log().write ("initializing index ...");
	loader::BasicFSImageLoader loader (configuration.Source, configuration.HashRepository);
	core::IIndex* pIndex = configuration.CreateIndex();
	pIndex->Init (loader);
	
	loader.Reset ();
	Log().write ("start quering each image ...");
	stringstream ss;
	float score = 0.0f;
	double docsConsidered = 0.0f;
	int i = 0;
	boost::posix_time::time_duration sumDuration;
	for (; loader.HasNextImage (); ++i)
	{
		ImageData* pImageData = loader.GetNextImage ();

		multimap<float, ImageData*> orderedResult;
		
		boost::posix_time::ptime start, stop;
		start = boost::posix_time::microsec_clock::local_time();
		pIndex->Find (*pImageData, orderedResult);
		stop = boost::posix_time::microsec_clock::local_time();
		boost::posix_time::time_duration dur = stop - start;
		sumDuration += dur;

		ss.str("");
		ss << "Query for '" << pImageData->GetName() << "', got " << orderedResult.size() << " results";
		Log().write (ss.str ());

		docsConsidered += orderedResult.size ();

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
		delete pImageData;
	}

	delete pIndex;
	this->totalScore = score / i;
	
	ss.str("");
	ss << "Avg docs considered: " << (docsConsidered / i) << " (" << (docsConsidered / i) * 100 / i << "%)";
	Log().write (ss.str());
	
	ss.str("");
	ss << "Total Score: " << totalScore;
	Log().write (ss.str());

	ss.str("");
	ss << "Avg query time: " << (sumDuration / i).total_milliseconds() << " ms";
	Log().write (ss.str());

	return true;
}


} // namespace tasks
} // namespace cbsf
} // namespace codemm
