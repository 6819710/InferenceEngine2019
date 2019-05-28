#include "Algorithm.h"
#include "Settings.h"

#include <iostream>
#include <queue>
#include <sstream>


InferenceEngine2019::Alg::Alg()
{
	fKB = nullptr;
	fOutput = "";
}

InferenceEngine2019::Alg::Alg(KnowledgeBase & aKnowledgeBase)
{
	fKB = &aKnowledgeBase;
}

bool InferenceEngine2019::Alg::run()
{
	return false;
}

void InferenceEngine2019::Alg::print()
{
	std::cout << fOutput << std::endl;
}

bool InferenceEngine2019::TT::run()
{
	// Check if KB contains refernce to Queiry?
	if (!fKB->propositionValid())
	{
		fOutput = "NO, Knowledgebase does not contain refernce to: " + fKB->getProposition();
		return false;
	}

	// Get truth table to check against.
	if (Settings::Debug)
		std::cout << "Please Standby, Generating Truth Table." << std::endl << std::endl;

	TruthTable lTruthTable = fKB->truthTable();

	// Knowledge base is true if each clause in the knowledge base is deemed correct.
	std::vector<std::string> lClauses;
	for (size_t i = 0; i < fKB->getClauses().size(); i++)
		lClauses.push_back(fKB->getClauses()[i].to_string());
		

	TruthTable::TruthColumn lCorrect("KB Validated");

	for (size_t j = 0; j < lTruthTable.m_size(); j++)
	{
		bool lValid = true;
		for (size_t i = 0; i < lClauses.size(); i++)
			if (!lTruthTable.getTruth(lClauses[i], j))
				lValid = false;
		lCorrect.push_back(lValid);
	}

	lTruthTable.push_back(lCorrect);

	// On Debug Print Truth Table.
	if (Settings::Debug)
	{
		lTruthTable.print();
		std::cout << std::endl;
	}
		

	// Examine Truth Table to determine output. (Count number of validated kb).
	size_t lCount = 0;
	for (size_t i = 0; i < lTruthTable.m_size(); i++)
		if (lTruthTable.getTruth("KB Validated", i))
			lCount++;

	if (lCount > 0) // Yes queiry is validated in kb
	{
		std::stringstream lS;
		lS << lCount;
		fOutput = "Yes: " + lS.str();
		return true;
	}
	else
	{
		fOutput = "No: KnowledgeBase not Validated.";
		return false;
	}
}

bool InferenceEngine2019::FC::run()
{
	// Check if KB contains refernce to Queiry?
	if (!fKB->propositionValid())
	{
		fOutput = "NO, Knowledgebase does not contain refernce to: " + fKB->getProposition();
		return false;
	}

	// Load Required Variables
	std::vector<std::pair<std::string, int>> lCount;
	std::vector<std::pair<std::string, bool>> lInferred;
	std::string lQ = fKB->getProposition();
	std::vector<std::string> lLiteralsOut;

	// Determine count of literals in premise of each statement, ignore Literal definitions
	for (size_t i = 0; i < fKB->getClauses().size(); i++)
		if(fKB->getClauses()[i].getType() != HornClause::LITERAL)
			lCount.push_back(std::pair<std::string, int>(fKB->getClauses()[i].to_string(), fKB->getClauses()[i].premise().getLiterals().size()));

	// Prepare list of literals
	std::vector<std::string> lLiterals = fKB->getLiterals();
	for (size_t i = 0; i < lLiterals.size(); i++)
		lInferred.push_back(std::pair<std::string, bool>(lLiterals[i], false));

	// Load Agenda with known truths (i.e. Literals defined within KB definition (not derived)
	std::queue<std::string> lAgenda;
	for (size_t i = 0; i < fKB->getClauses().size(); i++)
	{
		if (fKB->getClauses()[i].getType() == HornClause::LITERAL)
		{
			std::string lLiteral = fKB->getClauses()[i].to_string();
			lAgenda.push(lLiteral);
		}
	}
			
	while (!lAgenda.empty())
	{
		std::string lP = lAgenda.front();
		lLiteralsOut.push_back(lP);
		lAgenda.pop();

		// Check if Literal matches Queiry
		if (lP == lQ)
		{
			fOutput = "YES:";
			for (size_t i = 0; i < lLiteralsOut.size(); i++)
				fOutput.append(" " + lLiteralsOut[i]);
			return true;
		}

		bool lCurrentInferred;
		for (size_t i = 0; i < lInferred.size(); i++)
			if (lInferred[i].first == lP)
				lCurrentInferred = lInferred[i].second;

		if (!lCurrentInferred)
		{
			for (size_t i = 0; i < lInferred.size(); i++)
				if (lInferred[i].first == lP)
					lInferred[i].second = true;

			for (size_t i = 0; i < fKB->getClauses().size(); i++)
				if (fKB->getClauses()[i].premise().hasLiteral(lP))
				{
					std::string lCurrentClause = fKB->getClauses()[i].to_string();
					for (size_t j = 0; j < lCount.size(); j++)
						if (lCount[j].first == lCurrentClause)
						{
							lCount[j].second--;
							if (lCount[j].second == 0)
							{
								std::vector<std::string> lContainedLiterals = fKB->getClauses()[i].conclustion().getLiterals();

								for (size_t k = 0; k < lContainedLiterals.size(); k++)
								{
									bool lChecked = true;
									for (size_t l = 0; l < lLiteralsOut.size(); l++)
										if (lContainedLiterals[k] == lLiteralsOut[l])
											lChecked = false;

									std::queue<std::string> lCheck(lAgenda);
									while (!lCheck.empty())
									{
										std::string lCheckingLiteral = lCheck.front();
										lCheck.pop();
										if (lContainedLiterals[k] == lCheckingLiteral)
											lChecked = false;
									}
									if (lChecked)
										lAgenda.push(lContainedLiterals[k]);
								}
							}
						}
				}
		}	
	}

	fOutput = "No: ";
	for (size_t i = 0; i < lLiteralsOut.size(); i++)
		fOutput.append(" " + lLiteralsOut[i]);
	return false;
}

bool InferenceEngine2019::BC::run()
{
	// Check if KB contains refernce to Queiry?
	if (!fKB->propositionValid())
	{
		fOutput = "NO, Knowledgebase does not contain refernce to: " + fKB->getProposition();
		return false;
	}

	// Load list with known truths (i.e. Literals defined within KB definition (not derived)
	std::vector<std::string> lKnown;
	for (size_t i = 0; i < fKB->getClauses().size(); i++)
		if (fKB->getClauses()[i].getType() == HornClause::LITERAL)
			lKnown.push_back(fKB->getClauses()[i].to_string());

	// Load queue with proposition
	std::queue<std::string> lAgenda;
	lAgenda.push(fKB->getProposition());

	std::vector<std::string> lLiteralsOut;

	// Continute while there are still propositions to be proven
	while (!lAgenda.empty())
	{
		std::string lCurrent = lAgenda.front();
		lAgenda.pop();

		lLiteralsOut.push_back(lCurrent);

		// Check that current is not in proven terms
		bool lContained = false;
		for (size_t i = 0; i < lKnown.size(); i++)
			if (lCurrent == lKnown[i])
				lContained = true;

		if (!lContained)
		{
			// Find further literals that need to be proven
			bool lOk = false;
			for (size_t i = 0; i < fKB->getClauses().size(); i++)
			{
				if (fKB->getClauses()[i].conclustion().hasLiteral(lCurrent)) // Add all premise literals where the conclusion contains current literal.
				{
					lOk = true;
					std::vector<std::string> lContainedLiterals = fKB->getClauses()[i].premise().getLiterals();

					for (size_t k = 0; k < lContainedLiterals.size(); k++)
					{
						bool lChecked = true;
						for (size_t l = 0; l < lLiteralsOut.size(); l++)
							if (lContainedLiterals[k] == lLiteralsOut[l])
								lChecked = false;

						std::queue<std::string> lCheck(lAgenda);
						while (!lCheck.empty())
						{
							std::string lCheckingLiteral = lCheck.front();
							lCheck.pop();
							if (lContainedLiterals[k] == lCheckingLiteral)
								lChecked = false;
						}
						if (lChecked)
							lAgenda.push(lContainedLiterals[k]);
					}
				}
			}
			if (!lOk)
			{
				fOutput = "No: ";
				for (size_t i = 0; i < lLiteralsOut.size(); i++)
					fOutput.append(" " + lLiteralsOut[i]);
				return false;
			}
		}
	}

	fOutput = "YES:";
	for (size_t i = lLiteralsOut.size(); i > 0; i--)
		fOutput.append(" " + lLiteralsOut[i - 1]);
	return true;
}
