/*
 *
 *  CLASS StandardBet
 *
*/

// STD INCLUDES
#include <vector>   // std::vector

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

decision StandardBet::takeABet(const vector<ClauseSet> &p_formula, vector<decision> &p_currentAssignement)
{
    OUTDEBUG("Standard bet");
    int firstUnassigned = -1;

    for(auto it = p_formula[0].begin() ; it != p_formula[0].end() ; ++it)
        if(!it->getLiterals(0).empty())
        {
            firstUnassigned = it->getLiterals(0).begin()->index;
            break;
        }

    decision bet = decision(firstUnassigned,true,true);

    if(p_formula[0].empty())
        return bet;

    OUTDEBUG("Taking bet: " << firstUnassigned << " to True");
    p_currentAssignement.push_back(bet);
    return bet;
} // decision takeABet(const std::vector<ClauseSet>&, std::vector<decision>&)

