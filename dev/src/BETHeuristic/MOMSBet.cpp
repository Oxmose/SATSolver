/*
 *
 *  CLASS MOMSBet
 *
*/

// STD INCLUDES
#include <vector>   // std::vector
#include <map>      // std::map
#include <set>

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

decision MOMSBet::takeABet(SATSolver &p_solver)
{
    OUTDEBUG("MOMS bet");
    map<int, unsigned int> clausesSizes;
    map<int, unsigned int> unassignedLits;

    int min = -1;
    int selectedUnassigned = -1;
    bool value = true;

    // Retreive data
    map<int, set<int>> unsatLitsByClauses;

    // Gather minimal clauses
    for(int iClause: p_unsatClauses)
    {
        unsigned int clauseSize = p_clauses[iClause].getLiterals().size();
       	if(clauseSize < min || min == -1)
			min = clauseSize;
		clausesSizes.emplace(iClause, clauseSize);
    }    

	for(pair<int, unsigned int> entry : clausesSizes)
    {
		if(entry.second == min)
		{
			for(auto lit: p_clauses[entry.first].getLiterals())
            {
                if(p_valuation[lit.first] == -1)
                {
                    int polLit = (lit.second ? -lit.first : lit.first);
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
        OUTERROR("Critical issue in MOMSBet, a bet should exist " << *p_unsatClauses.begin() << " " << p_clauses[*p_unsatClauses.begin()].toStr());
    }

    OUTDEBUG("Taking bet: " << selectedUnassigned << " to " << value);
    return bet;
} // decision takeABet(SATSolver&)
