/*
 *
 *  CLASS DLISBet
 *
*/

// STD INCLUDES
#include <vector>   // vector
#include <map>      // map
#include <cmath>    // exp2, abs
#include <set>      // std::set

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

decision DLISBet::takeABet(SATSolver &p_solver)
{
    OUTDEBUG("DLIS bet");

    int firstUnassigned = -1;
    double max = 0;
    bool value = false;

    // Contains the literals and the number of clauses they appear in (or score if m_scoreMethod is true)
    map<int, double> unassignedLits;

    // Retreive data
    map<int, set<int>> unsatLitsByClauses = p_solver.getAliveVars();
    set<int> unsatClausesIndex = p_solver.getUnsatClauses();
    vector<Clause> clauses = p_solver.getClauses();

    if(m_scoreMethod)
    {
        for(int iClause: unsatClausesIndex)
        {
            double clauseSize = (double) (unsatLitsByClauses[iClause].size());
            for(auto iVar: unsatLitsByClauses[iClause])
            {
                int polLit = (clauses[iClause].getLiterals()[iVar]) ? -iVar : iVar;
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
    else
    {
        for(int iClause: unsatClausesIndex)
        {
            for(auto iVar: unsatLitsByClauses[iClause])
            {
                    int polLit = (clauses[iClause].getLiterals()[iVar]) ? -iVar : iVar;
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

    decision bet = decision(firstUnassigned, value, true);
    
    if(firstUnassigned == -1)
    {
        OUTERROR("Critical issue in DLISBet, a bet should exist.");
    }

    OUTDEBUG("Taking bet: " << firstUnassigned << " to " << value);
    return bet;
} // decision takeABet(SATSolver &p_solver)

