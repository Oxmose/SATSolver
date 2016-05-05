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
#include "../NewCore/SATSolver.h"  // Solver

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
}

void DLISBet::takeABet(SATSolver *p_solver)
{
    OUTDEBUG(fprintf(stderr, "DLIS bet"));
    assert(!p_solver->unsat_clauses.empty());
    OUTDEBUG(fprintf(stderr, "Current level is now %d.\n", p_solver->curr_level));

    int firstUnassigned = -1;
    double max = 0;
    bool value = false;

    // Contains the literals and the number of clauses they appear in (or score if m_scoreMethod is true)
    map<int, double> unassignedLits;


    if(m_scoreMethod)
    {
        double clauseSize = 0;
        for(auto it = *p_solver->unsat_clauses.begin(); it != *p_solver->unsat_clauses.end(); ++it)
            for(auto l : p_solver->formula[it].literal)
                if(p_solver->valuation[abs(l)] == -1)
                    ++clauseSize;
            
        for(auto it = *p_solver->unsat_clauses.begin(); it != *p_solver->unsat_clauses.end(); ++it)
        {
            for(auto l : p_solver->formula[it].literal)
            {
                if(p_solver->valuation[abs(l)] == -1)
                {
                    // If we never encountred the literal
                    if(unassignedLits.find(l) == unassignedLits.end())
                        unassignedLits.emplace(l, exp2(-clauseSize));
                    else
                        unassignedLits[l] += exp2(-clauseSize);

                    if(unassignedLits[l] > max)
                    {
                        max = unassignedLits[l];
                        firstUnassigned = abs(l);
                        if(l < 0)
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
        for(auto it = *p_solver->unsat_clauses.begin(); it != *p_solver->unsat_clauses.end(); ++it)
        {
            for(auto l : p_solver->formula[it].literal)
            {
                if(p_solver->valuation[abs(l)] == -1)
                {                  
                    // If we never encountred the literal
                    if(unassignedLits.find(l) == unassignedLits.end())
                        unassignedLits.emplace(l, 1);
                    else
                        ++unassignedLits[l];

                    if(unassignedLits[l] > max)
                    {
                        max = unassignedLits[l];
                        firstUnassigned = abs(l);

                        if(l < 0)
                            value = false;
                        else
                            value = true;
                    }
                }
            }
        }
    }

    OUTDEBUG(fprintf(stderr,"Taking bet %d.\n", firstUnassigned));
    p_solver->decision_stack.push_back(make_pair(firstUnassigned, value));
    if(settings_s.cl_s)
        p_solver->conflict_graph.add_node(firstUnassigned, make_pair(p_solver->curr_level, value));
    return;
} // void takeABet(SATSolver *p_solver)

