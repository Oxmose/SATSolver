/*
 *
 *  CLASS SATSolver
 *
*/

#ifndef DEF_SATSOLVERSTD_H
#define DEF_SATSOLVERSTD_H

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

class SATSolverSTD : public SATSolver
{
    public:
        virtual ~SATSolverSTD();

    private:

        virtual void initializeMethod();

        virtual void applyLastDecision();

        virtual bool solve();

        virtual bool deduce();
        virtual bool unitProp();
        virtual bool uniquePol();
        virtual bool backtrack(bool& p_unsat);


        std::map<int,std::vector<int>> m_clauseWithVar;//Direct access to Clauses
        std::map<int,std::set<int>> m_aliveVarIn;
        
}; // SATSolver

#endif // DEF_SATSOLVERSTD_H
