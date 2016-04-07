/*
 *
 *  CLASS MOMSBet
 *
*/

// STD INCLUDES
#include <vector>   // std::vector
#include <map>      // std::map
#include <set>      // std::set

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

decision MOMSBet::takeABet(vector<Clause> &p_clauses, const set<int> &p_unsatClauses, map<int,int> &p_valuation)
{
    OUTDEBUG("MOMS bet");
    map<int, int> clausesSizes;
    map<int, unsigned int> unassignedLits;

    int min = -1;
    int selectedUnassigned = -1;
    bool value = true;

    // Gather minimal clauses
    for(int iClause: p_unsatClauses)
    {
        int clauseSize = 0;
        for(auto lit: p_clauses[iClause].getLiterals())
        {
            // Get the size of the alive lits in clause 
            if(p_valuation[lit.first] == -1)
            {
                ++clauseSize;
            }
        }
        if(clauseSize < min || min == -1)
            min = clauseSize;
        clausesSizes.emplace(iClause, clauseSize);
    }    

    for(pair<int, int> entry : clausesSizes)
    {
        if(entry.second == min)
        {            
            for(auto iVar: p_clauses[entry.first].getLiterals())
            {
                if(p_valuation[iVar.first] == -1)
                {
                    int polLit = iVar.second ? -iVar.first : iVar.first;
                    // If we never encountred the literal
                    if(unassignedLits.find(polLit) == unassignedLits.end())
                        unassignedLits.emplace(polLit, 1);
                    else
                        ++unassignedLits[polLit];
                }
           
            }
        }
    }

    unsigned int max = 0;
    for(pair<int, unsigned int> entry : unassignedLits)
    {
        if(max < entry.second)
        {
            selectedUnassigned = entry.first;
            max = entry.second;
        }    
    }

    if(selectedUnassigned < 0)
    {
        selectedUnassigned = -selectedUnassigned;
        value = false;
    }
    else
        value = true;

    decision bet = decision(selectedUnassigned, value, true);
    
    if(selectedUnassigned == -1)
    {
        OUTERROR("Critical issue in MOMSBet, a bet should exist .");
    }

    OUTDEBUG("Taking bet: " << selectedUnassigned << " to " << value);
    return bet;
} // decision takeABet(vector<Clause>&, const set<int>&, map<int,int>&)
