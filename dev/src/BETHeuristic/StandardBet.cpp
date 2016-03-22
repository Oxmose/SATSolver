/*
 *
 *  CLASS StandardBet
 *
*/

// STD INCLUDES
#include <vector>   // vector
#include <set>      // std::set

// PROJECT INCLUDES
#include "../Core/Clause.h"     // ClauseSet
#include "../Core/SATSolver.h"  // decision

// INHERITANCE CLASS
#include "IBet.h"

// HEADER INCLUDE
#include "StandardBet.h"

using namespace std;

StandardBet::~StandardBet()
{
} // ~StandardBet()

decision StandardBet::takeABet(std::vector<Clause> &p_clauses, const std::set<int> &p_unsatClauses, std::map<int,int> &p_valuation)
{
    OUTDEBUG("Standard bet");
    int firstUnassigned = -1;

    for(int iClause: p_unsatClauses)
    {
        for(auto lit: p_clauses[iClause].getLiterals())
            if(p_valuation[lit.first] == -1)
            {
                OUTDEBUG(iClause);
                //first = key, second = value
                //first = index, second = polarization
                firstUnassigned = lit.first;
                break;
            }
        if(firstUnassigned != -1)
            break;
    }

    decision bet = decision(firstUnassigned,true,true);

    if(firstUnassigned == -1)
    {

        printf("%d\n", p_unsatClauses.size());
        OUTERROR("Critical issue in StdBet, a bet should exist " << *p_unsatClauses.begin() << " " << p_clauses[*p_unsatClauses.begin()].toStr());
    }

    OUTDEBUG("Taking bet: " << firstUnassigned << " to True");
    return bet;
} // decision takeABet(const std::vector<ClauseSet>&, std::vector<decision>&)
