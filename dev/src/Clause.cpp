/*
 *
 *    CLASS Clause 
 *
*/

// STD INCLUDES
#include <vector>   //std::vector
#include <utility>  //std::pair

// Class Header
#include "Clause.h"

using namespace std;

Clause::Clause(const vector<int> &p_literals)
{
    // Add literals
    for(int i : p_literals)
    {
        m_literals.push_back(make_pair(i, -1));
    }
} // Clause(const vector<int>&)

Clause::~Clause()
{
} // ~Clause()

int Clause::isSolvable()
{
    // Simple test for now
    for(pair<int, int> lit : m_literals)
    {
        for(pair<int, int> litsec : m_literals)
        {
            if(litsec.first == -lit.first)
                return 0;
        }
    }

    return 1;
} // int isSolvable()

bool Clause::solve()
{
} // solve()

vector<pair<int, bool>> Clause::getValuation()
{
} // vector<pair<int, bool>> getValuation()
