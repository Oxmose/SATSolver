/*
 *
 *	CLASS SATSolver
 *
*/

// STD INCLUDES
#include <string> 	// sts::string
#include <vector>	// std::vector

// PROJECT INCLUDES
#include "Clause.h"	// Clause class

class SATSolver 
{
	public:
		SATSolver(const std::string &p_formula);
		~SATSolver();
		
		bool parse();
		bool solve();
		bool evaluate(const std::vector<int, bool> &p_valuation);
		
		std::vector<bool> getValuation();

	private:
		std::string m_strFormula;

		std::vector<Clause> m_formula;
		std::vector<bool> 	m_valuation;
}; // SATSolver 
