#pragma once

#include <vector>
#include <string>

namespace InferenceEngine2019
{
	class TruthTable
	{
	public:
		class TruthColumn {
		private:
			std::string fTitle;
			std::vector<bool> fData;

		public:
			TruthColumn(std::string aTitle);

			// Getters
			std::string title();
			bool truth(size_t aIndex);

			bool areYou(std::string aTitle);
			size_t size();

			// Setters
			void setTitle(std::string aTitle);
			void push_back(bool aTruth);
			void setTruth(size_t aIndex, bool aTruth);
		};

	private:
		std::vector<TruthColumn> fTable;
		bool fCharDisplay;

	public:
		TruthTable();
		TruthTable(bool aFlag);

		// Getters
		TruthColumn getColumn(size_t aIndex);
		TruthColumn getColumn(std::string aTitle);

		bool getTruth(std::string aTitle, size_t aIndex);
		bool hasColumn(std::string aTitle);

		size_t size();
		size_t m_size();

		// Setters
		void push_back(TruthColumn aColumn);
		void setTruth(std::string aTitle, size_t aIndex, bool aTruth);

		// Printers
		void print();
	};
}