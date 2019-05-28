#include "HornClause.h"

#include <algorithm>
#include <iostream>
#include <map>
#include <queue>
#include <stack>
#include <Windows.h>

InferenceEngine2019::HornClause InferenceEngine2019::HornClause::NIL;

InferenceEngine2019::HornClause::HornClause()
{
	initalise();
}

void InferenceEngine2019::HornClause::initalise()
{
	fP = &NIL;
	fQ = &NIL;
	fC = &NIL;

	fOperand = NONE;
	fTruth = false;
}

/**
 * Horn Clause Constructor, constructs Horn Clause from defined format for clause (see readme).
 * @param	aClauseString, string containing Horn Clause in defined format.
 */
InferenceEngine2019::HornClause::HornClause(std::string aClauseString)
{
	initalise();
	// Remove blank spaces from Clause String
	std::string::iterator lEndPos = std::remove(aClauseString.begin(), aClauseString.end(), ' ');
	aClauseString.erase(lEndPos, aClauseString.end());

	fStatement = aClauseString;

	// Find first of each Operand
	std::vector<std::pair<size_t, Symbol>> lOperands;

	for (size_t i = 0; i < lkpString.size(); i++)
	{
		size_t lStrPos = 0;
		if ((lStrPos = fStatement.find(lkpString[i])) != std::string::npos)
			lOperands.push_back(std::pair<size_t, Symbol>(lStrPos, (Symbol)i));
	}

	// Find Paranthesis "(" ")"
	std::vector<size_t> lOpenPos;
	std::vector<size_t> lClosePos;

	size_t lPos = 0;
	std::string lData = fStatement;

	// Find "("
	while ((lPos = lData.find_last_of('(')) != std::string::npos)
	{
		lOpenPos.push_back(lPos);
		lData.erase(lPos, lData.size());
	}

	// Find ")"
	lPos = 0;
	lData = fStatement;
	while ((lPos = lData.find_last_of(')')) != std::string::npos)
	{
		lClosePos.push_back(lPos);
		lData.erase(lPos, lData.size());
	}

	// Check if text is correct (same number of open and close parenthesis)
	if(lOpenPos.size() != lClosePos.size())
		throw std::runtime_error("Invalid Horn Clause format, open and close parenthesis are not matching: " + fStatement);

	/**
	 * Determine location of opening and closing parenthesis, correctly pair up "(" and ")". Mainly looking for cases such as ( A => ( B & C ))
	 * Code should work on the following rule, index of close is index of open + number of opens before said close. i.e. in the above case,
	 * the indexes should be 0 for the open and 1 for the close. Note: the open parenthesis should always be at index 0.
	 * The opperand should always be before the first opening, or immedialty after the matching close.
	 */

	std::vector<std::pair<size_t, size_t>> lPairs;

	if (lOpenPos.size() > 0)
	{
		size_t lCount = 0;
		// For each open match the close.
		for (size_t j = 0; j < lOpenPos.size(); j++)
		{
			size_t lClose = lClosePos[j];
			size_t lOpen = lOpenPos[j - lCount];

			bool lChange = false;

			for (size_t i = (j + 1); i < lClosePos.size() - lCount; i++)
				if (lClosePos[i] > lOpen)
				{
					lCount++;
					lChange = true;
				}
				else
					if (lCount > 0)
						lCount--;
				
			if (lChange)
				lOpen = lOpenPos[j + lCount];

			lPairs.push_back(std::pair<size_t, size_t>(lOpen, lClose));
		}

		// If whole statement is withing parenthesis, put statement except for opening and closing parenthesis.
		if (lPairs[0].first == 0 && lPairs[0].second + 1 == fStatement.size())
		{
			fOperand = PARENTHESIS;
			fC = new HornClause(fStatement.substr(1, lPairs[0].second - 1));
			return;
		}

	}

	// Handle processing of LITERALS. (i.e. Clause string does not contain any defined operands)
	if (lOperands.size() == 0)
	{
		// Handle Negation
		if (fStatement.find("~") != std::string::npos)
		{
			fOperand = NEGATION;
			fC = new HornClause(fStatement.substr(1, fStatement.size()));
		}
		else
			fOperand = LITERAL;
	}
	else
	{
		// Find Correct Operand to Pharse (note "=>" and "<=>" should take preferance over "&" or "||"
		size_t lIndex = lOperands.size() - 1;
		size_t lPos = lOperands[lIndex].first;

		Symbol lCurrent = lOperands[lIndex].second;

		bool lValid = true; // Flag used to indicate that valid Operand has been found.

		// Find first Operand not within Parenthesis. note, valid operand will be before the first "(" or after the matching ")"
		for (size_t i = lOperands.size(); i > 0; i--)
		{
			bool lOverride = false;
			// If lCurrent is not "<=>" or "=>" AND this operand is, override
			if ((lCurrent != EQUIVILANCE && lCurrent != IMPLICATION) && (lOperands[i - 1].second == EQUIVILANCE || lOperands[i - 1].second == IMPLICATION))
				lOverride = true;

			// If position if more left than current position.
			if (lOperands[i - 1].first < lPos)
			{
				// override if both lcurrent and this operand are either "<=>" or "=>"
				if ((lCurrent == EQUIVILANCE || lCurrent == IMPLICATION) && (lOperands[i - 1].second == EQUIVILANCE || lOperands[i - 1].second == IMPLICATION))
					lOverride = true;
				// override if neither lCurrent or this opperand are either "<=>" or "=>"
				else if ((lCurrent != EQUIVILANCE && lCurrent != IMPLICATION) && (lOperands[i - 1].second != EQUIVILANCE && lOperands[i - 1].second != IMPLICATION))
					lOverride = true;
			}

			// Do not overidde if this operand falls between any pair of "(" or ")"
			for (size_t j = 0; j < lPairs.size(); j++)
				if (lOperands[i - 1].first > lPairs[j].first && lOperands[i - 1].first < lPairs[j].second)
					lOverride = false;

			// If needing to override, then do so.
			if (lOverride)
			{
				lIndex = i - 1;
				lPos = lOperands[lIndex].first;
				lCurrent = lOperands[lIndex].second;
			}
		}

		// Correct Operand Found, Everything before Operand becomes fP, everything after Operand becomes fQ
		fOperand = lOperands[lIndex].second;
		fP = new HornClause(fStatement.substr(0, lOperands[lIndex].first));
		fQ = new HornClause(fStatement.substr(lOperands[lIndex].first + lkpString[fOperand].size(), fStatement.size()));
	}
}

/**
 * Sets value of Truth on defined literal within statement.
 * @param	aLiteral, string defining literal.
 * @param	aTruth, boolean with value to set truth to (obiously true = true)
 */
void InferenceEngine2019::HornClause::setTruth(std::string aLiteral, bool aTruth)
{
	if (fOperand == LITERAL)
		if (fStatement == aLiteral)
			fTruth = aTruth;
			

	if (fP != &NIL)
		fP->setTruth(aLiteral, aTruth);
	if (fQ != &NIL)
		fQ->setTruth(aLiteral, aTruth);
	if (fC != &NIL)
		fC->setTruth(aLiteral, aTruth);
}

/**
 * Runs Clause
 */
bool InferenceEngine2019::HornClause::run()
{
	// RUN
	switch (fOperand)
	{
	case InferenceEngine2019::HornClause::EQUIVILANCE:
		fTruth = fP->run() == fQ->run();
		break;
	case InferenceEngine2019::HornClause::IMPLICATION:
		if (fP->run())
			fTruth = fQ->run();
		else
			fTruth = true;
		break;
	case InferenceEngine2019::HornClause::CONJUNCTION:
		fTruth = fP->run() && fQ->run();
		break;
	case InferenceEngine2019::HornClause::DISJUNCTION:
		fTruth = fP->run() || fQ->run();
		break;
	case InferenceEngine2019::HornClause::NEGATION:
		fTruth = !fC->run();
		break;
	case InferenceEngine2019::HornClause::PARENTHESIS:
		fTruth = fC->run();
		break;
	case InferenceEngine2019::HornClause::NONE:
		throw std::runtime_error("Statement not correctly initalised.");
	}
	return fTruth;
}

/**
 * Generates formated string from contents of Horn Clause (not just print fStatement).
 */
std::string InferenceEngine2019::HornClause::to_string()
{
	switch (fOperand)
	{
	case InferenceEngine2019::HornClause::IMPLICATION:
		if(fP != &NIL && fQ != &NIL)
			return fP->to_string() + " => " + fQ->to_string();
	case InferenceEngine2019::HornClause::EQUIVILANCE:
		if (fP != &NIL && fQ != &NIL)
			return fP->to_string() + " <=> " + fQ->to_string();
	case InferenceEngine2019::HornClause::CONJUNCTION:
		if (fP != &NIL && fQ != &NIL)
			return fP->to_string() + " & " + fQ->to_string();
	case InferenceEngine2019::HornClause::DISJUNCTION:
		if (fP != &NIL && fQ != &NIL)
			return fP->to_string() + " || " + fQ->to_string();
	case InferenceEngine2019::HornClause::NEGATION:
		return "~" + fC->to_string();
	case InferenceEngine2019::HornClause::PARENTHESIS:
		return "(" + fC->to_string() + ")";
	case InferenceEngine2019::HornClause::LITERAL:
		return fStatement;
	}
	return std::string();
}

/**
 * Gets list of literals in clause.
 */
std::vector<std::string> InferenceEngine2019::HornClause::getLiterals()
{
	std::vector<std::string> lResult;

	if (fOperand == LITERAL)
		lResult.push_back(fStatement);
	else
	{
		if (fP != &NIL)
		{
			std::vector<std::string> lSet = fP->getLiterals();
			for (size_t i = 0; i < lSet.size(); i++)
			{
				bool lCheck = false;
				for (size_t j = 0; j < lResult.size(); j++)
					if (lResult[j] == lSet[i])
						lCheck = true;
				if (!lCheck)
					lResult.push_back(lSet[i]);
			}
		}

		if (fQ != &NIL)
		{
			std::vector<std::string> lSet = fQ->getLiterals();
			for (size_t i = 0; i < lSet.size(); i++)
			{
				bool lCheck = false;
				for (size_t j = 0; j < lResult.size(); j++)
					if (lResult[j] == lSet[i])
						lCheck = true;
				if (!lCheck)
					lResult.push_back(lSet[i]);
			}
		}

		if (fC != &NIL)
		{
			std::vector<std::string> lSet = fC->getLiterals();
			for (size_t i = 0; i < lSet.size(); i++)
			{
				bool lCheck = false;
				for (size_t j = 0; j < lResult.size(); j++)
					if (lResult[j] == lSet[i])
						lCheck = true;
				if (!lCheck)
					lResult.push_back(lSet[i]);
			}
		}
	}
	return lResult;
}

bool InferenceEngine2019::HornClause::hasLiteral(std::string aLiteral)
{
	if (fOperand == LITERAL)
		if (fStatement == aLiteral)
			return true;

	if (fP != &NIL)
		if(fP->hasLiteral(aLiteral))
		return true;
	if (fQ != &NIL)
		if (fQ->hasLiteral(aLiteral))
			return true;
	if (fC != &NIL)
		if (fC->hasLiteral(aLiteral))
			return true;

	return false;
}

InferenceEngine2019::HornClause InferenceEngine2019::HornClause::premise()
{
	return *fP;
}

InferenceEngine2019::HornClause InferenceEngine2019::HornClause::conclustion()
{
	return *fQ;
}

InferenceEngine2019::HornClause::Symbol InferenceEngine2019::HornClause::getType()
{
	return fOperand;
}

/**
 * Gets truth table for Horn Clause.
 */
InferenceEngine2019::TruthTable InferenceEngine2019::HornClause::truthTable()
{
	TruthTable lResult;
	std::vector<std::string> lLiterals = getLiterals();
	std::vector<std::vector<bool>> lTruthSet;

	// Construct truth set.
	size_t lN = lLiterals.size();
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

	// Add literals to truth table.
	for (size_t j = lN; j > 0; j--)
	{
		TruthTable::TruthColumn lTruth(lLiterals[lLiterals.size() - j]);
		for (size_t i = 0; i < lM; i++)
			lTruth.push_back(lTruthSet[i][j - 1]);
		lResult.push_back(lTruth);
	}

	// Calculate result based on truth table segments.
	TruthTable::TruthColumn lTruth(to_string());
	for (size_t i = 0; i < lM; i++)
	{
		for (size_t j = 0; j < lN; j++)
			setTruth(lLiterals[j], lResult.getTruth(lLiterals[j], i));
		lTruth.push_back(run());
	}
	lResult.push_back(lTruth);
	
	return lResult;
}

/**
 * Prints details of Horn Clause. (Mainly used as debuggin tool)
 * Printer also triggers printing of details for P, Q & C where applicable.
 */
void InferenceEngine2019::HornClause::printDetails(std::string aPrefix)
{
	HANDLE lConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	CONSOLE_SCREEN_BUFFER_INFO lConsoleInfo;
	WORD lConsoleColor;
	
	GetConsoleScreenBufferInfo(lConsole, &lConsoleInfo);
	lConsoleColor = lConsoleInfo.wAttributes;

	//Print clause core details.
	std::cout << aPrefix << "Horn Clause: " << fStatement << std::endl;

	std::cout << aPrefix << "to_string(): ";
	SetConsoleTextAttribute(lConsole, 11);
	std::cout << to_string() << std::endl;

	SetConsoleTextAttribute(lConsole, lConsoleColor);
	std::cout << aPrefix << "Type: ";
	SetConsoleTextAttribute(lConsole, 11);
	std::cout << lkpEnglish[fOperand] << std::endl;

	if (fOperand == LITERAL)
	{
		SetConsoleTextAttribute(lConsole, lConsoleColor);
		std::cout << aPrefix << "Truth Set: ";
		if (fTruth == true)
		{
			SetConsoleTextAttribute(lConsole, 10);
			std::cout << "True" << std::endl;
		}
		else if(fTruth == false)
		{
			SetConsoleTextAttribute(lConsole, 12);
			std::cout << "False" << std::endl;
		}
		else
		{
			SetConsoleTextAttribute(lConsole, 12);
			std::cout << "NOT SET" << std::endl;
		}
	}

	if (fP != &NIL)
	{
		SetConsoleTextAttribute(lConsole, lConsoleColor);
		std::cout << aPrefix << "P: ";
		SetConsoleTextAttribute(lConsole, 11);
		std::cout << fP->to_string() << std::endl;
	}

	if (fQ != &NIL)
	{
		SetConsoleTextAttribute(lConsole, lConsoleColor);
		std::cout << aPrefix << "Q: ";
		SetConsoleTextAttribute(lConsole, 11);
		std::cout << fQ->to_string() << std::endl;
	}

	if (fC != &NIL)
	{
		SetConsoleTextAttribute(lConsole, lConsoleColor);
		std::cout << aPrefix << "C: ";
		SetConsoleTextAttribute(lConsole, 11);
		std::cout << fC->to_string() << std::endl;
	}
	
	SetConsoleTextAttribute(lConsole, lConsoleColor);
	if (fP != &NIL || fQ != &NIL || fC != &NIL)
		std::cout << aPrefix << "Connected Clauses: {" << std::endl;
	if (fP != &NIL)
		fP->printDetails(aPrefix + "    ");
	if (fQ != &NIL)
		fQ->printDetails(aPrefix + "    ");
	if (fC != &NIL)
		fC->printDetails(aPrefix + "    ");
	if (fP != &NIL || fQ != &NIL || fC != &NIL)
		std::cout << aPrefix << "}" << std::endl;
	std::cout << std::endl;
}
