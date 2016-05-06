/*
 *
 *  CLASS MOMSBet
 *
*/

// STD INCLUDES
#include <map>      // std::map
#include <iterator> // std::distance

// PROJECT INCLUDES
#include "../NewCore/SATSolver.h"  // Sovler

// INHERITANCE CLASS
#include "IBet.h"

// HEADER INCLUDE
#include "MOMSBet.h"

using namespace std;

MOMSBet::~MOMSBet()
{
}

void MOMSBet::takeABet(SATSolver *p_solver)
{
    OUTDEBUG(fprintf(stderr, "MOMS bet"));
    assert(!p_solver->unsat_clauses.empty());
    OUTDEBUG(fprintf(stderr, "Current level is now %d.\n", p_solver->curr_level));

    map<int, int> clausesSizes;
    map<int, unsigned int> unassignedLits;

    int min = -1;
    int selectedUnassigned = -1;

    // Gather minimal clauses
    for(auto it = *p_solver->unsat_clauses.begin(); it != *p_solver->unsat_clauses.end(); ++it)
    {
        int clauseSize = 0;
        for(auto l : p_solver->formula[it].literal)
        {
	    if(p_solver->valuation[abs(l)] == -1)
            {
                ++clauseSize;
            }
        }
        if(clauseSize != 0 && (clauseSize < min || min == -1))
            min = clauseSize;

        clausesSizes.emplace(it, clauseSize);
    }
    assert(min != -1);

    for(pair<int, int> entry : clausesSizes)
    {
        if(entry.second == min)
        {           
            for(auto l : p_solver->formula[entry.first].literal)
            {
                if(p_solver->valuation[abs(l)] == -1)
                {
                    // If we never encountred the literal
                    if(unassignedLits.find(l) == unassignedLits.end())
                        unassignedLits.emplace(l, 1);
                    else
                        ++unassignedLits[l];
                }
           
            }
        }
    }
    unsigned int max = 0;
    for(pair<int, unsigned int> entry : unassignedLits)
    {
        if(max < entry.second)
        {
            selectedUnassigned = entry.first;
            max = entry.second;
        }    
    }

    OUTDEBUG(fprintf(stderr,"Taking bet %d.\n", abs(selectedUnassigned)));
    p_solver->decision_stack.push_back(make_pair(abs(selectedUnassigned), true));
    if(settings_s.cl_s)
	    p_solver->conflict_graph.add_node(abs(selectedUnassigned), make_pair(p_solver->curr_level, true));
    return;
} // void takeABet(SATSolver *p_solver)
