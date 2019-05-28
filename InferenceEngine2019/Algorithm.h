#pragma once

#include "KnowledgeBase.h"

namespace InferenceEngine2019
{
	class Alg
	{
	protected:
		KnowledgeBase * fKB;

		std::string fOutput;
	public:
		Alg();
		Alg(KnowledgeBase & aKnowledgeBase);

		virtual bool run();

		virtual void print();
	};

	class TT : public Alg
	{
	public:
		TT(KnowledgeBase & aKnowledgeBase) : Alg(aKnowledgeBase) {}

		bool run() override;
	};

	class FC : public Alg
	{
	public:
		FC(KnowledgeBase & aKnowledgeBase) : Alg(aKnowledgeBase) {}

		bool run() override;
	};

	class BC : public Alg
	{
	public:
		BC(KnowledgeBase & aKnowledgeBase) : Alg(aKnowledgeBase) {}

		bool run() override;
	};
}