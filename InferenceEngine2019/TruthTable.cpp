#include "TruthTable.h"

#include <iostream>
#include <Windows.h>

InferenceEngine2019::TruthTable::TruthColumn::TruthColumn(std::string aTitle)
{
	fTitle = aTitle;
}

std::string InferenceEngine2019::TruthTable::TruthColumn::title()
{
	return fTitle;
}

bool InferenceEngine2019::TruthTable::TruthColumn::truth(size_t aIndex)
{
	return fData[aIndex];
}

bool InferenceEngine2019::TruthTable::TruthColumn::areYou(std::string aTitle)
{
	return fTitle == aTitle;
}

size_t InferenceEngine2019::TruthTable::TruthColumn::size()
{
	return fData.size();
}

void InferenceEngine2019::TruthTable::TruthColumn::setTitle(std::string aTitle)
{
	fTitle = aTitle;
}

void InferenceEngine2019::TruthTable::TruthColumn::push_back(bool aTruth)
{
	fData.push_back(aTruth);
}

void InferenceEngine2019::TruthTable::TruthColumn::setTruth(size_t aIndex, bool aTruth)
{
	fData[aIndex] = aTruth;
}

InferenceEngine2019::TruthTable::TruthTable()
{
	fCharDisplay = true;
}

InferenceEngine2019::TruthTable::TruthTable(bool aFlag)
{
	fCharDisplay = aFlag;
}

InferenceEngine2019::TruthTable::TruthColumn InferenceEngine2019::TruthTable::getColumn(size_t aIndex)
{
	return fTable[aIndex];
}

InferenceEngine2019::TruthTable::TruthColumn InferenceEngine2019::TruthTable::getColumn(std::string aTitle)
{
	for (int i = 0; i < fTable.size(); i++)
		if (fTable[i].areYou(aTitle))
			return fTable[i];
	throw std::runtime_error("Truth table does not have the coloumn: " + aTitle);
}

bool InferenceEngine2019::TruthTable::getTruth(std::string aTitle, size_t aIndex)
{
	return getColumn(aTitle).truth(aIndex);
}

bool InferenceEngine2019::TruthTable::hasColumn(std::string aTitle)
{
	for (int i = 0; i < fTable.size(); i++)
		if (fTable[i].areYou(aTitle))
			return true;
	return false;
}

size_t InferenceEngine2019::TruthTable::size()
{
	return fTable.size();
}

size_t InferenceEngine2019::TruthTable::m_size()
{
	return fTable[0].size();
}

void InferenceEngine2019::TruthTable::push_back(TruthColumn aColumn)
{
	fTable.push_back(aColumn);
}

void InferenceEngine2019::TruthTable::setTruth(std::string aTitle, size_t aIndex, bool aTruth)
{
	if (hasColumn(aTitle))
		getColumn(aTitle).setTruth(aIndex, aTruth);
	else
		throw std::runtime_error("Truth column " + aTitle + " does not exist in truth table;");
}

void InferenceEngine2019::TruthTable::print()
{
	HANDLE lConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	CONSOLE_SCREEN_BUFFER_INFO lConsoleInfo;
	WORD lConsoleColor;

	GetConsoleScreenBufferInfo(lConsole, &lConsoleInfo);
	lConsoleColor = lConsoleInfo.wAttributes;

	if (!fTable.empty())
	{
		// Print column headers
		std::vector<size_t> lLengths;
		size_t lSize = fTable[0].size();
		bool lFirstColumn = true;

		SetConsoleTextAttribute(lConsole, 14);
		for (int i = 0; i < fTable.size(); i++)
		{
			if (!lFirstColumn)
				std::cout << " | ";
			else
				lFirstColumn = false;

			std::cout << fTable[i].title().c_str();
			lLengths.push_back(fTable[i].title().size());
		}
		SetConsoleTextAttribute(lConsole, lConsoleColor);
		std::cout << std::endl;

		// Print truths
		for (int i = 0; i < lSize; i++)
		{
			lFirstColumn = true;

			for (int j = 0; j < fTable.size(); j++)
			{
				if (!lFirstColumn)
					std::cout << " | ";
				else
					lFirstColumn = false;

				if (fTable[j].truth(i))
				{
					SetConsoleTextAttribute(lConsole, 10);
					if (fCharDisplay)
						std::cout << "T";
					else
						std::cout << true;
				}
				else
				{
					SetConsoleTextAttribute(lConsole, 12);
					if (fCharDisplay)
						std::cout << "F";
					else
						std::cout << false;
				}
				SetConsoleTextAttribute(lConsole, lConsoleColor);

				if (lLengths[j] > 1)
					for (int k = 1; k < lLengths[j]; k++)
						std::cout << " ";
			}
			std::cout << std::endl;
		}
	}
}
