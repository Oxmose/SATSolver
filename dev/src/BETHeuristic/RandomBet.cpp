/*
 *
 *  CLASS RandomBet
 *
*/

// STD INCLUDES
#include <vector>   // std::vector
#include <ctime>     // std::time
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

decision RandomBet::takeABet(const vector<ClauseSet> &p_formula, vector<decision> &p_currentAssignement)
{
    OUTDEBUG("Random bet");
    vector<int> unassignedLits;
    int selectedUnassigned = -1;

    // Gather non assigned literals
    for(auto it = p_formula[0].begin() ; it != p_formula[0].end() ; ++it)
    {
        if(!it->getLiterals(0).empty())
        {
            for(auto it2 = it->getLiterals(0).begin(); it2 != it->getLiterals(0).end(); ++it2)
            {
                int value = it2->index;
                if(it2->bar)
                    unassignedLits.push_back(-value);
                else
                    unassignedLits.push_back(value);
            }
        }
    }

    bool value = true;
    if(unassignedLits.size() != 0)
    {
        // Select a random literal
        unsigned int index = rand() % (unassignedLits.size() - 1);    
        selectedUnassigned = abs(unassignedLits[index]);
        if(!m_randomBet)
        {
            int samePol = 0;
            int invePol = 0;
            // Seach if the selected variable appear more with - or +
            for(unsigned int i = 0; i < unassignedLits.size(); ++i)
            {
                if(unassignedLits[i] == unassignedLits[index])
                    ++samePol;
                else
                    ++invePol;
            }   
            bool same = samePol > invePol;
            if(samePol)
            {
                if(0 > unassignedLits[index])
                    value = false;
                else
                    value = true;
            }
            else
            {
                if(0 > unassignedLits[index])
                    value = true;
                else
                    value = false;
            }
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

    if(p_formula[0].empty())
        return bet;

    OUTDEBUG("Taking bet: " << selectedUnassigned << " to " << (value ? "True" : "False"));
    p_currentAssignement.push_back(bet);
    return bet;
} // decision takeABet(const std::vector<ClauseSet>&, std::vector<decision>&)

