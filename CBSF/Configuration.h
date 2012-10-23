// Configuration.h ---------------------------------------------//
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

#ifndef CBSF_CONFIGURATION_H_
#define CBSF_CONFIGURATION_H_


namespace codemm {
namespace cbsf {

namespace bof { class BagOfFeatures; }
namespace core { class IIndex; }
namespace minhash { class MinHashUtility; }

class Configuration
{
public:
	Configuration(void);
	virtual ~Configuration(void);

	bool Load (int argc, char* argv[]);
	void Init ();
	std::string ToString () const;

	core::IIndex* CreateIndex ();
	bof::BagOfFeatures& GetBagOfFeatures ();
	bof::BagOfFeatures& GetAndLoadBagOfFeatures ();
	minhash::MinHashUtility& GetMinHashUtility ();

	std::string ExecutionSequence;
	std::string SourceRepository;
	std::string TargetRepository;

	std::string DescriptorMatcher;
	std::string DescriptorExtractor;
	std::string FeatureDetector;

	int VisualWords;
	int Kmeans_Attemps;
	int Kmeans_Iterations;
	float Kmeans_Epsilon;

	int TrainingSetSize;
	int TestSetSize;
	int TestSetType;
	int NumSketches;
	int SketchSize;
	int Accuracy;

	bool UseImageList;
	bool UseWeights;
	bool UseBag;

	int IndexType;
	int MinSketchHits;
	
	float Clustering_MinSimilarity;
	int Clustering_MaxNeighbours;
	int Clustering_MinNeighbours;
	int Clustering_Type;
	std::string Clustering_ImagePath;
	std::string Clustering_ThumbnailPath;

	int Query_NumResults;
	int Query_ResultCols;
	int Query_MaxImgSize;
	bool Query_DrawKeypoints;
	bool Query_DrawMatches;
	bool Query_ROI;

	fs::path Source;
	fs::path Target;
	fs::path ClusterRepository;
	fs::path VocabularyPath;
	fs::path WeightsPath;
	fs::path MaxWordFrequenciesPath;
	fs::path DataRepository;
	fs::path HashRepository;

	static const int IndexType_MinHash = 1;
	static const int IndexType_InvertedFile = 2;

	static const int TrainingSetSize_Undefined = -1;
	static const int TestSetSize_Undefined = -1;
	static const int TestSetType_Random = -1;
	static const int Clustering_NeighboursUndefined = -1;

	static const char ExecutionSeq_CalculateVocabulary = 'c';
	static const char ExecutionSeq_CalculateWeights = 'w';
	static const char ExecutionSeq_MinHash = 'm';
	static const char ExecutionSeq_ExtractFrames = 'x';
	static const char ExecutionSeq_RunEvaluation = 'e';
	static const char ExecutionSeq_QueryMode = 'q';
	static const char ExecutionSeq_UKBenchScore = 'u';
	static const char ExecutionSeq_OxfordBenchmark = 'o';
	static const char ExecutionSeq_OxfordBenchmarkBof = 'p';
	static const char ExecutionSeq_UKBenchScoreBof = 'r';
	static const char ExecutionSeq_Cluster = 'l';

private:
	bof::BagOfFeatures* pBagOfFeatures;
	minhash::MinHashUtility* pMinHashUtility;
};

} // namespace cbsf
} // namespace codemm

#endif // CBSF_CONFIGURATION_H_
