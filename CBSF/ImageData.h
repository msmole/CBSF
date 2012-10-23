// ImageData.h ---------------------------------------------//
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

#ifndef CBSF_IMAGEDATA_H_
#define CBSF_IMAGEDATA_H_


#include "MinHashing.h"
#include "IContext.h"

namespace codemm {
namespace cbsf {
namespace core {

class ImageData
{
public:
	ImageData(std::string name);
	virtual ~ImageData(void);

	const cv::Mat& GetImage ();
	void SetImage (const cv::Mat& pImage);

	void SetROI (const cv::Rect& roi);
	void ResetROI ();

	const std::string& GetName() const;

	cv::Mat& GetDescriptor ();
	const cv::Mat& GetDescriptor () const;
	bool SaveDescriptor () const;
	bool LoadDescriptor ();
	int GetNumVisualWords () const;
	
	std::vector<minhash::Sketch>& GetSketches ();
	const std::vector<minhash::Sketch>& GetSketches () const;

	void SetImageContext (IContext* pContext);
	IContext* GetImageContext ();

	void SetHashContext (IContext* pContext);
	IContext* GetHashContext ();

	void ReleaseImage ();
	ImageData* Clone () const;

	void SaveSketches () const;
	bool LoadSketches ();

	fs::path GetHashPath () const;
	fs::path GetDescriptorPath () const;

private:
	std::string name;
	IContext* pImageContext;
	IContext* pHashContext;
	cv::Mat* pImage;
	cv::Mat imageROI;
	cv::Mat descriptor;
	std::vector<minhash::Sketch> sketches;
	bool useROI;
};

} // namespace core
} // namespace cbsf
} // namespace codemm

#endif // CBSF_IMAGEDATA_H_
