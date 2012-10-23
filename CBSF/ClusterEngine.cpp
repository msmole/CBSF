// ClusterEngine.cpp ---------------------------------------------//
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
#include "ClusterEngine.h"

#include <set>
#include <rapidxml_print.hpp>
#include <boost\algorithm\string\predicate.hpp>
#include <opencv2\core\core.hpp>
#include <opencv2\features2d\features2d.hpp>

#include "Configuration.h"
#include "ImageData.h"
#include "ImageCluster.h"
#include "BagOfFeatures.h"
#include "BasicFSImageLoader.h"
#include "IIndex.h"
#include "Log.h"


using namespace std;

namespace codemm {
namespace cbsf {
namespace clustering {


ClusterEngine::ClusterEngine(Configuration& configuration, const fs::path& hashRepository, float minSimilarity, int clusteringType /*= Type_CompleteLink*/)
	: configuration(configuration), hashRepository(hashRepository), minSimilarity(minSimilarity)
{
	this->clusteringType = clusteringType;
}


ClusterEngine::~ClusterEngine(void)
{
}


void ClusterEngine::WriteClusterXmlFile (const vector<ImageCluster*>& imageClusters, const fs::path& path) const
{
	using namespace rapidxml;

	xml_document<> doc;
	xml_node<>* pDecl = doc.allocate_node(node_declaration);
    pDecl->append_attribute(doc.allocate_attribute("version", "1.0"));
    pDecl->append_attribute(doc.allocate_attribute("encoding", "UTF-8"));
    doc.append_node(pDecl); 

	xml_node<>* pClusters = doc.allocate_node(node_element, "Clusters");
    doc.append_node(pClusters);

	for (vector<ImageCluster*>::const_iterator it = imageClusters.cbegin(); it != imageClusters.cend (); ++it)
	{
		xml_node<>* pCluster = this->Cluster2Xml (*(*it), doc);
		pClusters->append_node(pCluster);
	}

    ofstream file;
    file.open (path.string ());
    file << doc;
	file.close ();
}


rapidxml::xml_node<>* ClusterEngine::Cluster2Xml (const ImageCluster& imageCluster, rapidxml::xml_document<>& doc) const
{
	using namespace rapidxml;
	using core::ImageData;

	xml_node<>* pCluster = doc.allocate_node(node_element, "Cluster");
	xml_node<>* pImages = doc.allocate_node(node_element, "Images");
	pCluster->append_node (pImages);

	const vector<ImageData*>& images = imageCluster.GetImages();
	for (vector<ImageData*>::const_iterator it = images.cbegin(); it != images.cend(); ++it)
	{
		xml_node<>* pImage = doc.allocate_node(node_element, "Image");
		pImages->append_node (pImage);

		xml_attribute<>* pImageNameAttr = doc.allocate_attribute("id", (*it)->GetName().c_str());
		pImage->append_attribute(pImageNameAttr);

		fs::path path = (*it)->GetImageContext()->GetParentPath() / (*it)->GetImageContext()->GetContextName();
		char* pStr = doc.allocate_string (path.string().c_str());
		xml_attribute<>* pPathAttr = doc.allocate_attribute("path", pStr);
		pImage->append_attribute(pPathAttr);
	}

	if (imageCluster.GetLeftTree())
	{
		xml_node<>* pLeft = doc.allocate_node(node_element, "Left");
		pCluster->append_node (pLeft);
		pLeft->append_node (this->Cluster2Xml (*(imageCluster.GetLeftTree()), doc));

	}
	if (imageCluster.GetRightTree())
	{
		xml_node<>* pRight = doc.allocate_node(node_element, "Right");
		pCluster->append_node (pRight);
		pRight->append_node (this->Cluster2Xml (*(imageCluster.GetRightTree()), doc));
	}

	return pCluster;
}


void ClusterEngine::WriteSimilarities2XmlFile (const vector<MatrixEntry>& connections, const fs::path& path, const string& imagePath /*= string()*/, const string& thumbnailPath /*= string()*/) const
{
	using namespace rapidxml;
	using core::ImageData;

	xml_document<> doc;
	xml_node<>* pDecl = doc.allocate_node(node_declaration);
    pDecl->append_attribute(doc.allocate_attribute("version", "1.0"));
    pDecl->append_attribute(doc.allocate_attribute("encoding", "UTF-8"));
    doc.append_node(pDecl); 

	xml_node<>* pRoot = doc.allocate_node(node_element, "Root");
	doc.append_node (pRoot);
	xml_node<>* pImages = doc.allocate_node(node_element, "Images");
	pRoot->append_node (pImages);
	for (vector<ImageData*>::const_iterator it = this->images.cbegin(); it != this->images.cend(); ++it)
	{
		string imageNodeName = "Image";
		if (boost::starts_with (imagePath, "http"))
		{
			imageNodeName = "RemoteImage";
		}
		xml_node<>* pImage = doc.allocate_node(node_element, imageNodeName.c_str());
		pImages->append_node (pImage);

		xml_attribute<>* pImageNameAttr = doc.allocate_attribute("id", (*it)->GetName().c_str());
		pImage->append_attribute(pImageNameAttr);

		char* pStr = NULL;
		if (imagePath.size() == 0)
		{
			fs::path path = (*it)->GetImageContext()->GetParentPath() / (*it)->GetImageContext()->GetContextName();
			pStr = doc.allocate_string (path.string().c_str());
		}
		else
		{
			stringstream ss;
			ss << imagePath << (*it)->GetImageContext()->GetContextName();
			pStr = doc.allocate_string (ss.str().c_str());
		}
		xml_attribute<>* pThumbnailPathAttr = doc.allocate_attribute("thumbnailPath", pStr);
		pImage->append_attribute(pThumbnailPathAttr);

		stringstream ss;
		if (imagePath.size() != 0 && thumbnailPath.size() != 0)
		{
			ss << thumbnailPath << (*it)->GetImageContext()->GetContextName();
		}

		pStr = doc.allocate_string (ss.str().c_str());

		xml_attribute<>* pPathAttr = doc.allocate_attribute("path", pStr);
		pImage->append_attribute(pPathAttr);
	}

	xml_node<>* pConnections = doc.allocate_node(node_element, "Similarities");
	pRoot->append_node (pConnections);
	for (vector<MatrixEntry>::const_iterator it = connections.cbegin(); it != connections.cend(); ++it)
	{
		float value = it->first;
		const ImageConnection& imgConn = it->second;
		
		xml_node<>* pConnection = doc.allocate_node(node_element, "Similarity");
		pConnections->append_node (pConnection);
				
		xml_attribute<>* pFromAttr = doc.allocate_attribute("id1", imgConn.first->GetName().c_str());
		pConnection->append_attribute(pFromAttr);

		xml_attribute<>* pToAttr = doc.allocate_attribute("id2", imgConn.second->GetName().c_str());
		pConnection->append_attribute(pToAttr);

		stringstream valueStream;
		valueStream << value;
		char* pStr = doc.allocate_string (valueStream.str().c_str());
		xml_attribute<>* pValueAttr = doc.allocate_attribute("value", pStr);
		pConnection->append_attribute(pValueAttr);
	}

	ofstream file;
    file.open (path.string ());
    file << doc;
	file.close ();
}


void ClusterEngine::Cluster (int numClusters, vector<ImageCluster*>& imageClusters, vector<MatrixEntry>& strongConnections)
{
	using namespace logging;
	using core::ImageData;

	Log().write ("Start clustering ...");
	this->InitializeSimilarityMatrix ();

	stringstream ss;
	int iterationCount = 0;
	while (this->clusterCount > numClusters && this->similarityMatrix.size() > 0)
	{
		iterationCount ++;
		this->clusterCount --;

		ss.str("");
		ss << "Iteration no. " << iterationCount << " ...";
		Log().write (ss.str());

		SimilarityMatrix::iterator maxEntryIt = this->GetHighestSimilarityPair ();
		MatrixEntry maxEntry = maxEntryIt->second;
		ClusterPair maxSimPair = maxEntryIt->first;
		strongConnections.push_back (maxEntry);
		
		// updateVec will contain all pairs from the similarity matrix that contain the first item from the found key as this will stay
		// for all those pairs we need to update the similarity to take the highest value
		vector<SimilarityMatrix::iterator> updateVec;

		// erasedElems will contain all pairs from the similarity matrix that contain the second item from the found key
		// those pairs are removed from the matrix but will be needed to update the items stored in updateVec
		vector<pair<ClusterPair,MatrixEntry>> erasedElems;
		for (SimilarityMatrix::iterator it = this->similarityMatrix.begin(); it != this->similarityMatrix.end();)
		{
			if (it->first.first == maxSimPair.first && it->first.second == maxSimPair.second)
			{
				SimilarityMatrix::iterator eraseIt = it;
				++it;
				this->similarityMatrix.erase (eraseIt);
			}
			else if (it->first.first == maxSimPair.second || it->first.second == maxSimPair.second)
			{
				// element will be erased ... need to store it in order to update new cluster similarity
				erasedElems.push_back(*it);
				SimilarityMatrix::iterator eraseIt = it;
				++it;
				this->similarityMatrix.erase (eraseIt);
			}
			else if (it->first.first == maxSimPair.first || it->first.second == maxSimPair.first)
			{
				// elements that need to be updated
				updateVec.push_back(it);
				++it;
			}
			else
			{
				++it;
			}
		}

	
		// update the similarity of elements in updateVec
		for (unsigned int i = 0; i < updateVec.size(); ++i)
		{
			for (unsigned int j = 0; j < erasedElems.size(); ++j)
			{
				if (updateVec[i]->first.first == erasedElems[j].first.first)
				{
					MatrixEntry& updateEntry = updateVec[i]->second;
					const MatrixEntry& erasedEntry = erasedElems[j].second;
					if (this->clusteringType == ClusterEngine::Type_SingleLink && updateEntry.first < erasedEntry.first)		
					{
						// single link ... take the higher value of both
						updateEntry.first = erasedEntry.first;
						updateEntry.second = erasedEntry.second;
					}
					else if (this->clusteringType == ClusterEngine::Type_CompleteLink)
					{
						// complete link ... take the smaller similarity value of both, but link the two closest images
						if (updateEntry.first > erasedEntry.first)
						{
							updateEntry.first = erasedEntry.first;
						}
						else
						{
							updateEntry.second = erasedEntry.second;
						}
					}
					break;
				}
			}
		}

		this->RemoveCluster(maxSimPair.second);
		maxSimPair.first->Merge (maxSimPair.second);
	}

	Log().write ("Clustering finished!");

	this->CreateClusterVector (imageClusters);
}

void ClusterEngine::RemoveCluster (ImageCluster* pImageCluster)
{
	map<string,ImageCluster*>::iterator it = this->imageName2ClusterMap.find(pImageCluster->GetId());
	assert (it != this->imageName2ClusterMap.end());
	this->imageName2ClusterMap.erase (it);
}


void ClusterEngine::CreateClusterVector (vector<ImageCluster*>& imageClusters)
{
	imageClusters.clear ();
	for (map<string,ImageCluster*>::iterator it = this->imageName2ClusterMap.begin(); it != this->imageName2ClusterMap.end(); ++it)
	{
		imageClusters.push_back(it->second);
	}
}


SimilarityMatrix::iterator ClusterEngine::GetHighestSimilarityPair ()
{
	// find pair with highest similarity
	SimilarityMatrix::iterator itMax = this->similarityMatrix.begin();
	SimilarityMatrix::iterator it = this->similarityMatrix.begin();
	const MatrixEntry& initialEntry = it->second;
	float maxVal = initialEntry.first;
	++it;
	for (; it != this->similarityMatrix.end(); ++it)
	{
		const MatrixEntry& entry = it->second;
		if (entry.first > maxVal)
		{
			maxVal = entry.first;
			itMax = it;
		}
	}

	return itMax;
}


void ClusterEngine::InitializeSimilarityMatrix ()
{
	using namespace logging;
	using core::ImageData;

	this->imageName2ClusterMap.clear();
	this->similarityMatrix.clear();
	this->clusterCount = 0;

	Log().write ("Initializing index ...");
	loader::BasicFSImageLoader loader (this->configuration.Source, this->hashRepository);
	IIndex* pIndex = this->configuration.CreateIndex ();
	pIndex->Init(loader);

	loader.Reset ();
	Log().write ("Start creating initial similarity matrix ...");
	stringstream ss;
	for (int i = 0; loader.HasNextImage (); ++i)
	{
		ImageData* pImageData = loader.GetNextImage ();
		pImageData->LoadSketches ();
		ImageCluster* pImageCluster = this->GetOrCreateImageCluster (pImageData);

		ss.str("");
		ss << "Query for '" << pImageData->GetName() << "' ...";
		Log().write (ss.str ());
		
		multimap<float, ImageData*> rankedResult;
		pIndex->Find (*pImageData, rankedResult);

		ss.str("");
		ss << "Got " << rankedResult.size() << " results.";
		Log().write (ss.str ());
		
		int resultCount = 0;
		for (multimap<float, ImageData*>::reverse_iterator it = rankedResult.rbegin(); it != rankedResult.rend() && resultCount < configuration.Clustering_MaxNeighbours; ++it)
		{
			// ignore result of same returned image, check if min neighbours already reached ... if yes check min similarity
			if (it->second->GetName() != pImageData->GetName() && it->first < 1.0f && (it->first > this->minSimilarity || resultCount < configuration.Clustering_MinNeighbours))
			{
				ImageData* pResultImage = it->second->Clone(); // need to clone image data here ... the returned ImageData will be deleted together with the index.
				ImageCluster* pResultCluster = this->GetOrCreateImageCluster (pResultImage);
				ClusterPair* pKey = NULL;
				if ((*pImageCluster) < (*pResultCluster))
				{
					pKey = new ClusterPair (pImageCluster, pResultCluster);
				}
				else
				{
					pKey = new ClusterPair (pResultCluster, pImageCluster);
				}

				SimilarityMatrix::iterator simIt = similarityMatrix.find (*pKey);
				if (simIt == similarityMatrix.end())
				{
					similarityMatrix.insert (SimilarityMatrix::value_type (*pKey, MatrixEntry(it->first, ImageConnection(pImageData, pResultImage))));
				}

				++resultCount;
				delete pKey;
			}
		}

		ss.str("");
		ss << "Using " << resultCount << " results.";
		Log().write (ss.str ());

		images.push_back(pImageData);
	}
	delete pIndex;
	Log().write ("Initial similarity matrix done.");
}


ImageCluster* ClusterEngine::GetOrCreateImageCluster (core::ImageData* pImageData)
{
	ImageCluster* pCluster = NULL;
	map<string,ImageCluster*>::iterator clusterIt = this->imageName2ClusterMap.find(pImageData->GetName());
	if (clusterIt == this->imageName2ClusterMap.end())
	{
		pCluster = ImageCluster::Create (pImageData);
		this->imageName2ClusterMap.insert(map<string,ImageCluster*>::value_type(pImageData->GetName(), pCluster));
		this->clusterCount ++;
	}
	else
	{
		pCluster = clusterIt->second;
	}
	return pCluster;
}


} // namespace clustering
} // namespace cbsf
} // namespace codemm