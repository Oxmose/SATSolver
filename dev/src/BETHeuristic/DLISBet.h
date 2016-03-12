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
#include <utility>  // std::pair

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
        virtual decision takeABet(const std::vector<ClauseSet> &p_formula, std::vector<decision> &p_currentAssignement);

    private:
        bool m_scoreMethod;
}; // DLISBet

#endif // DEF_DLISBET_H
