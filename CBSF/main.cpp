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

#include "Log.h"
#include "Configuration.h"
#include "ITaskItem.h"
#include "TaskFactory.h"

#include "BasicFSImageLoader.h"
#include "ImageData.h"


using namespace std;

int main (int argc, char* argv[], char* window_name)
{
	using namespace codemm::cbsf;

	try
	{
		Configuration configuration;
		
#ifndef _DEBUG
		if (!configuration.Load (argc, argv))
		{
			return -1;
		}
#endif

		configuration.Init ();

		stringstream logFileName;
		logFileName << configuration.VisualWords << "_" << configuration.NumSketches << "_" << configuration.SketchSize << "_" << configuration.Accuracy << "_log.txt";

		logging::Log().init (logFileName.str());

		stringstream ss;
		ss << "Configuration" << endl << configuration.ToString();
		logging::Log().write(ss.str());

		for (string::iterator it = configuration.ExecutionSequence.begin(); it != configuration.ExecutionSequence.end(); ++it)
		{
			tasks::ITaskItem* pTaskItem = tasks::TaskFactory::CreateTaskItem (*it, configuration);
			if (!pTaskItem)
			{
				ss.str("");
				ss << "There is no task item assocciated with execution symbol '" << *it << "'!";
				logging::Log().write (ss.str ());
			}
			else
			{
				pTaskItem->Execute (configuration);
			}
		}

		logging::Log().write ("Everything done!");
	}
	catch (std::exception e)
	{
		stringstream ss;
		ss << "Unexpected termination of program: " << e.what();
		logging::Log().write (ss.str());
	}
	catch (...)
	{
		logging::Log().write ("Unknown expection: Terminate Program!");
	}

	return 0;
}
