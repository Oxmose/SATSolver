/*
 *
 *  CLASS MOMSBet
 *
*/

// STD INCLUDES
#include <map>      // std::map
#include <iterator> // std::distance

// PROJECT INCLUDES
#include "../NewCore/SATSolver.h"  // Sovler

// INHERITANCE CLASS
#include "IBet.h"

// HEADER INCLUDE
#include "MOMSBet.h"

using namespace std;

MOMSBet::~MOMSBet()
{
}

void MOMSBet::takeABet(SATSolver *p_solver)
{
    OUTDEBUG(fprintf(stderr, "MOMS bet\n"));
    assert(!p_solver->unsat_clauses.empty());

    vector<int> interesting_clauses;
    unsigned int min_size = p_solver->formula[*p_solver->unsat_clauses.begin()].literal.size();
    for(auto iClause : p_solver->unsat_clauses)
    {
        if(min_size < p_solver->formula[iClause].literal.size())
        {
            interesting_clauses.clear();
            min_size = p_solver->formula[iClause].literal.size();
        }

        if(min_size == p_solver->formula[iClause].literal.size())
            interesting_clauses.push_back(iClause);
    }

    map<int,int> occ;
    for(auto iClause : interesting_clauses)
        for(auto l : p_solver->formula[iClause].literal)
            if(p_solver->valuation[abs(l)] == -1)
            {
                if(occ.find(l) == occ.end())
                    occ[l] = 0;
                else
                    occ[l]++;
            }

    int max_occ = -1;
    int the_lit = 0;
    for(auto o : occ)
        if(max_occ < o.second)
        {
            max_occ = o.second;
            the_lit = o.first;
        }

    assert(the_lit != 0);
    OUTDEBUG(fprintf(stderr,"Taking bet %d.\n", the_lit));
    p_solver->decision_stack.push_back(make_pair(the_lit,true));
} // void takeABet(SATSolver *p_solver)
