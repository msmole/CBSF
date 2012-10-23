// QueryTask.cpp ---------------------------------------------//
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
#include "QueryTask.h"

#include <map>
#include <iostream>
#include <opencv2\highgui\highgui.hpp>

#include "Configuration.h"
#include "BasicFSImageLoader.h"
#include "BagOfFeatures.h"
#include "MinHashing.h"
#include "ImageData.h"
#include "FSContext.h"
#include "ComputeMinHashesTask.h"
#include "QueryImageWindow.h"
#include "InvertedFileSystem.h"
#include "MinHashIndex.h"
#include "MinHashUtility.h"
#include "ResultWindow.h"
#include "Log.h"


using namespace std;
using namespace cv;

namespace codemm {
namespace cbsf {
namespace tasks {


QueryTask::QueryTask(void)
{
}


QueryTask::~QueryTask(void)
{
}


bool QueryTask::Execute (Configuration& configuration)
{
	using namespace logging;
	using loader::BasicFSImageLoader;
	using core::ImageData;
	using core::FSContext;

	bof::BagOfFeatures& bagOfFeatures = configuration.GetAndLoadBagOfFeatures ();
	if (!bagOfFeatures.IsVocabularyLoaded ())
	{
		Log().write("No vocabulary available! A vocabulary must exist before min hashes can be calculated!");
		return false;
	}

	minhash::MinHashUtility minHashUtility = configuration.GetMinHashUtility ();
	
	Log().write("initializing index ...");
	BasicFSImageLoader loader (configuration.Source, configuration.HashRepository);
	core::IIndex* pIndex = configuration.CreateIndex ();
	pIndex->Init (loader);
	Log().write("index initialized!");
	
	cout << "Entering query mode. To exit type 'exit' as query image!" << endl;
	string queryImageName;
		
	while (queryImageName != "exit")
	{
		cout << "Enter query image name: " << endl;
		cin >> queryImageName;
		if (queryImageName == "exit")
		{
			continue;
		}
	
		fs::path imagePath = fs::path(configuration.SourceRepository) / queryImageName;
		if(!fs::exists (imagePath))
		{
			cout << "query image '" << queryImageName << "' does not exist in source repository!" << endl;
			cout << "Source repository: " << configuration.SourceRepository;
			continue;
		}

		ImageData queryImage (queryImageName);
		queryImage.SetImageContext (new FSContext (fs::path(configuration.SourceRepository) / queryImageName));
		queryImage.SetHashContext (new FSContext (configuration.HashRepository / (queryImageName + ".mh")));

		bool sketchesAvailable = false;
		if (configuration.Query_ROI)
		{
			ui::QueryImageWindow window ("query	");
			window.ShowQuery(queryImage.GetImage());
			Rect* pROI = window.GetSelection ();
			if (pROI)
			{
				if (!minHashUtility.IsInitialized())
				{
					Log().write ("Initializing min-Hashing ...");
					minHashUtility.Init ();	
				}
				queryImage.SetROI (*pROI);
				bagOfFeatures.ComputeDescriptor (queryImage);
				minHashUtility.CalculateMinHash (queryImage);
				sketchesAvailable = true;
			}
		}
		if (!sketchesAvailable)
		{
			if (!queryImage.LoadSketches ())
			{
				cout << "No min-hash calculated for query image. Calculate min-hash ..." << endl;
				if (!bagOfFeatures.LoadOrComputeDescriptor (queryImage))
				{
					stringstream ss;
					ss << "Descriptor for image '" << queryImage.GetName() << "' computed!";
					Log().write (ss.str());
				}
				if (!minHashUtility.IsInitialized())
				{
					Log().write ("Initializing min-Hashing ...");
					minHashUtility.Init ();	
				}
				minHashUtility.CalculateMinHash (queryImage);
				queryImage.SaveSketches ();
			}
			else
			{
				queryImage.LoadDescriptor();
			}
		}

		multimap<float, core::ImageData*> rankedResult;
		pIndex->Find (queryImage, rankedResult);

		cout << "Query image has " << queryImage.GetNumVisualWords() << " visual words." << endl;
		cout << "Found " << rankedResult.size() << " images, showing first " << configuration.Query_NumResults << " results:" << endl;
		int i = 0;
		vector<string> imagePaths;
		for (multimap<float, core::ImageData*>::reverse_iterator it = rankedResult.rbegin(); i < configuration.Query_NumResults && it != rankedResult.rend(); ++it, ++i)
		{
			it->second->LoadDescriptor();
			cout << it->second->GetName() << ": " << it->first << ", visual words=" << it->second->GetNumVisualWords() << endl;
			imagePaths.push_back ((it->second->GetImageContext()->GetParentPath() / it->second->GetName()).string());
		}

		ui::ResultWindow resultWindow ("ResultWindow", configuration);
		resultWindow.NumColumns = configuration.Query_ResultCols;
		resultWindow.MaxWidth = configuration.Query_MaxImgSize;
		resultWindow.MaxHeight = configuration.Query_MaxImgSize;
		resultWindow.DrawKeypoints = configuration.Query_DrawKeypoints;
		resultWindow.DrawMatches = configuration.Query_DrawMatches;
		resultWindow.ShowResult (queryImage.GetImage(), imagePaths);
	}
	delete pIndex;

	return true;
}


} // namespace tasks
} // namespace cbsf
} // namespace codemm
