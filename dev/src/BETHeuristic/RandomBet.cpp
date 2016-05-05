/*
 *
 *  CLASS RandomBet
 *
*/

// STD INCLUDES
#include <vector>   // std::vector
#include <ctime>    // std::time
#include <cstdlib>  // std::rand
#include <set>      // std::set

// PROJECT INCLUDES
#include "../NewCore/SATSolver.h"  // Solver

// INHERITANCE CLASS
#include "IBet.h"

// HEADER INCLUDE
#include "RandomBet.h"

using namespace std;

RandomBet::RandomBet(bool p_randomBet)
{
    m_randomBet = p_randomBet;
    srand(time(NULL));
} // RandomBet()

RandomBet::~RandomBet()
{
}

void RandomBet::takeABet(SATSolver *p_solver)
{
    OUTDEBUG(fprintf(stderr, "Random bet"));
    set<int> unassignedLits;

    assert(!p_solver->unsat_clauses.empty());
    OUTDEBUG(fprintf(stderr, "Current level is now %d.\n", p_solver->curr_level));

    for(auto it = *p_solver->unsat_clauses.begin(); it != *p_solver->unsat_clauses.end(); ++it)
        for(auto l : p_solver->formula[it].literal)
	        if(p_solver->valuation[abs(l)] == -1)
                	unassignedLits.emplace(l);
    

    if(unassignedLits.size() != 0)
    {
        // Select a random literal
        unsigned int index = rand() % (unassignedLits.size() - 1);
        set<int>::iterator it = unassignedLits.begin();

        for(unsigned int i = 0; i < index; ++i)
            ++it;

        bool value = true;
        int selectedUnassigned = abs(*it);
        if(!m_randomBet)
        {
            if(*it < 0)
                value = false;
        }
        else
        {
            int bet = rand() % 9;
            
            // Randomize bet    
            if(bet > 4)
                value = false;
        }
        OUTDEBUG(fprintf(stderr,"Taking bet %d to %d.\n", *it, value));
        p_solver->decision_stack.push_back(make_pair(selectedUnassigned, value));
        if(settings_s.cl_s)
	        p_solver->conflict_graph.add_node(selectedUnassigned, make_pair(p_solver->curr_level, value));
        return; 
    }
    else
        assert(false);
} // void takeABet(SATSolver *p_solver)

