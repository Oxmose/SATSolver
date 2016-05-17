/*
 *
 *  CLASS StandardBet
 *
*/

// PROJECT INCLUDES
#include "../Core/SATSolver.h"  // Solver

#include "../Global/Global.h"

// INHERITANCE CLASS
#include "IBet.h"

// HEADER INCLUDE
#include "StandardBet.h"

using namespace std;

StandardBet::~StandardBet()
{
}

StandardBet::StandardBet()
{
}

int StandardBet::takeABet(SATSolver *p_solver)
{
    OUTDEBUG(fprintf(stderr, "Standard bet\n"));
    assert(!p_solver->unsat_clauses.empty());

    for(auto l : p_solver->formula[*p_solver->unsat_clauses.begin()].literal)
	    if(p_solver->valuation[abs(l)] == -1)
		    return abs(l);
	    
    assert(false);
} // void takeABet(SATSolver *p_solver)
