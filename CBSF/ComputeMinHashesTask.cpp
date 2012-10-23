// ComputeMinHashesTask.cpp ---------------------------------------------//
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
#include "ComputeMinHashesTask.h"

#include "Configuration.h"
#include "ListImageLoader.h"
#include "BasicFSImageLoader.h"
#include "RandomImageLoader.h"
#include "BagOfFeatures.h"
#include "CreateRandomImageListTask.h"
#include "MinHashing.h"
#include "HistogramMinHashing.h"
#include "WeightedHistogramHFGenDecorator.h"
#include "WeightedHFGenDecorator.h"
#include "ImageData.h"
#include "IImageLoader.h"
#include "MinHashUtility.h"
#include "Log.h"


using namespace std;

namespace codemm {
namespace cbsf {
namespace tasks {


ComputeMinHashesTask::ComputeMinHashesTask(void)
{
}


ComputeMinHashesTask::~ComputeMinHashesTask(void)
{
	if (this->pLoader)
	{
		delete this->pLoader;
		this->pLoader = NULL;
	}
}


void ComputeMinHashesTask::SetImageLoader (loader::IImageLoader* pLoader)
{
	assert (pLoader);
	this->pLoader = pLoader;
}


bool ComputeMinHashesTask::Execute (Configuration& configuration)
{
	using namespace logging;
	using core::ImageData;
	using namespace minhash;

	assert (this->pLoader);

	Log().write ("create min hashes ...");

	if (!fs::exists (configuration.HashRepository))
	{
		fs::create_directories (configuration.HashRepository);
	}

	Log().write ("read vocabulary and weights ...");
	bof::BagOfFeatures& bagOfFeatures = configuration.GetAndLoadBagOfFeatures ();
	if (!bagOfFeatures.IsVocabularyLoaded())
	{
		Log().write("No vocabulary available! A vocabulary must exist before min hashes can be calculated!");
		return false;
	}

	Log().write("initializing min-hash ...");
	MinHashUtility& minHashUtility = configuration.GetMinHashUtility ();
	minHashUtility.Init ();	
	
	Log().write("calculate and save min hashes ...");
	while (this->pLoader->HasNextImage ())
	{
		ImageData* pImageData = this->pLoader->GetNextImage ();
		if (!fs::exists (pImageData->GetHashPath()))
		{
			if (!bagOfFeatures.LoadOrComputeDescriptor (*pImageData))
			{
				stringstream ss;
				ss << "Descriptor for image '" << pImageData->GetName() << "' computed!";
				Log().write (ss.str());
			}
			pImageData->ReleaseImage ();

			if (pImageData->GetDescriptor().cols > 0)
			{
				minHashUtility.CalculateMinHash (*pImageData);
				pImageData->SaveSketches ();
				stringstream ss;
				ss << pImageData->GetName() << ": min-hash calculated!";
				Log().write (ss.str());
			}
			else
			{
				stringstream ss;
				ss << pImageData->GetName() << " has no descriptor!";
				Log().write (ss.str());
			}
		}
		delete pImageData;
	}

	return true;
}


} // namespace tasks
} // namespace cbsf
} // namespace codemm
