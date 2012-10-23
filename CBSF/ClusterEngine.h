// stdafx.h ---------------------------------------------//
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

#ifndef CBSF_CLUSTERENGINE_H_
#define CBSF_CLUSTERENGINE_H_

#include <map>
#include <rapidxml.hpp>

namespace codemm {
namespace cbsf {

class Configuration;
namespace bof { class BagOfFeatures; }
namespace core { class ImageData; }


namespace clustering {


class ImageCluster;

typedef std::pair<ImageCluster*, ImageCluster*> ClusterPair;
typedef std::pair<core::ImageData*, core::ImageData*> ImageConnection;
typedef std::pair<float, ImageConnection> MatrixEntry;
typedef std::map<ClusterPair, MatrixEntry> SimilarityMatrix;
	
class ClusterEngine
{
public:
	ClusterEngine(Configuration& configuration, const fs::path& hashRepository, float minSimilarity, int clusteringType = Type_CompleteLink);
	virtual ~ClusterEngine(void);

	void Cluster (int numClusters, std::vector<ImageCluster*>& imageClusters, std::vector<MatrixEntry>& connections);
	void WriteClusterXmlFile (const std::vector<ImageCluster*>& imageClusters, const fs::path& path) const;
	void WriteSimilarities2XmlFile (const std::vector<MatrixEntry>& connections, const fs::path& path, const std::string& imagePath = std::string(), const std::string& thumbnailPath = std::string()) const;

	static const int Type_CompleteLink = 1;
	static const int Type_SingleLink = 2;

private:
	rapidxml::xml_node<>* Cluster2Xml (const ImageCluster& imageCluster, rapidxml::xml_document<>& doc) const;
	ImageCluster* GetOrCreateImageCluster (core::ImageData* pImageData);
	void InitializeSimilarityMatrix ();
	SimilarityMatrix::iterator GetHighestSimilarityPair ();
	void CreateClusterVector (std::vector<ImageCluster*>& imageClusters);
	void RemoveCluster (ImageCluster* pImageCluster);

	Configuration& configuration;
	const fs::path& hashRepository;
	float minSimilarity;
	int clusterCount;
	int clusteringType;

	std::map<std::string,ImageCluster*> imageName2ClusterMap;
	std::vector<core::ImageData*> images;
	SimilarityMatrix similarityMatrix;
};


} // namespace clustering
} // namespace cbsf
} // namespace codemm

#endif // CBSF_CLUSTERENGINE_H_
