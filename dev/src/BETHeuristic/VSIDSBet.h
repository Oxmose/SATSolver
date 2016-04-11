/*
 *
 *  CLASS VSIDSBet
 *
*/

#ifndef DEF_VSIDSBET_H
#define DEF_VSIDSBET_H

// STD INCLUDES
#include <vector>   // std::vector
#include <map>      // std::map
#include <cmath>    // std::exp2, std::abs
#include <set>      // std::set
#include <memory>   // std::shared_ptr

// PROJECT INCLUDES
#include "../Core/Clause.h"     // ClauseSet
#include "../Core/SATSolver.h"  // decision

// INHERITANCE CLASS
#include "IBet.h"

class VSIDSBet : public IBet
{
    public:
        VSIDSBet(std::shared_ptr<SATSolver> p_solver);
        virtual ~VSIDSBet();
        virtual decision takeABet(std::vector<Clause> &p_clauses, const std::set<int> &p_unsatClauses, std::map<int,int> &p_valuation);

    private:
        std::shared_ptr<SATSolver> m_solver;

}; // VSIDSBet

#endif // DEF_VSIDSBET_H
