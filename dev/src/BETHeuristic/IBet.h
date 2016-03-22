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
        virtual decision takeABet(SATSolver &p_solver) = 0;

    private:
}; // IBet

#endif // DEF_IBET_H
