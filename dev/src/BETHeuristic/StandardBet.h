/*
 *
 *  CLASS StandardBet
 *
*/

#ifndef DEF_STANDARDBET_H
#define DEF_STANDARDBET_H

// STD INCLUDES
#include <vector>   // std::vector
#include <set>      // std::set

// PROJECT INCLUDES
#include "../Core/Clause.h"     // ClauseSet
#include "../Core/SATSolver.h"  // decision

// INHERITANCE CLASS
#include "IBet.h"

class StandardBet : public IBet
{
    public:
        virtual ~StandardBet();
        virtual decision takeABet(SATSolver &p_solver);

    private:
}; // StandardBet

#endif // DEF_STANDARDBET_H
