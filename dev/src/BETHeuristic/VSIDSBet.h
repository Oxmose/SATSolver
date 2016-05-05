/*
 *
 *  CLASS VSIDSBet
 *
*/

#ifndef DEF_VSIDSBET_H
#define DEF_VSIDSBET_H

// STD INCLUDES
#include <vector>   // std::vector
#include <cmath>    // std::exp2, std::abs

// PROJECT INCLUDES
#include "../NewCore/SATSolver.h"  // Solver

// INHERITANCE CLASS
#include "IBet.h"

// Score function designed for VSIDS heuristic
double VSIDSScoreFunction(double oldScore, bool inLearnedClause);

class VSIDSBet : public IBet
{
    public:
        virtual ~VSIDSBet();
        virtual void takeABet(SATSolver *p_solver);

    private:

}; // VSIDSBet

#endif // DEF_VSIDSBET_H
