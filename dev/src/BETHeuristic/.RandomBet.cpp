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
#include "../Core/Clause.h"     // ClauseSet
#include "../Core/SATSolver.h"  // decision

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
} // ~RandomBet()

decision RandomBet::takeABet(SATSolver &p_solver)
{
    OUTDEBUG("Random bet");

    set<int> unassignedLits;
    int selectedUnassigned = -1;
    bool value = true;

    // Retreive data
    map<int, set<int>> unsatLitsByClauses = p_solver.getAliveVars();
    set<int> unsatClausesIndex = p_solver.getUnsatClauses();
    vector<Clause> clauses = p_solver.getClauses();

    // Gather non assigned literals
    for(int iClause: unsatClausesIndex)
    {
        for(auto iVar: unsatLitsByClauses[iClause])
        {    
            int polLit = (clauses[iClause].getLiterals()[iVar]) ? -iVar : iVar;
            unassignedLits.emplace(polLit);       
        }
    }

    if(unassignedLits.size() != 0)
    {
        // Select a random literal
        unsigned int index = rand() % (unassignedLits.size() - 1);
        set<int>::iterator it = unassignedLits.begin();

        for(unsigned int i = 0; i < index; ++i)
            ++it;

        selectedUnassigned = abs(*it);
        if(!m_randomBet)
        {
            if(*it > 0)
                value = true;
            else
                value = false;
        }
        else
        {
            int bet = rand() % 9;
            
            // Randomize bet    
            if(bet > 4)
                value = false;
        }
    }

    decision bet = decision(selectedUnassigned, value, true);
    
    if(selectedUnassigned == -1)
    {
        OUTERROR("Critical issue in RandBet, a bet should exist.");
    }

    OUTDEBUG("Taking bet: " << selectedUnassigned << " to " << value);
    return bet;
} // decision takeABet(SATSolver &p_solver)

