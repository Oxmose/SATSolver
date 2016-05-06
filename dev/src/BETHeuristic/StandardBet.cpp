/*
 *
 *  CLASS StandardBet
 *
*/

// PROJECT INCLUDES
#include "../NewCore/SATSolver.h"  // Solver

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

void StandardBet::takeABet(SATSolver *p_solver)
{
    OUTDEBUG(fprintf(stderr, "Standard bet"));
    assert(!p_solver->unsat_clauses.empty());
    OUTDEBUG(fprintf(stderr, "Current level is now %d.\n", p_solver->curr_level));

    for(auto l : p_solver->formula[*p_solver->unsat_clauses.begin()].literal)
    {
	    if(p_solver->valuation[abs(l)] == -1)
	    {
		    OUTDEBUG(fprintf(stderr,"Taking bet %d.\n", abs(l)));
		    p_solver->decision_stack.push_back(make_pair(abs(l),true));
		    if(settings_s.cl_s)
			    p_solver->conflict_graph.add_node(abs(l), make_pair(p_solver->curr_level, true));
		    return;
	    }
    }
    
    assert(false);
} // void takeABet(SATSolver *p_solver)
