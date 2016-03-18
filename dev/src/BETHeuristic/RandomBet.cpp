/*
 *
 *  CLASS RandomBet
 *
*/

// STD INCLUDES
#include <vector>   // std::vector
#include <ctime>    // std::time
#include <cstdlib>  // std::rand

// PROJECT INCLUDES
#include "../Core/Clause.h"     // ClauseSet
#include "../Core/SATSolver.h"  // decision

// INHERITANCE CLASS
#include "IBet.h"

// HEADER INCLUDE
#include "RandomBet.h"

using namespace std;

RandomBet::RandomBet(bool p_randomBet)
{
    m_randomBet = p_randomBet;
    srand(time(NULL));
} // RandomBet()

RandomBet::~RandomBet()
{
} // ~RandomBet()

decision RandomBet::takeABet(vector<Clause> &p_clauses, const set<int> &p_unsatClauses, map<int,int> &p_valuation)
{
    OUTDEBUG("Random bet");

    set<int> unassignedLits;
    int selectedUnassigned = -1;
    bool value = true;

    // Gather non assigned literals
    for(int iClause: p_unsatClauses)
    {
        for(auto lit: p_clauses[iClause].getLiterals())
        {
            if(p_valuation[lit.first] == -1)
            {
                int polLit = (lit.second ? -lit.first : lit.first);
                unassignedLits.emplace(polLit);
            }
        }
    }

    if(unassignedLits.size() != 0)
    {
        // Select a random literal
        unsigned int index = rand() % (unassignedLits.size() - 1);
	    set<int>::iterator it = unassignedLits.begin();

        for(unsigned int i = 0; i < index; ++i)
            ++it;

        selectedUnassigned = abs(*it);
        if(!m_randomBet)
        {
            if(*it > 0)
                value = true;
            else
                value = false;
        }
        else
        {
            int bet = rand() % 9;
            
            // Randomize bet    
            if(bet > 4)
                value = false;
        }
    }

    decision bet = decision(selectedUnassigned, value, true);
    
    if(selectedUnassigned == -1)
    {
        OUTERROR("Critical issue in StdBet, a bet should exist " << *p_unsatClauses.begin() << " " << p_clauses[*p_unsatClauses.begin()].toStr());
    }

    OUTDEBUG("Taking bet: " << selectedUnassigned << " to " << value);
    return bet;
} // decision takeABet(vector<Clause>&, const set<int>&, map<int,int>&)

