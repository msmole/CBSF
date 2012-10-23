// FileSystemUtility.cpp ---------------------------------------------//
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
#include "FileSystemUtility.h"

#include <set>
#include <iostream>

#include "Log.h"
#include "BasicScreenFrameExtractor.h"
#include "ImageData.h"


namespace codemm {
namespace cbsf {
namespace core {

using namespace std;
using namespace cv;


void FileSystemUtility::GetFiles (const fs::path& sourcePath, const string& extension, vector<fs::path>& files)
{
	fs::recursive_directory_iterator endIter;
	try
	{
		for (fs::recursive_directory_iterator it (sourcePath); it != endIter; ++it)
		{
			if (extension.compare (it->path().extension().string()) == 0)
			{
				fs::path file = complete(it->path());
				files.push_back(file);
			}
		}
	}
	catch (fs::filesystem_error e)
	{
		stringstream ss;
		ss << "Unexpected error in FileSystemUtility::GetFiles: " << e.what();
		logging::Log().write (ss.str());
	}
}


void FileSystemUtility::Save1DMat (const fs::path& path, const Mat& mat)
{
	ofstream ofs (path.string(), ios::binary);
	int num = (int) mat.cols;
	ofs.write((const char*) &num, sizeof (num));

	for (int i = 0; i < mat.cols; ++i)
	{
		const float& f = mat.at<float>(0, i);
		ofs.write ((const char*)&f, sizeof(f));
	}

	ofs.close();
}


bool FileSystemUtility::Load1DMat (const fs::path& path, Mat& mat)
{
	if (fs::exists(path))
	{
		ifstream ifs (path.string(), ios::binary);

		int num;
		ifs.read ((char*) &num, sizeof (num));

		mat.create (1, num, CV_32F);
		for (int i = 0; i < mat.cols; ++i)
		{
			float f;
			ifs.read((char*)&f, sizeof(f));
			mat.at<float>(0, i) = f;
		}

		ifs.close();

		return true;
	}
	return false;
}


istream& operator >> (istream& is, vector<float>& values)
{
	values.clear ();

	string line;
	getline (is, line);

	stringstream ss (line);
	string valueString;
	while (getline (ss, valueString, ';'))
	{
		stringstream valueStream (valueString);
		valueStream.imbue(std::locale(""));
		float value = 0.0f;
		valueStream >> value;

		values.push_back (value);
	}

	return is;
}


istream& operator >> (istream& is, vector<vector<float>>& data)
{
	data.clear();

	vector<float> values;
	while (is >> values)
	{
		data.push_back (values);
	}

	return is;  
}



ostream& operator << (ostream& os, const vector<float>& values)
{
	for (vector<float>::const_iterator it = values.begin(); it != values.end(); ++it)
	{
		os << *it << ";";
	}

	return os;
}


ostream& operator << (ostream& os, const vector<vector<float>>& data)
{
	for (vector<vector<float>>::const_iterator it = data.begin(); it != data.end(); ++it)
	{
		os << *it << endl;
	}

	return os;
}



} // namespace core
} // namespace cbsf
} // namespace codemm
