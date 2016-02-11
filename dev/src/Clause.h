/*
 *
 *    CLASS Clause
 *
*/

#ifndef DEF_CLAUSE_H
#define DEF_CLAUSE_H


// STD INCLUDES
#include <cstdlib>
#include <vector>    //std::vector
#include <utility>   //std::pair
#include <map>
#include <string>

struct literal
{
	literal(int index, bool bar, bool isAssigned=false) : index(index), bar(bar), isAssigned(isAssigned) {}

	int index; // index of the variable
	bool bar; // x or bar x

	bool isAssigned;
};

static literal literal_from_int(int p_l)
{
	return literal(abs(p_l),(p_l < 0));
}

class Clause
{
    public:
        Clause(const std::vector<literal> &p_literals);
        ~Clause();

        const std::vector<literal>& getLiterals() const;

        bool isSatisfied() const;
        int getSatisfier() const;
        void setSatisfier(int p_satisfier);

        bool hasVar(int p_index) const;
        const literal& getLiteral(int p_index) const;
        int getAliveVars();

        void setAssigned(int p_index, bool p_assign=true);

        bool evaluate(std::map<int,int>& p_valuation);
        std::string toStr() const;

    private:
        std::vector<literal> m_literals;
        int m_satisfier;//-1 if not satisfied
        int m_aliveVars;
}; // Clause

#endif
