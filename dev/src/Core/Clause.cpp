/*
 *
 *    CLASS Clause
 *
*/

// STD INCLUDES
#include <cstdlib>  //std::abs
#include <iostream> //std::cout std::cerr
#include <vector>   //std::vector
#include <utility>  //std::pair
#include <set>      //std::set
#include <map>      //std::map
#include <string>   //std::string

// Class Header
#include "Clause.h"

using namespace std;

Clause::Clause(const map<int,bool> &p_literals, bool p_isTaut, int p_id)
{
    m_literals = p_literals;
    m_isTaut = p_isTaut;
    m_id = p_id;
    m_score = 0;
    
} // Clause(const vector<int>&)

bool Clause::isTaut() const
{
    return m_isTaut;
} // bool isTaut() const

map<int,bool>& Clause::getLiterals()
{
    return m_literals;
} // set<literal>& getLiterals(int) const

int Clause::getId() const
{
    return m_id;
} // int getId() const

bool Clause::evaluate(map<int,int>& p_valuation)
{
    if(m_isTaut)
        return true;
        
    bool val = false;
    for(auto l : m_literals)
        if(p_valuation[l.first] != -1)
            val = val || p_valuation[l.first] == !l.second;
    
    return val;
} // bool evaluate(map<int, int>&)

string Clause::toStr() const
{
    string toReturn = "(";
    for(auto l : m_literals)
    {
        string sign = (l.second) ? "-" : "";
        string dis =  "\\/";
        toReturn += sign + to_string(l.first) + dis;
    }
    toReturn += ")";
    return toReturn;
} // string toStr() const

string Clause::toDIMACS() const
{
    string toReturn = "";
    for(auto l : m_literals)
    {
        string sign = (l.second) ? "-" : "";
        string dis =  " ";
        toReturn += sign + to_string(l.first) + dis;
    }
    toReturn += "0";
    return toReturn;
} // string toStr() const

int Clause::getScore() const
{
    return m_score;
}

Clause::~Clause()
{
} // ~Clause()
