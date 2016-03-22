/*
 *
 *  CLASS StandardBet
 *
*/

#ifndef DEF_STANDARDBET_H
#define DEF_STANDARDBET_H

// STD INCLUDES
#include <vector>   // std::vector

// PROJECT INCLUDES
#include "../Core/Clause.h"     // ClauseSet
#include "../Core/SATSolver.h"  // decision

// INHERITANCE CLASS
#include "IBet.h"

class StandardBet : public IBet
{
    public:
        virtual ~StandardBet();
        virtual decision takeABet(std::vector<Clause> &p_clauses, const std::set<int> &p_unsatClauses, std::map<int,int> &p_valuation);

    private:
}; // StandardBet

#endif // DEF_STANDARDBET_H