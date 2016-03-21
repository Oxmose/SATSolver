/*
 *
 *  CLASS DLISBet
 *
*/

// STD INCLUDES
#include <vector>   // vector
#include <map>      // map
#include <cmath>    // exp2, abs

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

decision DLISBet::takeABet(vector<Clause> &p_clauses, const set<int> &p_unsatClauses, map<int,int> &p_valuation)
{
    OUTDEBUG("DLIS bet");

    int firstUnassigned = -1;
    double max = 0;
    bool value = false;

    // Contains the literals and the number of clauses they appear in (or score if m_scoreMethod is true)
    map<int, double> unassignedLits;

    if(m_scoreMethod)
    {
        for(int iClause: p_unsatClauses)
        {
            double clauseSize = (double) (p_clauses[iClause].getLiterals().size());
            for(auto lit: p_clauses[iClause].getLiterals())
            {
                if(p_valuation[lit.first] == -1)
                {
                    int polLit = (lit.second ? -lit.first : lit.first);
                    // If we never encountred the literal
                    if(unassignedLits.find(polLit) == unassignedLits.end())
                        unassignedLits.emplace(polLit, exp2(-clauseSize));
                    else
                        unassignedLits[polLit] += exp2(-clauseSize);

                    if(unassignedLits[polLit] > max)
                    {
                        max = unassignedLits[polLit];
                        firstUnassigned = abs(polLit);
                        if(polLit < 0)
                            value = false;
                        else
                            value = true;
                    }
                }
            }
        }
    }
    else
    {
        for(int iClause: p_unsatClauses)
        {
            for(auto lit: p_clauses[iClause].getLiterals())
            {
                if(p_valuation[lit.first] == -1)
                {
                    int polLit = (lit.second ? -lit.first : lit.first);
                    // If we never encountred the literal
                    if(unassignedLits.find(polLit) == unassignedLits.end())
                        unassignedLits.emplace(polLit, 1);
                    else
                        ++unassignedLits[polLit];

                    if(unassignedLits[polLit] > max)
                    {
                        max = unassignedLits[polLit];
                        firstUnassigned = abs(polLit);
                        if(polLit < 0)
                            value = false;
                        else
                            value = true;
                    }
                }
            }
        }
    }

    decision bet = decision(firstUnassigned, value, true);
    
    if(firstUnassigned == -1)
    {
        OUTERROR("Critical issue in DLISBet, a bet should exist " << *p_unsatClauses.begin() << " " << p_clauses[*p_unsatClauses.begin()].toStr());
    }

    OUTDEBUG("Taking bet: " << firstUnassigned << " to " << value);
    return bet;
} // decision takeABet(vector<Clause>&, const set<int>&, map<int,int>&)

