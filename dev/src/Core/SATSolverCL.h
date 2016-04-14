/*
 *
 *  CLASS SATSolverCL
 *
*/

#ifndef DEF_SATSOLVERCL_H
#define DEF_SATSOLVERCL_H

// STD INCLUDES
#include <string>       // sts::string
#include <vector>       // std::vector
#include <set>          // std::set
#include <map>          // std::map
#include <sstream>      // std::stringstream
#include <iostream>     // std::cout std::cerr std::endl
#include <fstream>      // std::ifstream
#include <functional>   // std::function
#include <queue>

// PROJECT INCLUDES
#include "ConflictGraph.h"  // ConflictGraph
#include "SATSolver.h"      // Clause 

class SATSolverCL : public SATSolver
{
    public:
        SATSolverCL(const bool &p_interact, const bool &p_forget, const bool &p_vsids, std::function<double(double, bool)> p_scoreFunction);

        virtual ~SATSolverCL();
        virtual double getVarScores(int p_var);

        std::map<std::string,int> learnedClauses() { return m_learnedClauses; }

    private:

        virtual void initializeMethod();

        virtual decision takeABet();
        virtual bool applyLastDecision();

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
        std::function<double(double, bool)> m_scoreFunction;
        
        std::map<Clause, double> m_clauseScore;

        ConflictGraph m_conflictGraph;

        std::map<std::string,int> m_learnedClauses;
        bool m_forget;
        bool m_vsids;
        std::vector<int>  m_bets;

}; // SATSolver

#endif // DEF_SATSOLVERWL_H
