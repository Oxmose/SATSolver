/*
 *
 *  CLASS MOMSBet
 *
*/

// STD INCLUDES
#include <vector>   // std::vector
#include <map>      // std::map

// PROJECT INCLUDES
#include "../Core/Clause.h"     // ClauseSet
#include "../Core/SATSolver.h"  // decision

// INHERITANCE CLASS
#include "IBet.h"

// HEADER INCLUDE
#include "MOMSBet.h"

using namespace std;

MOMSBet::~MOMSBet()
{
} // ~MOMSBet()

decision MOMSBet::takeABet(const vector<ClauseSet> &p_formula, vector<decision> &p_currentAssignement)
{
    OUTDEBUG("MOMS bet");

    vector<Clause> unassignedLits;
    int selectedUnassigned = -1;

    int minSize = -1;
    // Gather minimal clauses
    for(auto it = p_formula[0].begin() ; it != p_formula[0].end() ; ++it)
    {
        if(!it->getLiterals(0).empty())
        {
            if(it->getLiterals(0).size() < minSize || minSize == -1)
            {
                minSize = it->getLiterals(0).size();
                unassignedLits.clear();
                unassignedLits.push_back(*it);
            }
            else if(it->getLiterals(0).size() == minSize)
            {
                unassignedLits.push_back(*it);
            }
        }
    }

    map<int, unsigned int> appearences;
    bool value = true;
    unsigned int max = 0;
    for(unsigned int i = 0; i < unassignedLits.size(); ++i)
    {
        for(auto it = unassignedLits[i].getLiterals(0).begin(); it != unassignedLits[i].getLiterals(0).end(); ++it)
        {
            int lit = (it->bar ? -(it->index) : it->index);
            if(appearences.find(lit) == appearences.end())
            {
                appearences.emplace(lit, 1);
                if(max < 1)
                {
                    max = 1;
                    if(lit < 0)
                    {
                        selectedUnassigned = -lit;
                        value = false;
                    }
                    else
                    {
                        selectedUnassigned = lit;
                        value = true;
                    }
                }
            }
            else
            {
                ++appearences[lit];
                if(max < appearences[lit])
                {
                    max = appearences[lit];
                    if(lit < 0)
                    {
                        selectedUnassigned = -lit;
                        value = false;
                    }
                    else
                    {
                        selectedUnassigned = lit;
                        value = true;
                    }
                }
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
