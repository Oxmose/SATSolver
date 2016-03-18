/*
 *
 *  CLASS DLISBet
 *
*/

#ifndef DEF_DLISBET_H
#define DEF_DLISBET_H

// STD INCLUDES
#include <vector>   // std::vector
#include <map>      // std::map
#include <cmath>    // std::exp2, std::abs

// PROJECT INCLUDES
#include "../Core/Clause.h"     // ClauseSet
#include "../Core/SATSolver.h"  // decision

// INHERITANCE CLASS
#include "IBet.h"

class DLISBet : public IBet
{
    public:
        DLISBet(bool p_scoreMethod);
        virtual ~DLISBet();
        virtual decision takeABet(std::vector<Clause> &p_clauses, const std::set<int> &p_unsatClauses, std::map<int,int> &p_valuation);

    private:
        bool m_scoreMethod;
}; // DLISBet

#endif // DEF_DLISBET_H
