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

Clause::Clause(const vector<literal> &p_literals, bool p_isTaut, int p_id)
{
    m_isTaut = p_isTaut;
    m_id = p_id;
    m_satisfier = -1;
    // Add literals as unassigned
    for(literal l : p_literals)
        m_literals[0].insert(l);
} // Clause(const vector<int>&)

bool Clause::isTaut() const
{
    return m_isTaut;
} // bool isTaut() const

set<literal>& Clause::getLiterals(int p_which /* = 0 */) const
{
    return m_literals[p_which];
} // set<literal>& getLiterals(int) const

int Clause::getSatisfier() const
{
    return m_satisfier;
} // int getSatifier() const

void Clause::setSatisfier(int p_satisfier)
{
    m_satisfier = p_satisfier;
} // setSatisfier(int)

void Clause::setAssigned(int p_index, bool p_assign /* = true */) const
{
    auto l_it = m_literals[int(!p_assign)].find(literal(p_index,false));
    if(l_it != m_literals[int(!p_assign)].end())
    {
        literal toCopy = *l_it;
        m_literals[int(!p_assign)].erase(l_it);
        m_literals[int(p_assign)].insert(toCopy);
    }
} // Clause setAssigned(int, bool) const

int Clause::getId() const
{
    return m_id;
} // int getId() const

bool Clause::evaluate(map<int,int>& p_valuation)
{
    if(m_isTaut)
        return true;
        
    bool val = false;
    for(literal l : m_literals[1])
    {
        if(p_valuation[l.index] == -1)
        {
            //Should never happen (assigned vars are assigned), but who knows ?
            cerr << "Something went really wrong in Clause::evaluate..." << endl;
            exit(0);
            continue;
        }
        val = val || p_valuation[l.index] == !l.bar;
    }
    return val;
} // bool evaluate(map<int, int>&)

string Clause::toStr() const
{
    string toReturn = "(";
    for(auto l : m_literals[0])
    {
        string sign = (l.bar) ? "-" : "";
        string dis =  "\\/";
        toReturn += sign + to_string(l.index) + dis;
    }
    toReturn += ")";
    return toReturn;
} // string toStr() const

Clause::~Clause()
{
} // ~Clause()
