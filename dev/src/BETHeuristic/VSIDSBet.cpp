/*
 *
 *  CLASS VSIDSBet
 *
*/

// STD INCLUDES
#include <vector>   // vector
#include <map>      // map
#include <cmath>    // exp2, abs
#include <set>      // set
#include <memory>   // shared_ptr

// PROJECT INCLUDES
#include "../Core/Clause.h"     // ClauseSet
#include "../Core/SATSolver.h"  // decision

// INHERITANCE CLASS
#include "IBet.h"

// HEADER INCLUDE
#include "VSIDSBet.h"

using namespace std;

VSIDSBet::VSIDSBet(shared_ptr<SATSolver> p_solver)
{
    m_solver = p_solver;
}

VSIDSBet::~VSIDSBet()
{
} // ~VSIDSBet()

decision VSIDSBet::takeABet(vector<Clause> &p_clauses, const set<int> &p_unsatClauses, map<int,int> &p_valuation)
{
    OUTDEBUG("VSIDS bet");

    int firstUnassigned = -1;
    double max = 0;
    bool value = false;

    // Contains the literals and the number of clauses they appear in (or score if m_scoreMethod is true)
    map<int, double> unassignedLits;



    decision bet = decision(firstUnassigned, value, true);
    
    if(firstUnassigned == -1)
    {
        OUTERROR("Critical issue in VSIDSBet, a bet should exist.");
    }

    OUTDEBUG("Taking bet: " << firstUnassigned << " to " << value);
    return bet;
} // decision takeABet(vector<Clause>&, const set<int>&, map<int,int>&)

