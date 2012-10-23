// BasicScreenFrameExtractor.cpp ---------------------------------------------//
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
#include "BasicScreenFrameExtractor.h"

#include <set>
#include <boost/filesystem/fstream.hpp>
#include <boost/filesystem/convenience.hpp>
#include <rapidxml.hpp>
#include <opencv2/highgui/highgui.hpp>

#include "ImageData.h"
#include "FileSystemUtility.h"
#include "VideoContext.h"
#include "Log.h"


using namespace std;
using namespace cv;
using namespace rapidxml;
using namespace codemm::cbsf::core;

namespace codemm {
namespace cbsf {
namespace loader {


BasicScreenFrameExtractor::BasicScreenFrameExtractor(const fs::path& videoRepository)
{
	this->videoRepository = videoRepository;
	this->VideoFileExtension = ".mp4";
	this->DataFileExtension = ".mpg.xml";
	this->videoFilePosition = 0;
}


BasicScreenFrameExtractor::~BasicScreenFrameExtractor(void)
{
}


void BasicScreenFrameExtractor::GetImages (vector<ImageData*>& images)
{
	if (this->videoFiles.size() == 0)
	{
		FileSystemUtility::GetFiles (this->videoRepository, this->VideoFileExtension, this->videoFiles);
		this->videoFilePosition = 0;
	}

	for (vector<fs::path>::iterator it = videoFiles.begin(); it != videoFiles.end(); ++it)
	{
		this->GetKeyFrames (*it, this->DataFileExtension, images);
	}
}


bool BasicScreenFrameExtractor::HasNextImage ()
{
	if (this->videoFiles.size() == 0)
	{
		FileSystemUtility::GetFiles (this->videoRepository, this->VideoFileExtension, this->videoFiles);
		this->videoFilePosition = 0;
	}

	return (this->videoFilePosition + 1) < this->videoFiles.size () || this->tmpVideoImages.size () > 0;
}


ImageData* BasicScreenFrameExtractor::GetNextImage ()
{
	if (this->videoFiles.size() == 0)
	{
		FileSystemUtility::GetFiles (this->videoRepository, this->VideoFileExtension, this->videoFiles);
		this->videoFilePosition = 0;
	}

	// load frames from next video if no tmp frames are available
	if (this->tmpVideoImages.size () == 0)
	{
		this->videoFilePosition++;
		if (this->videoFilePosition < this->videoFiles.size ())
		{
			fs::path path = this->videoFiles.at(this->videoFilePosition);
			this->GetKeyFrames (path, this->DataFileExtension, this->tmpVideoImages);
		}	
	}

	// return next frame from tmp frames
	if (this->tmpVideoImages.size () != 0)
	{
		ImageData* pImageData = this->tmpVideoImages.front ();
		this->tmpVideoImages.erase (this->tmpVideoImages.begin());
		return pImageData;
	}
	
	return NULL;
}


bool BasicScreenFrameExtractor::ExtractKeyFrames (const fs::path& videoFile, const vector<long>& keyFramePositions, vector<ImageData*>& keyFrames) const
{
	for (unsigned int i=0; i < keyFramePositions.size(); i++)
	{
		// HACK: need to open the video for every key frame as
		// only the first call of capture.set (CV_CAP_PROP_POS_FRAMES, ...) works.
		VideoCapture capture (videoFile.string());
		if (!capture.isOpened ())
		{
			stringstream ss;
			ss << "Could not open video '" << videoFile.string() << "'!";
			logging::Log().write (ss.str());
			return false;
		}
	
		long frameCount = static_cast<long> (capture.get (CV_CAP_PROP_FRAME_COUNT));
	
		if (keyFramePositions[i] >= frameCount)
		{
			logging::Log().write ("Key frame position exceeds frame count!");
			return false;
		}
		capture.set (CV_CAP_PROP_POS_FRAMES, keyFramePositions[i]);

		Mat frame;
		capture.read (frame);

		stringstream ss;
		ss << videoFile.filename().string() << " sceene " << i;
		VideoContext* pContext = new VideoContext (videoFile, keyFramePositions[i]);
		ImageData* pImageData = new ImageData (ss.str());
		pImageData->SetImageContext (pContext);
		pImageData->SetImage (frame.clone());
		keyFrames.push_back(pImageData);
	}
	
	return true;
}


void BasicScreenFrameExtractor::ReadSegmentPositions (const fs::path& file, vector<long>& segmentPositions) const
{
	assert(fs::exists(file));
	fs::ifstream xmlFile (file);
	stringstream fileContent;
	while (!xmlFile.eof())
	{
		string line;
		getline (xmlFile, line);
		fileContent << line;
	}
	
	string fileContentStr = fileContent.str();
	vector<char> xmlCopy(fileContentStr.begin(), fileContentStr.end());
    xmlCopy.push_back('\0');

	try
	{
		xml_document<> doc;
		doc.parse<0>(&xmlCopy[0]);
		xml_node<>* segmentationNode = doc.first_node("video")->first_node("segmentation");
		for (xml_node<>* segmentNode = segmentationNode->first_node(); segmentNode; segmentNode = segmentNode->next_sibling())
		{
			string lastFrameStr = segmentNode->first_attribute("lastFrame")->value();
			stringstream strs(lastFrameStr);
			long lastFrame;
			strs >> lastFrame;
			segmentPositions.push_back(lastFrame);
		}

	}
	catch (std::exception& ex)
	{
		stringstream ss;
		ss << "Unexpected error in BasicScreenFrameExtractor::ReadSegmentPositions: " << ex.what();
		logging::Log().write (ss.str());
	}
}


bool BasicScreenFrameExtractor::GetKeyFrames (const fs::path& videoFile, const string& segmentFileExtension, vector<ImageData*>& keyFrames) const
{
	fs::path segmentFile (videoFile);
	string fileName = segmentFile.filename().replace_extension().string();
	segmentFile.remove_filename();
	segmentFile = segmentFile / (fileName + segmentFileExtension);
	
	vector<long> segmentPositions;
	this->ReadSegmentPositions (segmentFile, segmentPositions);

	// calculate key frame positions
	long currentPosition = 1;
	for (unsigned int i=0; i < segmentPositions.size(); i++)
	{
		segmentPositions[i] = (segmentPositions[i] - currentPosition) / 2;
	}
	
	return this->ExtractKeyFrames (videoFile, segmentPositions, keyFrames);
}


int BasicScreenFrameExtractor::GetNumberImages ()
{
	// invalid operation
	assert (false);
	
	logging::Log().write ("BasicScreenFrameExtractor::GetNumberImages: Invalid Operation!");
	return 0;
}


} // namespace loader
} // namespace cbsf
} // namespace codemm
