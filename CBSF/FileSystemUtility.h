// FileSystemUtility.h ---------------------------------------------//
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

#ifndef CBSF_FILESYSTEMUTILITY_H_
#define CBSF_FILESYSTEMUTILITY_H_


namespace codemm {
namespace cbsf {
namespace core {

std::istream& operator >> (std::istream& is, std::vector<float>& values);
std::istream& operator >> (std::istream& is, std::vector<std::vector<float>>& data);

std::ostream& operator << (std::ostream& os, const std::vector<float>& values);
std::ostream& operator << (std::ostream& os, const std::vector<std::vector<float>>& data);


class FileSystemUtility
{
public:
	static void GetFiles (const fs::path& sourcePath, const std::string& extension, std::vector<fs::path>& files);

	static void Save1DMat (const fs::path& path, const cv::Mat& mat);
	static bool Load1DMat (const fs::path& path, cv::Mat& mat);

	template <class T> 
	static void SaveVector (const fs::path& path, const std::vector<T>& matrix);

	template <class T> 
	static bool LoadVector (const fs::path& path, std::vector<T>& matrix);

	template <class T> 
	static void SaveMatrix (const fs::path& path, const std::vector<std::vector<T>>& matrix);

	template <class T> 
	static bool LoadMatrix (const fs::path& path, std::vector<std::vector<T>>& matrix);

};


template <class T> 
void FileSystemUtility::SaveVector (const fs::path& path, const std::vector<T>& vec)
{
	ofstream ofs (path.string(), ios::binary);
	if (!vec.empty())
	{
		int num = (int) vec.size();
		ofs.write((const char*) &num, sizeof (num));
		ofs.write ((const char*) &vec[0], num * sizeof(vec[0]));
	}
	ofs.close();
}


template <class T> 
bool FileSystemUtility::LoadVector (const fs::path& path, std::vector<T>& vec)
{
	if (fs::exists(path))
	{
		vec.clear();
		ifstream ifs (path.string(), ios::binary);

		int num;
		ifs.read ((char*) &num, sizeof (num));

		vec.resize(num);
		ifs.read((char*) &vec[0], num * sizeof(vec[0]));
		ifs.close();
		return true;
	}
	return false;
}


template <class T>
void FileSystemUtility::SaveMatrix (const fs::path& path, const std::vector<std::vector<T>>& matrix)
{
	ofstream ofs (path.string(), ios::binary);
	if (!matrix.empty())
	{
		size_t numRows = matrix.size();
		size_t numCols = matrix[0].size();
		ofs.write((const char*) &numRows, sizeof (numRows));
		ofs.write((const char*) &numCols, sizeof (numCols));

		for (size_t i = 0; i < numRows; ++i)
		{
			assert (matrix[i].size() == numCols);
			ofs.write ((const char*) &matrix[i][0], numCols * sizeof(matrix[0][0]));
		}
	}
	ofs.close();
}


template <class T>
bool FileSystemUtility::LoadMatrix (const fs::path& path, std::vector<std::vector<T>>& matrix)
{
	if (fs::exists(path))
	{
		matrix.clear();
		ifstream ifs (path.string(), ios::binary);

		size_t numRows, numCols;
		ifs.read ((char*) &numRows, sizeof (numRows));
		ifs.read ((char*) &numCols, sizeof (numCols));

		matrix.resize(numRows);
		for (size_t i = 0; i < numRows; ++i)
		{
			matrix[i].resize(numCols);
			ifs.read((char*) &matrix[i][0], numCols * sizeof(matrix[0][0]));
		}
		ifs.close();
		return true;
	}
	return false;
}


} // namespace core
} // namespace cbsf
} // namespace codemm

#endif // CBSF_FILESYSTEMUTILITY_H_