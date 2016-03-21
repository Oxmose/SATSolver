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

decision MOMSBet::takeABet(vector<Clause> &p_clauses, const set<int> &p_unsatClauses, map<int,int> &p_valuation)
{
    OUTDEBUG("MOMS bet");
    map<int, unsigned int> unassignedLits;
    unsigned int max = 0;
    int selectedUnassigned = -1;
    bool value = true;
    int minSize = -1;

    // Gather minimal clauses
    for(int iClause: p_unsatClauses)
    {
        int clauseSize = p_clauses[iClause].getLiterals().size();
        if(clauseSize <= minSize || minSize == -1)
        {
            if(clauseSize < minSize)
            {
                unassignedLits.clear();
                max = 0;
            }

            minSize = clauseSize;           
            for(auto lit: p_clauses[iClause].getLiterals())
            {
	            if(p_valuation[lit.first] == -1)
	            {
                    int polLit = (lit.second ? -lit.first : lit.first);
                    
                    if(unassignedLits.find(polLit) == unassignedLits.end())
                        unassignedLits.emplace(polLit, 1);
                    else
                        ++unassignedLits[polLit];

                    if(max < unassignedLits[polLit])
                    {
                        selectedUnassigned = lit.first;
                        max = unassignedLits[polLit];

                        if(polLit < 0)
                            value = false;
                        else
                            value = true;
                    }
                }                
            }
        }
    }    

    decision bet = decision(selectedUnassigned, value, true);
    
    if(selectedUnassigned == -1)
    {
        OUTERROR("Critical issue in MOMSBet, a bet should exist " << *p_unsatClauses.begin() << " " << p_clauses[*p_unsatClauses.begin()].toStr());
    }

    OUTDEBUG("Taking bet: " << selectedUnassigned << " to " << value);
    return bet;
} // decision takeABet(vector<Clause>&, const set<int>&, map<int,int>&)
