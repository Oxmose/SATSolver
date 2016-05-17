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
#include "../Core/SATSolver.h"  // Solver

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

int DLISBet::takeABet(SATSolver *p_solver)
{
    OUTDEBUG(fprintf(stderr, "DLIS bet\n"));
    assert(!p_solver->unsat_clauses.empty());

    map<int,double> scoreOf;
    for(auto iClause : p_solver->unsat_clauses)
        for(auto l : p_solver->formula[iClause].literal)
            if(p_solver->valuation[abs(l)] == -1)
            {
                if(scoreOf.find(l) == scoreOf.end())
                    scoreOf[l] = 0;
                else
                    scoreOf[l] += (m_scoreMethod) ? exp2((double)-p_solver->formula[iClause].literal.size()) : 1;
            }

    int maxScore = 0;
    int the_lit = 0;
    for(auto s : scoreOf)
        if(s.second > maxScore)
        {
            maxScore = s.second;
            the_lit = s.first;
        }

    assert(the_lit != 0);
    return the_lit;
} // void takeABet(SATSolver *p_solver)
