/*
 *
 *  CLASS VSIDSBet
 *
*/

// STD INCLUDES
#include <vector>   // vector
#include <cmath>    // abs

// PROJECT INCLUDES
#include "../NewCore/SATSolver.h"  // Solver

// INHERITANCE CLASS
#include "IBet.h"

// HEADER INCLUDE
#include "VSIDSBet.h"

using namespace std;

VSIDSBet::~VSIDSBet()
{
}

void VSIDSBet::takeABet(SATSolver *p_solver)
{
    OUTDEBUG(fprintf(stderr, "VSIDS bet"));
    assert(!p_solver->unsat_clauses.empty());
    OUTDEBUG(fprintf(stderr, "Current level is now %d.\n", p_solver->curr_level));

    int firstUnassigned = 0;
    double max = -1;
    double score = 0;

    vector<pair<int,bool>> candidates;

    for(auto it = *p_solver->unsat_clauses.begin(); it != *p_solver->unsat_clauses.end(); ++it)
    {
        for(auto l : p_solver->formula[it].literal)
        {
	    if(p_solver->valuation[abs(l)] == -1 && max < (score = p_solver->getVarScores(abs(l))))
            {
            
                max = score;
                firstUnassigned = abs(l);
                candidates.clear();
                candidates.push_back(make_pair(firstUnassigned, true));
            }
            else if(p_solver->valuation[abs(l)] == -1 && max == p_solver->getVarScores(abs(l)))
            {
                firstUnassigned = abs(l);
                candidates.push_back(make_pair(firstUnassigned, true));
            }
        }
    }    

    if(candidates.size() != 0)
    {
        int iCandidate = rand() % candidates.size();
        firstUnassigned = candidates[iCandidate].first;
    }
    else
        assert(false);

    assert(firstUnassigned != 0);

    OUTDEBUG(fprintf(stderr,"Taking bet %d.\n", abs(firstUnassigned)));
    p_solver->decision_stack.push_back(make_pair(abs(firstUnassigned), true));
    if(settings_s.cl_s)
	    p_solver->conflict_graph.add_node(abs(firstUnassigned), make_pair(p_solver->curr_level, true));
    return;
} // void takeABet(SATSolver *p_solver)

