/*
 *
 *  CLASS FORGETBet
 *
*/

// STD INCLUDES
#include <vector>   // vector
#include <map>      // map
#include <cmath>    // exp2, abs
#include <set>      // set

// PROJECT INCLUDES
#include "../Core/Clause.h"     // ClauseSet
#include "../Core/SATSolver.h"  // decision

// INHERITANCE CLASS
#include "IBet.h"

// HEADER INCLUDE
#include "FORGETBet.h"

using namespace std;

FORGETBet::~FORGETBet()
{
} // ~FORGETBet()

decision FORGETBet::takeABet(vector<Clause> &p_clauses, const set<int> &p_unsatClauses, map<int,int> &p_valuation)
{
    OUTDEBUG("FORGET bet");

    int firstUnassigned = -1;
    double max = 0;
    bool value = false;

    // Contains the literals and the number of clauses they appear in (or score if m_scoreMethod is true)
    map<int, double> unassignedLits;



    decision bet = decision(firstUnassigned, value, true);
    
    if(firstUnassigned == -1)
    {
        OUTERROR("Critical issue in FORGETBet, a bet should exist.");
    }

    OUTDEBUG("Taking bet: " << firstUnassigned << " to " << value);
    return bet;
} // decision takeABet(vector<Clause>&, const set<int>&, map<int,int>&)

