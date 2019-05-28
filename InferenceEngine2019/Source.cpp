/**
 * Inference Engine
 * @version v1.1.0
 * @author  J.P. Galovic - 6819710
 * @date    May 2019
 */

#include "Algorithm.h"
#include "Settings.h"

#include <algorithm>
#include <iostream>
#include <stdexcept>

using namespace InferenceEngine2019;

int main(int argc, char** argv)
{
	if (argc < 3)
	{
		std::cerr << "Executiable parameters not set correctly, please use the following paramiter: <METHOD> <FILEPATH>";
		system("PAUSE");
		return 1;
	}

	try
	{
		std::string lFileName = argv[2];
		std::string lMethod = argv[1];

		std::transform(lMethod.begin(), lMethod.end(), lMethod.begin(), toupper);

		KnowledgeBase lKB(lFileName);
		if (Settings::Debug)
		{
			system("PAUSE");
			system("CLS");
		}

		if (lMethod == "TT")
		{
			TT lAlgorithem(lKB);
			lAlgorithem.run();
			lAlgorithem.print();
		}
		else if (lMethod == "FC")
		{
			FC lAlgorithem(lKB);
			lAlgorithem.run();
			lAlgorithem.print();
		}
		else if (lMethod == "BC")
		{
			BC lAlgorithem(lKB);
			lAlgorithem.run();
			lAlgorithem.print();
		}
		else
		{
			std::cerr << "The method " << lMethod << " this program. please use methods: TT, FC, BC" << std::endl;
			system("PAUSE");
			return 3;
		}

		system("PAUSE");
		return 0;
	}
	catch (const std::runtime_error & e)
	{
		std::cerr << e.what() << std::endl;
		system("PAUSE");
		return 2;
	}
}