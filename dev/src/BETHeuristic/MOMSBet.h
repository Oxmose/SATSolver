/*
 *
 *  CLASS MOMSBet
 *
*/

#ifndef DEF_MOMSBET_H
#define DEF_MOMSBET_H

// STD INCLUDES
#include <vector>   // std::vector
#include <map>      // std::map

// PROJECT INCLUDES
#include "../Core/Clause.h"     // ClauseSet
#include "../Core/SATSolver.h"  // decision

// INHERITANCE CLASS
#include "IBet.h"

class MOMSBet : public IBet
{
    public:
        virtual ~MOMSBet();
        virtual decision takeABet(const std::vector<ClauseSet> &p_formula, std::vector<decision> &p_currentAssignement);
    private:

}; // MOMSBet

#endif // DEF_MOMSBET_H
