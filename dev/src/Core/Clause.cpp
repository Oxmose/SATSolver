/*
 *
 *    CLASS Clause
 *
*/

// STD INCLUDES
#include <cstdlib>  //std::abs
#include <vector>   //std::vector
#include <utility>  //std::pair
#include <map>      //std::map
#include <string>   //std::string

// Class Header
#include "Clause.h"

using namespace std;

Clause::Clause(const vector<literal> &p_literals, bool p_isTot)
{
    m_isTot = p_isTot;
    m_satisfier = -1;

    // Add literals
    for(literal l : p_literals)
    {
        m_literals.push_back(l);
    }

    m_aliveVars = p_literals.size();
} // Clause(const vector<int>&)

bool Clause::evaluate(map<int,int>& p_valuation)
{
    bool val = false;
    for(literal l : m_literals)
    {
        if(p_valuation[l.index] == -1)
            continue;
        val = val || p_valuation[l.index] == !l.bar;
    }
    return val;
} // bool evaluate(map<int,int>&)

const vector<literal>& Clause::getLiterals() const
{
    return m_literals;
} // const vector<literal>& getliterals() const

bool Clause::isSatisfied() const
{
    return m_satisfier != -1;
} // bool isSatified() const

int Clause::getSatisfier() const
{
    return m_satisfier;
} // int getSatifier() const

void Clause::setSatisfier(int p_satisfier)
{
    m_satisfier = p_satisfier;
} // setSatisfier(int)

bool Clause::hasVar(int p_index) const
{
    for(auto l : m_literals)
        if(l.index == p_index)
            return true;
    return false;
} // bool hasVar(int) const

const literal& Clause::getLiteral(int p_index) const
{
    for(const literal& l : m_literals)
        if(l.index == p_index)
            return l;
    //todo
} // const literal& getLiteral(int) const

int Clause::getAliveVars() const
{
    return m_aliveVars;
} // int getAliveVars()

void Clause::setAssigned(int p_index, bool p_assign /*=true*/)
{
    for(literal& l : m_literals)
        if(l.index == p_index)
            l.isAssigned = p_assign;
    m_aliveVars += (p_assign) ? -1 : 1;
} // setAssigned(int, bool)

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
} // string toStr() const

Clause::~Clause()
{
} // ~Clause()
