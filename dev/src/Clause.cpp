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

Clause::Clause(const vector<literal> &p_literals)
{
    // Add literals
    for(literal l : p_literals)
    {
        m_literals.push_back(l);
    }
} // Clause(const vector<int>&)

Clause::~Clause()
{
} // ~Clause()
