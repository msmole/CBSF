// RunEvaluationTask.cpp ---------------------------------------------//
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
#include "RunEvaluationTask.h"

#include <map>

#include "Configuration.h"
#include "RandomImageLoader.h"
#include "MinHashLoader.h"
#include "BagOfFeatures.h"
#include "MinHashing.h"
#include "ImageData.h"
#include "Log.h"


using namespace std;

namespace codemm {
namespace cbsf {
namespace tasks {


RunEvaluationTask::RunEvaluationTask(void)
{
}


RunEvaluationTask::~RunEvaluationTask(void)
{
}


bool RunEvaluationTask::Execute (Configuration& configuration)
{
	using namespace logging;
	using core::ImageData;

	Log().write("run dataset evaluation task ...");
	bof::BagOfFeatures& bagOfFeatures = configuration.GetAndLoadBagOfFeatures ();
	if (!bagOfFeatures.IsVocabularyLoaded ())
	{
		Log().write("No vocabulary available! A vocabulary must exist prior to evaluation!");
		return false;
	}

	Log().write("loading min hashes ...");
	loader::IImageLoader* pLoader = NULL;
	loader::MinHashLoader* pMinHashLoader = new loader::MinHashLoader (configuration.DataRepository, configuration.Source);
	if (configuration.TestSetSize != Configuration::TestSetSize_Undefined
		&& configuration.TestSetType == Configuration::TestSetType_Random)
	{
		loader::RandomImageLoader* pRandomLoader = new loader::RandomImageLoader (pMinHashLoader, configuration.TestSetSize);
		pLoader = pRandomLoader;
	}
	else
	{
		pLoader = pMinHashLoader;
	}
	
	int numberOfImages = (configuration.TestSetSize == Configuration::TestSetSize_Undefined ? pLoader->GetNumberImages() : configuration.TestSetSize);
	Log().write("creating similarity matrices ...");
	ofstream osHashSim ((configuration.DataRepository / "hash_similarity.csv").string());
	ofstream osSketchHits ((configuration.DataRepository / "sketch_hits.csv").string());
	ofstream osBoFSim ((configuration.DataRepository / "bof_similarity.csv").string());
	osHashSim.imbue (std::locale(""));
	osBoFSim.imbue (std::locale(""));
	osSketchHits.imbue (std::locale(""));
	osSketchHits << "Image Name;";
	osBoFSim << "Image Name;";
	osHashSim << "Image Name;";
	vector<ImageData*> images;	
	for (unsigned int i = 0; i < numberOfImages; i++)
	{
		ImageData* pImageData = pLoader->GetNextImage ();
		osSketchHits << pImageData->GetImageContext()->GetContextName() << ";";
		osBoFSim << pImageData->GetImageContext()->GetContextName() << ";";
		osHashSim << pImageData->GetImageContext()->GetContextName() << ";";
		if (!bagOfFeatures.LoadOrComputeDescriptor (*pImageData))
		{
			stringstream ss;
			ss << "Descriptor for image '" << pImageData->GetName() << "' computed!";
			Log().write (ss.str());
		}
		pImageData->ReleaseImage ();
		images.push_back(pImageData);
	}
	osSketchHits << endl;
	osBoFSim << endl;
	osHashSim << endl;

	for (unsigned int i = 0; i < numberOfImages; i++)
	{
		stringstream ss;
		ss << "similarity for image " << i << " of " << numberOfImages << ": " << images[i]->GetImageContext()->GetContextName();
		Log().write(ss.str());
		osSketchHits << images[i]->GetImageContext()->GetContextName() << ";";
		osBoFSim << images[i]->GetImageContext()->GetContextName() << ";";
		osHashSim << images[i]->GetImageContext()->GetContextName() << ";";
		for (unsigned int j = 0; j < images.size (); j++)
		{
			if (j > i)
			{
				osSketchHits << minhash::MinHashing<int>::IdenticalSketches(images[i]->GetSketches(), images[j]->GetSketches()) << ";";
				osHashSim << minhash::MinHashing<int>::Similarity (images[i]->GetSketches(), images[j]->GetSketches()) << ";";
				if (configuration.UseBag)
				{
					osBoFSim << bof::BagOfFeatures::Similarity (images[i]->GetDescriptor(), images[j]->GetDescriptor(), bagOfFeatures.GetIdfWeights()) << ";";
				}
				else
				{
					osBoFSim << bof::BagOfFeatures::SetSimilarity (images[i]->GetDescriptor(), images[j]->GetDescriptor(), bagOfFeatures.GetIdfWeights()) << ";";
				}
			}
			else
			{
				osSketchHits << ";";
				osBoFSim << ";";
				osHashSim << ";";
			}
		}
		osSketchHits << endl;
		osBoFSim << endl;
		osHashSim << endl;
	}
	osSketchHits.close ();
	osBoFSim.close ();
	osHashSim.close ();
	delete pLoader;

	return true;
}


} // namespace tasks
} // namespace cbsf
} // namespace codemm
