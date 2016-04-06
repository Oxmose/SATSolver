/*
 *
 *    CLASS Clause
 *
*/

#ifndef DEF_CLAUSE_H
#define DEF_CLAUSE_H

// STD INCLUDES
#include <cstdlib>  //std::abs
#include <iostream> //std::cout std::cerr
#include <vector>   //std::vector
#include <utility>  //std::pair
#include <set>      //std::set
#include <map>      //std::map
#include <string>   //std::string


class Clause
{
    public:
        Clause() {}
        Clause(const std::map<int,bool> &p_literals, bool p_isTaut, int p_id);
        ~Clause();

        bool isTaut() const;
        std::map<int,bool>& getLiterals();

        int getId() const;

        /* Testing purposes */
        bool evaluate(std::map<int,int>& p_valuation);
        std::string toStr() const;

    private:

        int m_id;//unique id for the clause
        bool m_isTaut;//is a tautologie ?
        std::map<int,bool> m_literals;//key = var index, value = isBar?
}; // Clause



#endif
