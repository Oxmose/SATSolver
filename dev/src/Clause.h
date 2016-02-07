/*
 *
 *    CLASS Clause 
 *
*/

#ifndef DEF_CLAUSE_H
#define DEF_CLAUSE_H

// STD INCLUDES
#include <vector>    //std::vector
#include <utility>   //std::pair

class Clause
{
    public:
        Clause(const std::vector<int> &p_literals);
        ~Clause();

        bool isSolvable();
        bool solve();

        std::vector<std::pair<int, bool>> getValuation();

    private:
        std::vector<std::pair<int, int>> m_literals;
        std::vector<std::pair<int, bool>> m_valuation;
}; // Clause

#endif
