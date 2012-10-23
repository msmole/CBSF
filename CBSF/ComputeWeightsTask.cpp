// ComputeWeightsTask.cpp ---------------------------------------------//
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
#include "ComputeWeightsTask.h"

#include "Configuration.h"
#include "ListImageLoader.h"
#include "BasicFSImageLoader.h"
#include "RandomImageLoader.h"
#include "BagOfFeatures.h"
#include "CreateRandomImageListTask.h"
#include "Log.h"


using namespace std;

namespace codemm {
namespace cbsf {
namespace tasks {


ComputeWeightsTask::ComputeWeightsTask(void)
{
}


ComputeWeightsTask::~ComputeWeightsTask(void)
{
}


bool ComputeWeightsTask::Execute (Configuration& configuration)
{
	using namespace logging;
	using namespace loader;

	Log().write ("calculate idf weights ...");

	if (!fs::exists (configuration.ClusterRepository))
	{
		fs::create_directories (configuration.ClusterRepository);
	}

	Log().write ("read vocabulary ...");
	bof::BagOfFeatures& bagOfFeatures = configuration.GetAndLoadBagOfFeatures ();
	if (!bagOfFeatures.IsVocabularyLoaded())
	{
		Log().write("No vocabulary available! A vocabulary must exist before idf weights can be calculated!");
		return false;
	}
	
	if (configuration.UseImageList && configuration.TrainingSetSize != Configuration::TrainingSetSize_Undefined)
	{
		stringstream imageListName;
		imageListName << "imageList_" << configuration.TrainingSetSize << ".txt";
	
		vector<fs::path> imageList;
		if (!ListImageLoader::LoadImageList (configuration.Target / imageListName.str(), imageList))
		{
			// create new image list
			CreateRandomImageListTask createRandomImageListTask;
			createRandomImageListTask.SetImageListName (imageListName.str());
			createRandomImageListTask.Execute (configuration);
			imageList = createRandomImageListTask.GetImageFileList ();
		}

		// use existing image list
		ListImageLoader imageLoader (imageList, configuration.HashRepository);
		bagOfFeatures.ComputeWeights (imageLoader);
	}
	else if (configuration.TrainingSetSize == Configuration::TrainingSetSize_Undefined)
	{
		BasicFSImageLoader imageLoader (configuration.Source, configuration.HashRepository);
		bagOfFeatures.ComputeWeights (imageLoader);
	}
	else
	{
		BasicFSImageLoader imageLoader (configuration.Source, configuration.HashRepository);
		RandomImageLoader randomLoader (&imageLoader, configuration.TrainingSetSize);
		bagOfFeatures.ComputeWeights (imageLoader);
	}

	bagOfFeatures.WriteWeights (configuration.WeightsPath);
	bagOfFeatures.WriteMaxWordFrequencies (configuration.MaxWordFrequenciesPath);

	const vector<float>& weights = bagOfFeatures.GetIdfWeights();
	const vector<float>& frequencies = bagOfFeatures.GetMaxWordFrequencies();

	Log().write ("Weights and frequencies for each visual word (word no; weight; max frequency):");
	stringstream ss;
	float maxFrequency = FLT_MIN;
	double sum = 0;
	for (unsigned int i = 0; i < weights.size(); ++i)
	{
		ss.str("");
		ss << (i + 1) << ";" << weights[i] << ";" << frequencies[i];
		Log().write (ss.str ());

		if (maxFrequency < frequencies[i])
		{
			maxFrequency = frequencies[i];
		}
		sum += frequencies[i];
	}

	ss.str("");
	ss << "Max frequency: " << maxFrequency;
	Log().write (ss.str());
	ss.str("");
	ss << "Avg frequency: " << (sum / weights.size());
	Log().write (ss.str());

	return true;
}


} // namespace tasks
} // namespace cbsf
} // namespace codemm
