// ComputeVocabularyTask.cpp ---------------------------------------------//
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
#include "ComputeVocabularyTask.h"

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


ComputeVocabularyTask::ComputeVocabularyTask(void)
{
}


ComputeVocabularyTask::~ComputeVocabularyTask(void)
{
}


bool ComputeVocabularyTask::Execute (Configuration& configuration)
{
	using namespace logging;
	using namespace loader;

	Log().write ("calculate visual vocabulary ...");

	if (!fs::exists (configuration.ClusterRepository))
	{
		fs::create_directories (configuration.ClusterRepository);
	}

	bof::BagOfFeatures& bagOfFeatures = configuration.GetBagOfFeatures ();
	if (bagOfFeatures.ReadVocabulary (configuration.VocabularyPath))
	{
		Log().write ("Vocabulary already exists. For safety reasons it will not be overwritten. It must be deleted first!");
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
		bagOfFeatures.ComputeVocabulary (imageLoader, configuration.VisualWords, configuration.Kmeans_Attemps, configuration.Kmeans_Iterations, configuration.Kmeans_Epsilon);
	}
	else if (configuration.TrainingSetSize == Configuration::TrainingSetSize_Undefined)
	{
		BasicFSImageLoader imageLoader (configuration.Source, configuration.HashRepository);
		bagOfFeatures.ComputeVocabulary (imageLoader, configuration.VisualWords, configuration.Kmeans_Attemps, configuration.Kmeans_Iterations, configuration.Kmeans_Epsilon);
	}
	else
	{
		BasicFSImageLoader imageLoader (configuration.Source, configuration.HashRepository);
		RandomImageLoader randomLoader (&imageLoader, configuration.TrainingSetSize);
		bagOfFeatures.ComputeVocabulary (randomLoader, configuration.VisualWords, configuration.Kmeans_Attemps, configuration.Kmeans_Iterations, configuration.Kmeans_Epsilon);
	}
	bagOfFeatures.WriteVocabulary (configuration.VocabularyPath);
	return true;
}


} // namespace tasks
} // namespace cbsf
} // namespace codemm
