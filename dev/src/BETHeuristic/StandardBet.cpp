/*
 *
 *  CLASS StandardBet
 *
*/

// STD INCLUDES
#include <vector>   // vector

// PROJECT INCLUDES
#include "../Core/Clause.h"     // ClauseSet
#include "../Core/SATSolver.h"  // decision
#include <set>      // std::set

// INHERITANCE CLASS
#include "IBet.h"

// HEADER INCLUDE
#include "StandardBet.h"

using namespace std;

StandardBet::~StandardBet()
{
} // ~StandardBet()

decision StandardBet::takeABet(SATSolver &p_solver)
{
    OUTDEBUG("Standard bet");
    int firstUnassigned = -1;

    // Retreive data
    map<int, set<int>> unsatLitsByClauses = p_solver.getAliveVars();
    set<int> unsatClausesIndex = p_solver.getUnsatClauses();
    vector<Clause> clauses = p_solver.getClauses();

    for(int iClause: unsatClausesIndex)
    {
        for(auto lit: unsatLitsByClauses[iClause])
        {
            firstUnassigned = lit;
            break;
        }
        if(firstUnassigned != -1)
            break;
    }

    decision bet = decision(firstUnassigned, true, true);

    if(firstUnassigned == -1)
    {
        OUTERROR("Critical issue in StdBet, a bet should exist.");
    }

    OUTDEBUG("Taking bet: " << firstUnassigned << " to True");
    return bet;
} // decision takeABet(SATSolver &p_solver)

