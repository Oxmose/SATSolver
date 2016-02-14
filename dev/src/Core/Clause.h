/*
 *
 *    CLASS Clause
 *
*/

#ifndef DEF_CLAUSE_H
#define DEF_CLAUSE_H


// STD INCLUDES
#include <cstdlib>
#include <iostream>
#include <vector>    //std::vector
#include <utility>   //std::pair
#include <set>
#include <map>
#include <string>

typedef struct literal literal;
struct literal
{
	literal(int index, bool bar) : index(index), bar(bar) {}

    int index; // index of the variable
    bool bar;  // x or bar x

    bool operator < (const literal& b) const
    {
        return index < b.index;//For a log(n) access in set structure
    }
};

static literal literal_from_int(int p_l)//parsing utility
{
    return literal(abs(p_l),(p_l < 0));
} // static literal literal_from_int(int);

class Clause
{
    public:
        Clause(const std::vector<literal> &p_literals, bool p_isTaut, int p_id);
        ~Clause();

        /*
                Functions are marked const in order to use them inside set structure.
            It requires some attributes to be mutable.
        */

        bool isTaut() const;
        std::set<literal>& getLiterals(int p_which=0) const;

        /* Access/set the satisfier of a clause (see readme)*/
        int getSatisfier() const;
        void setSatisfier(int p_satisfier);

        /* Move literal through m_literals[0]/m_literals[1] */
        void setAssigned(int p_index, bool p_assign = true) const;

        int getId() const;

        /* Testing purposes */
        bool evaluate(std::map<int,int>& p_valuation);
        std::string toStr() const;

    private:

        int m_id;//unique id for the clause

        mutable std::set<literal> m_literals[2];//0 unassigned 1 assigned
        int m_satisfier;//-1 if not satisfied
        bool m_isTaut;//is a tautologie ?
}; // Clause

/* Tool functions for performing searchs in sets */

static Clause makeSearchClause(int p_indice)
{
    std::vector<literal> lit;
    return Clause(lit,false,p_indice);
}

static Clause makeSearchClause2(int p_satisfier)
{
    std::vector<literal> lit;
    Clause c = Clause(lit,false,-1);
    c.setSatisfier(p_satisfier);
    return c;
}

typedef std::multiset<Clause,bool (*) (const Clause&,const Clause&)> ClauseSet;
typedef ClauseSet::iterator It;

static bool compareUnsat(const Clause& p_a, const Clause& p_b)
{
    return p_a.getId() < p_b.getId();//Sort unsat Clauses by id for log(n) find
}

static bool compareSat(const Clause& p_a, const Clause& p_b)
{
    //Using multiset, Clause with same satisfier are equivalent
    return p_a.getSatisfier() < p_b.getSatisfier();
}

#endif
