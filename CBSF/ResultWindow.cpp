// ResultWindow.cpp ---------------------------------------------//
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
#include "ResultWindow.h"

#include <opencv2\highgui\highgui.hpp>
#include <opencv2\imgproc\imgproc.hpp>

#include "BagOfFeatures.h"
#include "Configuration.h"


using namespace std;
using namespace cv;

namespace codemm {
namespace cbsf {
namespace ui {


ResultWindow::ResultWindow(string ID, Configuration& configuration)
	: ImageWindow(ID), configuration(configuration)
{
	this->MaxWidth = 160;
	this->MaxHeight = 160;
	this->BorderSize = 2;
	this->NumColumns = 3;
	this->NumMatches = 25;
	this->DrawKeypoints = false;
	this->DrawMatches = true;
}


ResultWindow::~ResultWindow(void)
{
}


void ResultWindow::ShowResult(Mat queryImage, vector<string> imagePaths)
{
	int width = 0, height = 0;
	vector<Mat> images;
	for (vector<string>::iterator it = imagePaths.begin(); it != imagePaths.end(); ++it)
	{
		//create a new image pointer to point to the source image
		Mat image = this->LoadImage (queryImage, *it);
		int tmpWidth = image.cols;
		int tmpHeight = image.rows;
		if (tmpWidth > this->MaxWidth || tmpHeight > this->MaxHeight)
		{
			float ratioWidth = ((float) this->MaxWidth) / tmpWidth;
			float ratioHeight = ((float) this->MaxHeight) / tmpHeight;
			float ratio = min (ratioWidth, ratioHeight);
			tmpWidth = (int) (tmpWidth * ratio);
			tmpHeight = (int) (tmpHeight * ratio);
		}
		resize (image, image, Size(tmpWidth, tmpHeight));
		images.push_back (image);

		if (tmpWidth > width)
		{
			width = tmpWidth;
		}
		if (tmpHeight > height)
		{
			height = tmpHeight;
		}
	}

	if (images.size() > 0)
	{
		int rows = (int) ceil(((float) images.size()) / ((float) this->NumColumns));
		Mat newImg = Mat::zeros (Size((width + 2 * this->BorderSize) * this->NumColumns, (height + 2 * this->BorderSize) * rows), images.at(0).type());

		int offsetX = 0;
		int offsetY = 0;
		for (vector<Mat>::iterator it = images.begin(); it != images.end(); ++it, offsetX += (width + 2 * this->BorderSize))
		{
			if(offsetX >= width * this->NumColumns)
			{
				offsetX = 0;
				offsetY += (height + 2 * this->BorderSize);
			}

			int offsetWidth = (width - it->cols) / 2;
			int offsetHeight = (height - it->rows) / 2;
		
			Rect roi (offsetX + offsetWidth + this->BorderSize, offsetY + offsetHeight + this->BorderSize, it->cols, it->rows);
			it->copyTo (newImg(roi));
		}
	
		this->Show (newImg);
		waitKey();
		this->Hide ();
	}
}


Mat ResultWindow::LoadImage (Mat queryImage, const string& path)
{
	Mat image = imread (path);
	vector<KeyPoint> resultKeypoints;
	if (this->DrawMatches)
	{
		bof::BagOfFeatures& bagOfFeatures = this->configuration.GetBagOfFeatures();
		if (resultKeypoints.size() == 0)
		{
			bagOfFeatures.GetFeatureDetector()->detect (image, resultKeypoints);
		}
		vector<KeyPoint> queryKeypoints;
		bagOfFeatures.GetFeatureDetector()->detect (queryImage, queryKeypoints);

		Mat queryDescriptors, resultDescriptors;
		bagOfFeatures.GetDescriptorExtractor()->compute (queryImage, queryKeypoints, queryDescriptors);
		bagOfFeatures.GetDescriptorExtractor()->compute (image, resultKeypoints, resultDescriptors);

		vector<cv::DMatch> matches;
		bagOfFeatures.GetDescriptorMatcher()->match(queryDescriptors, resultDescriptors, matches);

		if (matches.size() > this->NumMatches)
		{
			// keeping only the strongest matches
			std::nth_element(matches.begin(), matches.begin() + this->NumMatches - 1, matches.end());
			matches.erase(matches.begin() + this->NumMatches, matches.end());
		}

		int flag = this->DrawKeypoints ? DrawMatchesFlags::DEFAULT : DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS;

		cv::Mat imageMatches;
		cv::drawMatches(queryImage, queryKeypoints, image, resultKeypoints, matches, imageMatches, cv::Scalar(0,255,255), cv::Scalar::all(-1), vector<char>(), flag);
		image = imageMatches;
	}
	else if (this->DrawKeypoints)
	{
		this->configuration.GetBagOfFeatures().GetFeatureDetector()->detect (image, resultKeypoints);
		drawKeypoints (image, resultKeypoints, image, cv::Scalar(0,255,255), cv::DrawMatchesFlags::DEFAULT);
	}
	return image;
}


} // namespace ui
} // namespace cbsf
} // namespace codemm
