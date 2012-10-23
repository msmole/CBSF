// main.cpp ---------------------------------------------//
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
#include "Configuration.h"

#include <iostream>
#include <boost/program_options.hpp>

#include "BagOfFeatures.h"
#include "MinHashUtility.h"
#include "MinHashIndex.h"
#include "InvertedFileSystem.h"
#include "ImageData.h"
#include "ClusterEngine.h"
#include "Log.h"


using namespace std;
using namespace cv;
namespace po = boost::program_options;

namespace codemm {
namespace cbsf {


Configuration::Configuration(void)
{
	this->pBagOfFeatures = NULL;
	this->pMinHashUtility = NULL;

	//this->ExecutionSequence = "e"; // codebook, minhash, evaluate, query
	// this->SourceRepository = "C:\\Users\\msmole\\Documents\\Diplomarbeit\\TestData\\VOCtrainval_11-May-2012\\VOCdevkit\\VOC2012\\JPEGImages";
	// this->TargetRepository = "C:\\Users\\msmole\\Documents\\Diplomarbeit\\TestData\\VOCtrainval_11-May-2012\\VOCdevkit\\VOC2012\\JPEGImages";
	// this->SourceRepository = "C:\\Users\\msmole\\Documents\\Diplomarbeit\\TestData\\TRECVID\\IACC.1.C";
	//this->SourceRepository = "C:\\Users\\msmole\\Documents\\Diplomarbeit\\TestData\\ukbench\\full";
	//this->TargetRepository = "C:\\Users\\msmole\\Documents\\Diplomarbeit\\TestData\\ukbench\\SURF\\repo_bag_weights";
	//this->SourceRepository = "C:\\TestData\\oxford\\images";
	//this->TargetRepository = "C:\\TestData\\oxford\\repo";
	//this->SourceRepository = "C:\\Users\\msmole\\Documents\\Diplomarbeit\\TestData\\Oxford\\images";
	//this->TargetRepository = "C:\\Users\\msmole\\Documents\\Diplomarbeit\\TestData\\Oxford\\repo";
	//this->SourceRepository = "C:\\Users\\msmole\\Documents\\Diplomarbeit\\TestData\\CodeMM\\images";
	//this->TargetRepository = "C:\\Users\\msmole\\Documents\\Diplomarbeit\\TestData\\CodeMM\\repo";
	//this->SourceRepository = "C:\\TestData\\ukbench\\full";
	//this->TargetRepository = "C:\\TestData\\repo";
	// this->TargetRepository = "";
	
	// this->DescriptorMatcher = "FlannBased";
	//this->DescriptorExtractor = "SURF";
	//this->FeatureDetector = "SURF";
	//this->DescriptorMatcher = "BruteForce-L1";
	this->DescriptorMatcher = "BruteForce";
	this->DescriptorExtractor = "SIFT";
	this->FeatureDetector = "SIFT";

	this->VisualWords = 10000;
	this->Kmeans_Attemps = 3;
	this->Kmeans_Iterations = 100;
	this->Kmeans_Epsilon = FLT_EPSILON;

	this->TrainingSetSize = Configuration::TrainingSetSize_Undefined;
	this->TestSetSize = Configuration::TestSetSize_Undefined;
	this->TestSetType = Configuration::TestSetType_Random;

	this->NumSketches = 256;
	this->SketchSize = 2;
	this->Accuracy = 10;

	this->UseImageList = false;
	this->UseWeights = true;
	this->UseBag = true;

	this->MinSketchHits = 1;
	this->IndexType = Configuration::IndexType_MinHash;
	//this->IndexType = Configuration::IndexType_InvertedFile;
	
	this->Clustering_MinSimilarity = 0.01f;
	this->Clustering_MinNeighbours = 0;
	this->Clustering_MaxNeighbours = 10;
	this->Clustering_Type = clustering::ClusterEngine::Type_SingleLink;
	this->Clustering_ImagePath = "";
	this->Clustering_ThumbnailPath = "";
	
	this->Query_NumResults = 4;
	this->Query_ResultCols = 2;
	this->Query_MaxImgSize = 320;
	this->Query_DrawKeypoints = false;
	this->Query_DrawMatches = false;
	this->Query_ROI = true;
}


Configuration::~Configuration(void)
{
	if (this->pBagOfFeatures)
	{
		delete this->pBagOfFeatures;
		this->pBagOfFeatures = NULL;
	}
	if (this->pMinHashUtility)
	{
		delete this->pMinHashUtility;
		this->pMinHashUtility = NULL;
	}
}


void Configuration::Init ()
{
	this->Source = fs::path (this->SourceRepository);
	this->Target = fs::path (this->TargetRepository);
	
	stringstream clusterFolderName;
	clusterFolderName << this->VisualWords;
	this->ClusterRepository = this->Target / clusterFolderName.str();
	
	this->VocabularyPath = this->ClusterRepository / "vocabulary.xml";	
	this->WeightsPath = this->ClusterRepository / "weights.bin";	
	this->MaxWordFrequenciesPath = this->ClusterRepository / "frequencies.bin";
	
	stringstream dataFolderName;
	dataFolderName << this->NumSketches << "_" << this->SketchSize << "_" << this->Accuracy;
	this->DataRepository = this->ClusterRepository / dataFolderName.str();
	this->HashRepository = this->DataRepository / "mh";
}


bool Configuration::Load (int argc, char* argv[])
{
	po::options_description desc("Supported options");
	desc.add_options()
		("help,h", "produce help message")
		("mode,m", po::value<string>(&this->ExecutionSequence)->required(), "mode of execution (c .. calculate codebook, m .. calculate min hashes, e .. evaluate, q .. query)")
		("source,s", po::value<string>(&this->SourceRepository)->required(), "path to source repository of images")
		("target,t", po::value<string>(&this->TargetRepository), "path to target repository (hashes, weights, etc.)")
		("feature-detector,f", po::value<string>(&this->FeatureDetector), "feature detector")
		("desc-extractor,e", po::value<string>(&this->DescriptorExtractor), "descriptor extractor")
		("desc-matcher,d", po::value<string>(&this->DescriptorMatcher), "descriptor matcher")
		("visual-words,w", po::value<int>(&this->VisualWords), "number of visual words")
		("kmeans-attemps", po::value<int>(&this->Kmeans_Attemps), "number of times k-means will run to get the best result")
		("kmeans-iterations", po::value<int>(&this->Kmeans_Iterations), "max number of iterations for k-means [2, 100]")
		("kmeans-epsilon", po::value<float>(&this->Kmeans_Epsilon), "min shift of cluster centers for k-means. default: FLT_EPSILON")
		("test-set-size,b", po::value<int>(&this->TestSetSize), "size of the training set, -1 for all images")
		("test-set-type", po::value<int>(&this->TestSetType), "method for selecting test images: -1 ... random, [positive number] ... start position in dataset")
		("training-set-size,r", po::value<int>(&this->TrainingSetSize), "size of the training set, -1 for all images")
		("sketches,k", po::value<int>(&this->NumSketches), "number of sketches")
		("sketch-size,n", po::value<int>(&this->SketchSize), "sketch size")
		("accuracy,a", po::value<int>(&this->Accuracy), "histogram intersection accuracy (defines the maximum value of frequency considered)")
		("use-list", po::value<bool>(&this->UseImageList), "if set creates/uses image list in target repo for training")
		("use-weights", po::value<bool>(&this->UseWeights), "if set uses weights for calculating similarity and calculating the hash functions")
		("use-bag", po::value<bool>(&this->UseBag), "if set uses the bag of features approach instead of set of features")
		("index-type", po::value<int>(&this->IndexType), "type of index structure to use: 1 .. min-Hash index, 2 .. inverted file index")
		("min-hits", po::value<int>(&this->MinSketchHits), "minimum number of sketch hits to be considered by quering the repository")
		("c-min-sim", po::value<float>(&this->Clustering_MinSimilarity), "minimum similarity for results to be used in clustering")
		("c-min-res", po::value<int>(&this->Clustering_MinNeighbours), "minimum results considered for similarity matrix for every image")
		("c-max-res", po::value<int>(&this->Clustering_MaxNeighbours), "maximum considered results returned from one query when initializing the similarity matrix for the clustering process")
		("c-type", po::value<int>(&this->Clustering_Type), "type of clustering. 1 ... complete link, 2 ... single link")
		("c-img-path", po::value<string>(&this->Clustering_ImagePath), "path to the images")
		("c-thumb-path", po::value<string>(&this->Clustering_ThumbnailPath), "path to thumbnails")
		("q-num-results", po::value<int>(&this->Query_NumResults), "number of results from the query")
		("q-res-cols", po::value<int>(&this->Query_ResultCols), "number of columns to display the result")
		("q-max-imgsize", po::value<int>(&this->Query_MaxImgSize), "maximum size of the image (width and height)")
		("q-keypoints", po::value<bool>(&this->Query_DrawKeypoints), "draw keypoints in result images")
		("q-matches", po::value<bool>(&this->Query_DrawMatches), "draw matches from query to result images")
		("q-roi", po::value<bool>(&this->Query_ROI), "select region of interest before querying")
	;

	po::variables_map vm;
	po::store(po::parse_command_line(argc, argv, desc), vm);
	try
	{
		po::store(po::parse_config_file<char>("config.cfg", desc), vm);
	}
	catch (std::exception& e)
	{
		cout << "ERROR: Error while parsing config file: " << e.what() << endl;
		return false;
	}

	if (vm.count("help"))
	{
		desc.print(cout);
		return false;
	}

	try
	{
		po::notify(vm);
	}
	catch(std::exception& e)
    {
        cerr << "Error: " << e.what() << "\n";
		desc.print(cerr);
		return false;
    }

	if (this->TargetRepository.empty())
	{
		this->TargetRepository = this->SourceRepository;
	}

	return true;
}


string Configuration::ToString () const
{
	stringstream ss;
	ss << "executionSequence: " << this->ExecutionSequence << endl;
	ss << "source: " << this->SourceRepository << endl;
	ss << "target: " << this->TargetRepository << endl;
	ss << "visualWords: " << this->VisualWords << endl;
	ss << "kmeans attemps: " << this->Kmeans_Attemps << endl;
	ss << "kmeans iterations: " << this->Kmeans_Iterations << endl;
	ss << "kmeans epsilon: " << this->Kmeans_Epsilon << endl;
	ss << "descriptorMatcher: " << this->DescriptorMatcher << endl;
	ss << "descriptorExtractor: " << this->DescriptorExtractor << endl;
	ss << "featureDetector: " << this->FeatureDetector << endl;
	ss << "trainingSetSize: " << this->TrainingSetSize << endl;
	ss << "testSetSize: " << this->TestSetSize << endl;
	ss << "numSketches: " << this->NumSketches << endl;
	ss << "sketchSize: " << this->SketchSize << endl;
	ss << "accuracy: " << this->Accuracy << endl;
	ss << "useImageList: " << (this->UseImageList ? "true" : "false") << endl;
	ss << "useWeights: " << (this->UseWeights ? "true" : "false") << endl;
	ss << "useBag: " << (this->UseBag ? "true" : "false") << endl;
	ss << "minSketchHits: " << this->MinSketchHits << endl;
	ss << "index: ";
	if (this->IndexType == Configuration::IndexType_InvertedFile) ss << "inverted file system" << endl;
	else if (this->IndexType == Configuration::IndexType_MinHash) ss << "min-Hash" << endl;
	ss << "use roi: " << (this->Query_ROI ? "true" : "false") << endl;
	ss << "clustering type: ";
	if (this->Clustering_Type == clustering::ClusterEngine::Type_SingleLink) ss << "single link" << endl;
	else if (this->Clustering_Type == clustering::ClusterEngine::Type_CompleteLink) ss << "complete link" << endl;
	ss << "clustering min similarity: " << this->Clustering_MinSimilarity << endl;
	ss << "clustering min neighbours: " << this->Clustering_MinNeighbours << endl;
	ss << "clustering max neighbours: " << this->Clustering_MaxNeighbours;
	return ss.str();
}


bof::BagOfFeatures& Configuration::GetBagOfFeatures ()
{
	using bof::BagOfFeatures;
	using namespace logging;

	if (!this->pBagOfFeatures)
	{
		Ptr<cv::DescriptorMatcher> matcher = cv::DescriptorMatcher::create(this->DescriptorMatcher);
		if (matcher == NULL)
		{
			stringstream ss;
			ss << "ERROR: The given descriptor matcher '" << this->DescriptorMatcher << "' could not be created!";
			Log().write (ss.str());
			exit(-1);
		}

		Ptr<cv::DescriptorExtractor> extractor = cv::DescriptorExtractor::create(this->DescriptorExtractor);
		if (extractor == NULL)
		{
			stringstream ss;
			ss << "ERROR: The given descriptor extractor '" << this->DescriptorExtractor << "' could not be created!";
			Log().write (ss.str());
			exit(-1);
		}

		Ptr<cv::FeatureDetector> detector = cv::FeatureDetector::create(this->FeatureDetector);
		if (detector == NULL)
		{
			stringstream ss;
			ss << "ERROR: The given descriptor detector '" << this->FeatureDetector << "' could not be created!";
			Log().write (ss.str());
			exit(-1);
		}
		
		this->pBagOfFeatures = new BagOfFeatures (detector, extractor, matcher);
	}
	return *this->pBagOfFeatures;
}


bof::BagOfFeatures& Configuration::GetAndLoadBagOfFeatures ()
{
	using bof::BagOfFeatures;
	using namespace logging;

	BagOfFeatures& bagOfFeatures = this->GetBagOfFeatures ();
	if (!bagOfFeatures.IsVocabularyLoaded())
	{
		if (!bagOfFeatures.ReadVocabulary (this->VocabularyPath))
		{
			stringstream ss;
			ss << "WARNING: Vocabulary could not be loaded: " << this->VocabularyPath.string();
			Log().write (ss.str());
		}

		if (this->UseWeights)
		{
			if (!bagOfFeatures.ReadWeights (this->WeightsPath))
			{
				stringstream ss;
				ss << "WARNING: Weights could not be loaded: " << this->WeightsPath.string();
				Log().write (ss.str());
			}
		}
	}
	return bagOfFeatures;
}


minhash::MinHashUtility& Configuration::GetMinHashUtility ()
{
	if (!this->pMinHashUtility)
	{
		this->pMinHashUtility = new minhash::MinHashUtility (*this, this->GetAndLoadBagOfFeatures().GetIdfWeights());
	}
	return *this->pMinHashUtility;
}


core::IIndex* Configuration::CreateIndex ()
{
	if (this->IndexType == Configuration::IndexType_InvertedFile)
	{
		bof::BagOfFeatures& bagOfFeatures = this->GetAndLoadBagOfFeatures();
		return new bof::InvertedFileSystem (bagOfFeatures, bagOfFeatures.GetIdfWeights(), this->UseBag);
	}
	else if (this->IndexType == Configuration::IndexType_MinHash)
	{
		return new minhash::MinHashIndex (this->GetMinHashUtility(), this->NumSketches, this->MinSketchHits);
	}
	throw new std::exception ("invalid value of index type");
}


} // namespace cbsf
} // namespace codemm
