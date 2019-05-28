#include "KnowledgeBase.h"
#include "Settings.h"

#include <algorithm>
#include <stdexcept>
#include <sstream>
#include <fstream>
#include <iostream>

InferenceEngine2019::KnowledgeBase::KnowledgeBase()
{
}

InferenceEngine2019::KnowledgeBase::KnowledgeBase(std::string aFileName)
{
	std::ifstream lInputFile;
	lInputFile.open(aFileName, std::ios::in);

	if (!lInputFile.is_open() || !lInputFile.good())
	{
		std::string errorStr = "Failed to open file \"";
		errorStr.append(aFileName);
		errorStr.append("\": ");
		errorStr.append(strerror(errno));
		throw std::runtime_error(errorStr);
	}

	int lN = 0; // line Tracking (for formatting)
	std::string lLine;

	while (std::getline(lInputFile, lLine))
	{
		// First line of File should read "TELL"
		if (lN == 0)
			if (lLine != "TELL")
				throw std::runtime_error("Incorrect File Format, First line of the file should read \"TELL\", the file reads: " + lLine);

		if (lN == 1) // Line 1 of file should contain knowledge base with set of n horn-clause statements.
		{
			std::stringstream lData(lLine);
			std::vector<std::string> lKnowledgeBase;

			while (lData.good())
			{
				std::string lKBHorn;
				std::getline(lData, lKBHorn, ';');
				lKnowledgeBase.push_back(lKBHorn);
			}

			for (size_t i = 0; i < lKnowledgeBase.size() - 1; i++)
			{
				fClauses.push_back(HornClause(lKnowledgeBase[i]));
				if (Settings::Debug)
				{
					fClauses[fClauses.size() - 1].printDetails("");
					fClauses[fClauses.size() - 1].truthTable().print();

					std::cout << std::endl << std::endl;
				}
					
			}
				
		}

		// Line 2 of file should read "ASK"
		if (lN == 2)
			if (lLine != "ASK")
				throw std::runtime_error("Incorrect File Format, First line of the file should read \"ASK\", the file reads: " + lLine);

		// Line 3 of gile should contain single proposition symbol.
		if (lN == 3)
			fProposition = lLine;

		lN++;
	}
	
	// Get all literals from Clause Set
	for (size_t i = 0; i < fClauses.size(); i++)
	{
		std::vector<std::string> lSet = fClauses[i].getLiterals();
		for (size_t i = 0; i < lSet.size(); i++)
		{
			bool lCheck = false;
			for (size_t j = 0; j < fLiterals.size(); j++)
				if (fLiterals[j] == lSet[i])
					lCheck = true;
			if (!lCheck)
				fLiterals.push_back(lSet[i]);
		}
	}
}

std::vector<std::string> InferenceEngine2019::KnowledgeBase::getLiterals()
{
	return fLiterals;
}

std::vector<InferenceEngine2019::HornClause> InferenceEngine2019::KnowledgeBase::getClauses()
{
	return fClauses;
}

/**
 * Determines if Queiry Fact is contained within literals of KB.
 */
bool InferenceEngine2019::KnowledgeBase::propositionValid()
{
	for (size_t i = 0; i < fLiterals.size(); i++)
		if (fProposition == fLiterals[i])
			return true;
	return false;
}

 std::string InferenceEngine2019::KnowledgeBase::getProposition()
 {
	 return fProposition;
 }

/**
 * Generates truth table containing all clauses in Knowledgebase.
 */
InferenceEngine2019::TruthTable InferenceEngine2019::KnowledgeBase::truthTable()
{
	TruthTable lResult;
	std::vector<std::vector<bool>> lTruthSet;

	// Construct TruthSet
	size_t lN = fLiterals.size();
	size_t lM = std::exp2(lN);

	for (size_t i = 0; i < lM; i++)
	{
		std::vector<bool> lTruthSetRow;
		size_t lO = i;
		for (size_t j = 0; j < lN; j++)
		{
			lTruthSetRow.push_back(lO % 2 == 0 ? false : true);
			lO = lO / 2;
		}
		lTruthSet.push_back(lTruthSetRow);
	}

	// Add literals to truth table
	for (size_t j = lN; j > 0; j--)
	{
		TruthTable::TruthColumn lTruth(fLiterals[fLiterals.size() - j]);
		for (size_t i = 0; i < lM; i++)
			lTruth.push_back(lTruthSet[i][j - 1]);
		lResult.push_back(lTruth);
	}

	// Calculate remaining truth columns in Knowelegebase
	for (size_t k = 0; k < fClauses.size(); k++)
	{
		TruthTable::TruthColumn lTruth(fClauses[k].to_string());
		for (size_t i = 0; i < lM; i++)
		{
			for (size_t j = 0; j < lN; j++)
			fClauses[k].setTruth(fLiterals[j], lResult.getTruth(fLiterals[j], i));
			lTruth.push_back(fClauses[k].run());
		}

		lResult.push_back(lTruth);
	}

	return lResult;
}
