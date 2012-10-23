// ClusteringTask.cpp ---------------------------------------------//
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
#include "ClusteringTask.h"

#include "Configuration.h"
#include "BagOfFeatures.h"
#include "ImageCluster.h"
#include "ClusterEngine.h"
#include "ImageData.h"
#include "Log.h"


using namespace std;

namespace codemm {
namespace cbsf {
namespace tasks {


ClusteringTask::ClusteringTask(void)
{
}


ClusteringTask::~ClusteringTask(void)
{
}


bool ClusteringTask::Execute (Configuration& configuration)
{
	using namespace logging;
	using namespace clustering;

	vector<ImageCluster*> clusters;
	vector<MatrixEntry> strongConnections;
	ClusterEngine clusterEngine (configuration, configuration.HashRepository, configuration.Clustering_MinSimilarity, configuration.Clustering_Type);
	clusterEngine.Cluster (10, clusters, strongConnections);

	Log().write ("Writing cluster layout file ... ");
	fs::path xmlDoc = configuration.DataRepository / "cluster_layout.xml";
	clusterEngine.WriteSimilarities2XmlFile (strongConnections, xmlDoc, configuration.Clustering_ImagePath, configuration.Clustering_ThumbnailPath);

	return true;
}


} // namespace tasks
} // namespace cbsf
} // namespace codemm
