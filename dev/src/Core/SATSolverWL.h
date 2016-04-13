/*
 *
 *  CLASS SATSolverWL
 *
*/

#ifndef DEF_SATSOLVERWL_H
#define DEF_SATSOLVERWL_H

// STD INCLUDES
#include <string>   // sts::string
#include <vector>   // std::vector
#include <set>      // std::set
#include <map>      // std::map
#include <sstream>  // std::stringstream
#include <iostream> // std::cout std::cerr std::endl
#include <fstream>  // std::ifstream

// PROJECT INCLUDES
#include "SATSolver.h"                         // Clause class

class SATSolverWL : public SATSolver
{
    public:
    	SATSolverWL();
        virtual ~SATSolverWL();
        virtual double getVarScores(int p_var);

    private:

        virtual void initializeMethod();

        virtual bool applyLastDecision();

        virtual bool backtrack(bool& p_unsat);

        std::map<int,std::set<int>> m_clausesWatchedBy;//Direct access to Clauses
}; // SATSolver

#endif // DEF_SATSOLVERWL_H
