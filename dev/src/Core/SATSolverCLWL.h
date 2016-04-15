#ifndef DEF_SATSOLVERCLWL_H
#define DEF_SATSOLVERCLWL_H

// STD INCLUDES
#include <string>       // sts::string
#include <vector>       // std::vector
#include <set>          // std::set
#include <map>          // std::map
#include <sstream>      // std::stringstream
#include <iostream>     // std::cout std::cerr std::endl
#include <fstream>      // std::ifstream
#include <functional>   // std::function

// PROJECT INCLUDES
#include "ConflictGraph.h"  // ConflictGraph
#include "SATSolver.h"      // Clause 

class SATSolverCLWL : public SATSolver
{
    public:
        SATSolverCLWL(const bool &p_naiveuip, const bool &p_interact, const bool &p_forget, const bool &p_vsids, std::function<double(double, bool)> p_scoreFunction);

        virtual ~SATSolverCLWL();
        virtual double getVarScores(int p_var);

        std::map<std::string,int> learnedClauses() { return m_learnedClauses; }

    private:

        virtual void initializeMethod();

        virtual decision takeABet();
        virtual bool applyLastDecision();
        void standardBT();
        virtual bool backtrack(bool& p_unsat);
        bool addResolutionClause();

    

        std::map<int,std::set<int>> m_clausesWatchedBy;

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
        bool m_naiveuip;
        std::vector<int> m_bets;

}; // SATSolver


#endif
