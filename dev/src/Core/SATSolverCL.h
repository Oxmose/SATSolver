/*
 *
 *  CLASS SATSolverCL
 *
*/

#ifndef DEF_SATSOLVERCL_H
#define DEF_SATSOLVERCL_H

// STD INCLUDES
#include <string>   // sts::string
#include <vector>   // std::vector
#include <set>      // std::set
#include <map>      // std::map
#include <sstream>  // std::stringstream
#include <iostream> // std::cout std::cerr std::endl
#include <fstream>  // std::ifstream

// PROJECT INCLUDES
#include "ConflictGraph.h"
#include "SATSolver.h"                         // Clause class

class SATSolverCL : public SATSolver
{
    public:
    	SATSolverCL() { m_isCL = true; }
        virtual ~SATSolverCL();
	    virtual std::map<int, int> getVarScores();

    private:

        virtual void initializeMethod();

        virtual bool applyLastDecision();

        vector<pair<int,Clause>> clauseForConflict(const set<node>& nodes, set<int>& old);
        void constructConflictGraph();
        virtual bool backtrack(bool& p_unsat);

	

        std::map<int,std::vector<int>> m_clausesWithVar;//Direct access to Clauses
        std::map<int,std::set<int>> m_aliveVarsIn;
        std::map<int,int> m_levelOfVar;

	    std::map<int, int> m_varScores;

        ConflictGraph m_conflictGraph;

}; // SATSolver

#endif // DEF_SATSOLVERWL_H
