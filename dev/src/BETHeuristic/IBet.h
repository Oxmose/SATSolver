/*
 *
 *  INTERFACE BET
 *
*/

#ifndef DEF_IBET_H
#define DEF_IBET_H

// STD INCLUDES
#include <vector>   // std::vector

// PROJECT INCLUDES
#include "../Core/Clause.h"     // ClauseSet
#include "../Core/SATSolver.h"  // decision

enum BET_METHOD
{
    NORM,
    RAND0,
    RAND1,
    MOMS,
    DLIS,
    DLIS1
};

typedef struct decision decision;
class IBet
{
    public:
        virtual ~IBet(){}
        virtual decision takeABet(std::vector<Clause> &p_clauses, const std::set<int> &p_unsatClauses, std::map<int,int> &p_valuation) = 0;

    private:
}; // IBet

#endif // DEF_IBET_H