/*
 *
 *  CLASS StandardBet
 *
*/

#ifndef DEF_STANDARDBET_H
#define DEF_STANDARDBET_H

// PROJECT INCLUDES
#include "../Core/SATSolver.h"  // Solver

// INHERITANCE CLASS
#include "IBet.h"

class StandardBet : public IBet
{
    public:
        StandardBet();
        virtual ~StandardBet();
        virtual int takeABet(SATSolver *p_solver);

    private:
}; // StandardBet

#endif // DEF_STANDARDBET_H
