/*
 *
 *  CLASS FORGETBet
 *
*/

#ifndef DEF_FORGETBET_H
#define DEF_FORGETBET_H

// STD INCLUDES
#include <vector>   // std::vector
#include <map>      // std::map
#include <cmath>    // std::exp2, std::abs
#include <set>      // std::set

// PROJECT INCLUDES
#include "../Core/Clause.h"     // ClauseSet
#include "../Core/SATSolver.h"  // decision

// INHERITANCE CLASS
#include "IBet.h"

class FORGETBet : public IBet
{
    public:
        virtual ~FORGETBet();
        virtual decision takeABet(std::vector<Clause> &p_clauses, const std::set<int> &p_unsatClauses, std::map<int,int> &p_valuation);

    private:

}; // FORGETBet

#endif // DEF_FORGETBET_H
