// TaskFactory.cpp ---------------------------------------------//
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
#include "TaskFactory.h"

#include "Configuration.h"
#include "IImageLoader.h"
#include "BasicFSImageLoader.h"
#include "RandomImageLoader.h"
#include "ITaskItem.h"
#include "KeyFrameExtractionTask.h"
#include "ComputeMinHashesTask.h"
#include "ComputeVocabularyTask.h"
#include "ComputeWeightsTask.h"
#include "ClusteringTask.h"
#include "CreateRandomImageListTask.h"
#include "UKBenchScoreTask.h"
#include "OxfordBuildingTask.h"
#include "OxfordBuildingBofTask.h"
#include "QueryTask.h"
#include "UKBenchScoreBofTask.h"
#include "RunEvaluationTask.h"


using namespace std;

namespace codemm {
namespace cbsf {
namespace tasks {


TaskFactory::TaskFactory(void)
{
}


TaskFactory::~TaskFactory(void)
{
}


ITaskItem* TaskFactory::CreateTaskItem (char id, const Configuration& configuration)
{
	if (id == Configuration::ExecutionSeq_CalculateVocabulary)
	{
		return new ComputeVocabularyTask ();
	}
	else if (id == Configuration::ExecutionSeq_CalculateWeights)
	{
		return new ComputeWeightsTask ();
	}
	else if (id == Configuration::ExecutionSeq_MinHash)
	{
		loader::IImageLoader* pImageLoader = NULL;
		pImageLoader = new loader::BasicFSImageLoader (configuration.Source, configuration.HashRepository);
		if (configuration.TestSetSize != Configuration::TestSetSize_Undefined)
		{
			pImageLoader = new loader::RandomImageLoader (pImageLoader, configuration.TestSetSize);
		}
		ComputeMinHashesTask* pTask = new ComputeMinHashesTask ();
		pTask->SetImageLoader (pImageLoader);		
		return pTask;
	}
	else if (id == Configuration::ExecutionSeq_ExtractFrames)
	{
		return new KeyFrameExtractionTask ();
	}
	else if (id == Configuration::ExecutionSeq_RunEvaluation)
	{
		return new RunEvaluationTask ();
	}
	else if (id == Configuration::ExecutionSeq_QueryMode)
	{
		return new QueryTask ();
	}
	else if (id == Configuration::ExecutionSeq_UKBenchScore)
	{
		return new UKBenchScoreTask ();
	}
	else if (id == Configuration::ExecutionSeq_UKBenchScoreBof)
	{
		return new UKBenchScoreBofTask ();
	}
	else if (id == Configuration::ExecutionSeq_Cluster)
	{
		return new ClusteringTask ();
	}
	else if (id == Configuration::ExecutionSeq_OxfordBenchmark)
	{
		return new OxfordBuildingTask ();
	}
	else if (id == Configuration::ExecutionSeq_OxfordBenchmarkBof)
	{
		return new OxfordBuildingBofTask ();
	}
	return NULL;
}


} // namespace tasks
} // namespace cbsf
} // namespace codemm
