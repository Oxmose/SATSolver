/*
 *
 *  CLASS MOMSBet
 *
*/

#ifndef DEF_MOMSBET_H
#define DEF_MOMSBET_H

// STD INCLUDES
#include <map>      // std::map
#include <iterator> // std::distance

// PROJECT INCLUDES
#include "../Core/SATSolver.h"  // Solver

// INHERITANCE CLASS
#include "IBet.h"

class MOMSBet : public IBet
{
    public:
        virtual ~MOMSBet();
        virtual int takeABet(SATSolver *p_solver);
    private:

}; // MOMSBet

#endif // DEF_MOMSBET_H
