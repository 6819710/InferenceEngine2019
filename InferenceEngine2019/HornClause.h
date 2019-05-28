#pragma once

#include "TruthTable.h"

#include <vector>
#include <string>

namespace InferenceEngine2019
{
	/**
	 * Horn Clause Model Definintion
	 * @version 1.0.0
	 * @author	J.P. Galovic - 6819710
	 * @date	May 2019
	 */
	class HornClause
	{
	public:
		/**
		 * Symbol Definitions.
		 */
		enum Symbol
		{
			EQUIVILANCE,	// <=>
			IMPLICATION,	// =>
			CONJUNCTION,	// &
			DISJUNCTION,	// \/
			NEGATION,		// ~

			PARENTHESIS,	// ( )
			LITERAL,		// any other character

			NONE
		};

	private:
		const std::vector<std::string> lkpEnglish = { "EQUIVILANCE", "IMPLICATION", "CONJUNCTION", "DISJUNCTION", "NEGATION", "PARENTHESIS", "LITERAL", "NONE" };
		const std::vector<std::string> lkpString = { "<=>", "=>", "&", "||"};

		/**
		 * Each horn clause is defined by three componants, fP, fSymbol, and fQ, directly defining for example P => Q.
		 * A => B => C  is assumed the equivlant of A => (B => C), the same is said on any other componant except Parenthesis or Equivilance.
		 * Equivlance assumes that A => B <=> C is the eqivilant of (A => B) <=> C.
		 * All parenthesis clauses are run and executed within a child clause. the structure would look like the following: (A & B) => C;
		 * (P) => C
		 *  |
		 * (P): A & B
		 */

		std::string fStatement; // String of statement. Primarily used for definintions of Literals. (to_string() generates formatted string)
		bool fTruth; // Truth of clause. (used for calucations)

		HornClause* fP;
		HornClause* fQ;
		HornClause* fC;

		Symbol fOperand;

		/**
		 * Interal Use Methods.
		 */
		HornClause();
		void initalise();

		bool subStatementsTruthSet();

	public:
		/**
		 * Construction & Destruction Methods.
		 */
		static HornClause NIL; // Null sentinal.
		HornClause(std::string aClauseString);
		
		/**
		 * Aux. Methods
		 */
		void setTruth(std::string aLiteral, bool aTruth);

		bool run();
		std::string to_string();

		std::vector<std::string> getLiterals();
		bool hasLiteral(std::string aLiteral);

		HornClause premise();
		HornClause conclustion();

		Symbol getType();

		TruthTable truthTable();

		/**
		 * Printing Methods.
		 */
		void printDetails(std::string aPrefix); // Prints details of this Clause
	};
}