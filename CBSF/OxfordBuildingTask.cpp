// OxfordBuildingTask.cpp ---------------------------------------------//
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
#include "OxfordBuildingTask.h"

#include <boost/date_time/posix_time/posix_time.hpp>

#include "Configuration.h"
#include "BagOfFeatures.h"
#include "Configuration.h"
#include "BasicFSImageLoader.h"
#include "FSContext.h"
#include "ImageData.h"
#include "IIndex.h"
#include "FileSystemUtility.h"
#include "MinHashUtility.h"
#include "Log.h"


using namespace std;

namespace codemm {
namespace cbsf {
namespace tasks {


OxfordBuildingTask::OxfordBuildingTask(void)
{
}


OxfordBuildingTask::~OxfordBuildingTask(void)
{
}


float OxfordBuildingTask::GetMeanAP () const
{
	return this->meanAP;
}


bool OxfordBuildingTask::Execute (Configuration& configuration)
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

	Log().write ("Starting Oxford Building Benchmark ...");
	Log().write ("Initializing min-Hashing ...");
	minhash::MinHashUtility minHashUtility = configuration.GetMinHashUtility ();
	if (configuration.Query_ROI)
	{
		minHashUtility.Init ();
	}
	
	Log().write ("Initializing index ...");
	loader::BasicFSImageLoader loader (configuration.Source, configuration.HashRepository);
	core::IIndex* pIndex = configuration.CreateIndex ();
	pIndex->Init (loader);
	
	vector<OxfordQuery> queries;
	LoadQueries (configuration.Source / "gt", queries);
	
	Log().write ("Start quering ...");
	stringstream ss;
	boost::posix_time::time_duration sumDuration;
	for (vector<OxfordQuery>::iterator queryIt = queries.begin(); queryIt != queries.end(); ++queryIt)
	{
		ImageData* pImageData = new ImageData (queryIt->FileName);
		pImageData->SetImageContext (new FSContext (configuration.Source / queryIt->FileName));
		stringstream hashFileName;
		hashFileName << queryIt->FileName << ".mh";
		pImageData->SetHashContext (new FSContext (configuration.HashRepository / hashFileName.str()));
		
		if (configuration.Query_ROI)
		{
			pImageData->SetROI (cv::Rect(queryIt->X, queryIt->Y, queryIt->Width, queryIt->Height));
			bagOfFeatures.ComputeDescriptor (*pImageData);
			minHashUtility.CalculateMinHash (*pImageData);
		}

		multimap<float, ImageData*> orderedResult;
		
		boost::posix_time::ptime start, stop;
		start = boost::posix_time::microsec_clock::local_time();
		pIndex->Find (*pImageData, orderedResult);
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
		ss << "Query '" << queryIt->ID << "', " << queryIt->FileName << ": keypoints=" << pImageData->GetNumVisualWords() << ", AP=" << ap << ", got " << orderedResult.size() << " results";
		Log().write (ss.str ());
		
		delete pImageData;
	}

	delete pIndex;
	this->meanAP /= queries.size ();
	
	ss.str("");
	ss << "MeanAP: " << this->meanAP;
	Log().write (ss.str());

	ss.str("");
	ss << "Avg query time: " << (sumDuration / queries.size()).total_milliseconds() << " ms";
	Log().write (ss.str());

	return true;
}


void OxfordBuildingTask::LoadQueries (const fs::path& path, vector<OxfordQuery>& queries)
{
	fs::directory_iterator endIter;
	try
	{
		for (fs::directory_iterator it (path); it != endIter; ++it)
		{
			if (it->path().filename().string().find("query") != string::npos)
			{
				queries.push_back(CreateOxfordQuery(*it));
			}
		}
	}
	catch (fs::filesystem_error e)
	{
		stringstream ss;
		ss << "Unexpected error in OxfordBuildingTask::LoadQueries: " << e.what();
		logging::Log().write (ss.str());
	}
}


OxfordQuery OxfordBuildingTask::CreateOxfordQuery (const fs::path& queryFile)
{
	ifstream ifs (queryFile.string());
	string line;
	OxfordQuery query;
	if (ifs.good ())
	{
		getline (ifs, line);
		if (!line.empty())
		{
			stringstream ss (line);
			string imageFileName;
			float x, y, xx, yy;
			ss >> imageFileName >> x >> y >> xx >> yy;

			query.X = (int) x;
			query.Y = (int) y;
			query.Width = (int) (xx - query.X);
			query.Height = (int) (yy - query.Y);
			
			imageFileName = imageFileName.substr (imageFileName.find("_") + 1);
			stringstream nameStream;
			nameStream << imageFileName << ".jpg";
			query.FileName = nameStream.str();

			query.ID = queryFile.filename().string();
			query.ID = query.ID.substr (0, query.ID.find ("."));
			query.ID = query.ID.substr (0, query.ID.find_last_of ("_"));
		}
	}
	return query;
}


// the following evaluation code is from http://www.robots.ox.ac.uk/~vgg/data/oxbuildings/compute_ap.cpp
vector<string>
load_list(const string& fname)
{
  vector<string> ret;
  ifstream fobj(fname.c_str());
  if (!fobj.good()) { cerr << "File " << fname << " not found!\n"; exit(-1); }
  string line;
  while (getline(fobj, line)) {
    ret.push_back(line);
  }
  return ret;
}


float
compute_ap(const set<string>& pos, const set<string>& amb, const vector<string>& ranked_list)
{
  float old_recall = 0.0;
  float old_precision = 1.0;
  float ap = 0.0;
  
  size_t intersect_size = 0;
  size_t i = 0;
  size_t j = 0;
  for ( ; i<ranked_list.size(); ++i) {
    if (amb.count(ranked_list[i])) continue;
    if (pos.count(ranked_list[i])) intersect_size++;

    float recall = intersect_size / (float)pos.size();
    float precision = intersect_size / (j + 1.0f);

    ap += (recall - old_recall) * ((old_precision + precision) / 2.0f);

    old_recall = recall;
    old_precision = precision;
    j++;
  }
  return ap;
}


} // namespace tasks
} // namespace cbsf
} // namespace codemm
