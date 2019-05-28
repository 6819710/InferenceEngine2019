#pragma once

#include "HornClause.h"

namespace InferenceEngine2019
{
	class KnowledgeBase
	{
	private:
		std::vector<HornClause> fClauses;
		std::vector<std::string> fLiterals;
		std::string fProposition;

	public:
		KnowledgeBase();
		KnowledgeBase(std::string aFileName);

		std::vector<std::string> getLiterals();
		std::vector<HornClause> getClauses();

		bool propositionValid();
		std::string getProposition();

		TruthTable truthTable();
	};
}