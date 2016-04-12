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
    	SATSolverCL() { m_interact = true; m_isCL = true; m_currLevel = -1; m_btLevel=-1;  m_conflictGraph.clear(); }

        virtual ~SATSolverCL();
        virtual double getVarScores(int p_var);

    private:

        virtual void initializeMethod();

        virtual decision takeABet();
        virtual bool applyLastDecision();

        pair<vector<pair<int,Clause>>,bool> clauseForConflict(set<int>& old);
        void constructConflictGraph();
        virtual bool backtrack(bool& p_unsat);
        void addResolutionClause();

    

        std::map<int,std::vector<int>> m_clausesWithVar;//Direct access to Clauses
        std::map<int,std::set<int>> m_aliveVarsIn;

        std::map<int,int> m_parentsOf;
        int m_currLevel;
        int m_btLevel;
        Clause m_resolutionClause;
        bool m_interact;

        std::map<int, double> m_varScores;

        ConflictGraph m_conflictGraph;

        bool m_forget;

}; // SATSolver

#endif // DEF_SATSOLVERWL_H
