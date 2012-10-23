// OxfordBuildingBofTask.cpp ---------------------------------------------//
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

// NOTE: contains evaluation code from http://www.robots.ox.ac.uk/~vgg/data/oxbuildings/compute_ap.cpp

#include "stdafx.h"
#include "OxfordBuildingBofTask.h"

#include <boost/date_time/posix_time/posix_time.hpp>
#include "OxfordBuildingTask.h"
#include "Configuration.h"
#include "BagOfFeatures.h"
#include "Configuration.h"
#include "BasicFSImageLoader.h"
#include "MinHashLoader.h"
#include "FSContext.h"
#include "ImageData.h"
#include "FileSystemUtility.h"
#include "Log.h"

using namespace std;

namespace codemm {
namespace cbsf {
namespace tasks {


OxfordBuildingBofTask::OxfordBuildingBofTask(void)
{
}


OxfordBuildingBofTask::~OxfordBuildingBofTask(void)
{
}


float OxfordBuildingBofTask::GetMeanAP () const
{
	return this->meanAP;
}


bool OxfordBuildingBofTask::Execute (Configuration& configuration)
{
	using namespace logging;
	using core::ImageData;
	using core::FSContext;

	bof::BagOfFeatures& bagOfFeatures = configuration.GetAndLoadBagOfFeatures ();
	if (!bagOfFeatures.IsVocabularyLoaded ())
	{
		Log().write("No vocabulary available! A vocabulary must exist before Oxford Building Benchmark can be evaluated!");
		return false;
	}

	Log().write ("Starting Oxford Building Benchmark for pure histogram intersection ...");
	
	vector<OxfordQuery> queries;
	OxfordBuildingTask::LoadQueries (configuration.Source / "gt", queries);

	Log().write ("Loading images ...");
	vector<ImageData*> images;
	loader::BasicFSImageLoader loader (configuration.Source, configuration.HashRepository);
	loader.GetImages (images);

	stringstream ss;
	for (vector<ImageData*>::iterator it = images.begin(); it != images.end(); ++it)
	{
		if (!bagOfFeatures.LoadOrComputeDescriptor(**it))
		{
			ss.str("");
			ss << "Calculated descriptor for '" << (*it)->GetName() << "' ...";
			Log().write (ss.str ());
		}
	}
	
	Log().write ("Start quering ...");
	boost::posix_time::time_duration sumDuration;
	for (vector<OxfordQuery>::iterator queryIt = queries.begin(); queryIt != queries.end(); ++queryIt)
	{
		ImageData* pImageData = new ImageData (queryIt->FileName);
		pImageData->SetImageContext (new FSContext (configuration.Source / queryIt->FileName));

		if (configuration.Query_ROI)
		{
			pImageData->SetROI (cv::Rect(queryIt->X, queryIt->Y, queryIt->Width, queryIt->Height));
			bagOfFeatures.ComputeDescriptor (*pImageData);
		}
		
		boost::posix_time::ptime start, stop;
		start = boost::posix_time::microsec_clock::local_time();
		multimap<float, ImageData*> orderedResult;
		for (vector<ImageData*>::iterator it = images.begin(); it != images.end(); ++it)
		{
			float similarity;
			if (configuration.UseBag)
			{
				similarity = bof::BagOfFeatures::Similarity(pImageData->GetDescriptor(), (*it)->GetDescriptor(), bagOfFeatures.GetIdfWeights());
			}
			else
			{
				similarity = bof::BagOfFeatures::SetSimilarity(pImageData->GetDescriptor(), (*it)->GetDescriptor(), bagOfFeatures.GetIdfWeights());
			}

			orderedResult.insert(multimap<float, ImageData*>::value_type(similarity, *it));
		}
		stop = boost::posix_time::microsec_clock::local_time();
		boost::posix_time::time_duration dur = stop - start;
		sumDuration += dur;

		vector<string> rankedList;
		for (multimap<float, ImageData*>::reverse_iterator it = orderedResult.rbegin(); it != orderedResult.rend(); ++it)
		{
			string imageName = it->second->GetName();
			rankedList.push_back (imageName.substr (0, imageName.find(".")));
		}

		set<string> good_set = vector_to_set(load_list((configuration.Source / "gt" / (queryIt->ID + "_good.txt")).string()));
		set<string> ok_set = vector_to_set(load_list((configuration.Source / "gt" / (queryIt->ID + "_ok.txt")).string()));
		set<string> junk_set = vector_to_set(load_list((configuration.Source / "gt" / (queryIt->ID + "_junk.txt")).string()));

		set<string> pos_set;
		pos_set.insert(good_set.begin(), good_set.end());
		pos_set.insert(ok_set.begin(), ok_set.end());

		float ap = compute_ap(pos_set, junk_set, rankedList);
		this->meanAP += ap;

		ss.str("");
		ss << "Query '" << queryIt->ID << "', " << queryIt->FileName << ": keypoints=" << pImageData->GetNumVisualWords() << ", AP=" << ap;
		Log().write (ss.str ());
		
		delete pImageData;
	}

	this->meanAP /= queries.size ();
	
	ss.str("");
	ss << "MeanAP: " << this->meanAP;
	Log().write (ss.str());

	ss.str("");
	ss << "Avg query time: " << (sumDuration / queries.size()).total_milliseconds() << " ms";
	Log().write (ss.str());

	return true;
}


} // namespace tasks
} // namespace cbsf
} // namespace codemm
