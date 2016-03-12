/*
 *
 *  CLASS DLISBet
 *
*/

// STD INCLUDES
#include <vector>   // std::vector
#include <map>      // std::map
#include <utility>  // std::pair

// PROJECT INCLUDES
#include "../Core/Clause.h"     // ClauseSet
#include "../Core/SATSolver.h"  // decision

// INHERITANCE CLASS
#include "IBet.h"

// HEADER INCLUDE
#include "DLISBet.h"

using namespace std;

DLISBet::DLISBet(bool p_scoreMethod)
{
    m_scoreMethod = p_scoreMethod;
} // DLISBet()

DLISBet::~DLISBet()
{
} // ~DLISBet()

decision DLISBet::takeABet(const vector<ClauseSet> &p_formula, vector<decision> &p_currentAssignement)
{
    OUTDEBUG("DLIS bet");
    map<int, unsigned int> unassignedLits;
    int selectedUnassigned = -1;

    // Gather non assigned literals
    for(auto it = p_formula[0].begin() ; it != p_formula[0].end() ; ++it)
    {
        if(!it->getLiterals(0).empty())
        {
            for(auto it2 = it->getLiterals(0).begin(); it2 != it->getLiterals(0).end(); ++it2)
            {
                int value = it2->index;
                int lit;

                if(it2->bar)
                    lit = -(it2->index);
                else
                    lit = it2->index;

                if(unassignedLits.find(lit) == unassignedLits.end())
                    unassignedLits.emplace(lit, 1);
                else
                    ++unassignedLits[lit];
            }
        }
    }

    bool value = true;
    if(unassignedLits.size() > 0)
    {
        // Get the literal that appear the most
        int max = 0;
        for(pair<int, unsigned int> entry : unassignedLits)
        {
            if(entry.second > max)
            {
                max = entry.second;
                selectedUnassigned = abs(entry.first);
                if(entry.first < 0)
                    value = false;
                else 
                    value = true;
            }
        }

        
    }
    decision bet = decision(selectedUnassigned, value, true);

    if(p_formula[0].empty())
        return bet;

    OUTDEBUG("Taking bet: " << selectedUnassigned << " to " << (value ? "True" : "False"));
    p_currentAssignement.push_back(bet);
    return bet;
} // decision takeABet(const std::vector<ClauseSet>&, std::vector<decision>&)

