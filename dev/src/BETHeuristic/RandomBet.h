/*
 *
 *  CLASS RandomBet
 *
*/

#ifndef DEF_RANDOMBET_H
#define DEF_RANDOMBET_H

// STD INCLUDES
#include <vector>   // std::vector
#include <ctime>    // std::time
#include <cstdlib>  // std::rand
#include <set>      // std::set

// PROJECT INCLUDES
#include "../Core/Clause.h"     // ClauseSet
#include "../Core/SATSolver.h"  // decision

// INHERITANCE CLASS
#include "IBet.h"

class RandomBet : public IBet
{
    public:
        RandomBet(bool p_randomBet);
        virtual ~RandomBet();
        virtual decision takeABet(SATSolver &p_solver);

    private:
        bool m_randomBet;
}; // RandomBet

#endif // DEF_RANDOMBET_H
