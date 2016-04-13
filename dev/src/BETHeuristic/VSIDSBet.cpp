/*
 *
 *  CLASS VSIDSBet
 *
*/

// STD INCLUDES
#include <vector>   // vector
#include <map>      // map
#include <cmath>    // exp2, abs
#include <set>      // set
#include <memory>   // shared_ptr

// PROJECT INCLUDES
#include "../Core/Clause.h"     // ClauseSet
#include "../Core/SATSolver.h"  // decision

// INHERITANCE CLASS
#include "IBet.h"

// HEADER INCLUDE
#include "VSIDSBet.h"

using namespace std;

VSIDSBet::VSIDSBet(shared_ptr<SATSolver> p_solver)
{
    m_solver = p_solver;
}

VSIDSBet::~VSIDSBet()
{
} // ~VSIDSBet()

double VSIDSScoreFunction(double oldScore, bool inLearnedClause)
{
    double incrementConstant = 1;
    double pondConstant =  0.95;//What is in minisat according to http://www.cs.tau.ac.il/research/alexander.nadel/SAT-05_CBH_2.pdf
    if(inLearnedClause)
    {
        return (oldScore + incrementConstant);
    }
    else
    {
        return oldScore*pondConstant;
    }
}

decision VSIDSBet::takeABet(vector<Clause> &p_clauses, const set<int> &p_unsatClauses, map<int,int> &p_valuation)
{
    OUTDEBUG("VSIDS bet");

    int firstUnassigned = -1;
    double max = -1;
    double score = 0;
    bool value = false;

    std::vector<pair<int,bool>> candidates;
    for(int iClause: p_unsatClauses)
    {
        for(auto lit: p_clauses[iClause].getLiterals())
        {
            //if(m_solver->getVarScores(lit.first) != 0)
            //printf("%d %lf\n", lit.first, m_solver->getVarScores(lit.first));
            if(p_valuation[lit.first] == -1 && max < (score = m_solver->getVarScores(lit.first)))
            {
                max = score;
                //firstUnassigned = lit.first;
                //value = !lit.second;
                candidates.clear();
                candidates.push_back(make_pair(firstUnassigned,value));
            }
            else if(p_valuation[lit.first] == -1 && max == (score = m_solver->getVarScores(lit.first)))
            {
                firstUnassigned = lit.first;
                value = !lit.second;
                candidates.push_back(make_pair(firstUnassigned,value));
            }
        }
    }    

    if(candidates.size() != 0)
    {
        int iCandidate = rand()%candidates.size() - 1;
        firstUnassigned = candidates[iCandidate].first;
        value = candidates[iCandidate].second;
    }

    decision bet = decision(firstUnassigned, value, true);
    
    if(firstUnassigned == -1)
    {
        OUTERROR("Critical issue in VSIDSBet, a bet should exist.");
    }

    OUTDEBUG("Taking bet: " << firstUnassigned << " (" << m_solver->getVarScores(bet.index) << ")" << " to " << value);
    return bet;
} // decision takeABet(vector<Clause>&, const set<int>&, map<int,int>&)

