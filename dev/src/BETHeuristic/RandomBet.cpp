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

int RandomBet::takeABet(SATSolver *p_solver)
{
    OUTDEBUG(fprintf(stderr, "Random bet\n"));
    assert(p_solver->unsat_clauses.size() != 0);

    map<unsigned int,bool> alive_vars;
    for(auto iClause : p_solver->unsat_clauses)
        for(auto l : p_solver->formula[iClause].literal)
            if(p_solver->valuation[abs(l)] == -1)
                alive_vars[abs(l)] = true;

    assert(alive_vars.size() != 0);

    unsigned int rand_index = rand()%(alive_vars.size());
    int var = -1;
    unsigned int i = 0;
    for(auto v : alive_vars)
    {
        if(i == rand_index)
        {
            var = v.first;
            break;
        }
        i++;
    }
    
    assert(var != -1);
    int mod = 1;
    if(m_randomBet)
        mod = (rand()%2) ? 1 : -1;
    return var*mod;
} // void takeABet(SATSolver *p_solver)

