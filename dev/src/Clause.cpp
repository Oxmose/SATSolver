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

Clause::Clause(const vector<literal> &p_literals, bool p_isTot)
{
    m_isSatisfied = false;
    m_isTot = p_isTot;
    // Add literals
    for(literal l : p_literals)
    {
        m_literals.push_back(l);
    }

    m_aliveVars = p_literals.size();
} // Clause(const vector<int>&)

const std::vector<literal>& Clause::getLiterals() const
{
    return m_literals;
}

bool Clause::isSatisfied() const
{
    return m_isSatisfied;
}

void Clause::setSatisfied(bool p_isSatisfied)
{
    m_isSatisfied = p_isSatisfied;
}

bool Clause::hasVar(int p_index) const
{
    for(auto l : m_literals)
        if(l.index == p_index)
            return true;
    return false;
}

const literal& Clause::getLiteral(int p_index) const
{
    for(const literal& l : m_literals)
        if(l.index == p_index)
            return l;
    //todo
}

int Clause::getAliveVars()
{
    return m_aliveVars;
}

void Clause::setAssigned(int p_index, bool p_assign /*=true*/)
{
    for(literal& l : m_literals)
        if(l.index == p_index)
            l.isAssigned = p_assign;
    m_aliveVars += (p_assign) ? -1 : 1;
}

string Clause::toStr() const
{
    string toReturn = "(";
    for(int iLit = 0 ; iLit < m_literals.size() ; iLit++)
    {
        auto l = m_literals[iLit];
        if(!l.isAssigned)
        {
            string sign = (l.bar) ? "-" : "";
            string dis = (iLit == m_literals.size()-1) ? "" : "\\/";
            toReturn += sign + to_string(l.index) + dis;
        }
    }
    toReturn += ")";
    return toReturn;
}

Clause::~Clause()
{
} // ~Clause()
