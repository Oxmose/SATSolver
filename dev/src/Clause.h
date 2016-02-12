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
#include <map>      //std::map
#include <string>   //std::string

typedef struct literal literal;
struct literal
{
    literal(int index, bool bar, bool isAssigned = false) : index(index), bar(bar), isAssigned(isAssigned) {}

    int index; // index of the variable
    bool bar;  // x or bar x

    // Gives the state of the literal
    bool isAssigned;
}; // struct literal

static literal literal_from_int(int p_l)
{
    return literal(abs(p_l),(p_l < 0));
} // static literal literal_from_int(int);

class Clause
{
    public:
        // Contructor / Destructor
        Clause(const std::vector<literal> &p_literals, bool p_isTot);
        ~Clause();
        
        // Misc        
        bool evaluate(std::map<int,int>& p_valuation);

        // Setters
        void setSatisfier(int p_satisfier);
        void setAssigned(int p_index, bool p_assign=true);
        
        // Getters
        bool hasVar(int p_index) const;
        const literal& getLiteral(int p_index) const;
        bool isSatisfied() const;
        int getSatisfier() const;
        int getAliveVars() const;
        const std::vector<literal>& getLiterals() const;
        std::string toStr() const;

    private:
        // Literals of the clause
        std::vector<literal> m_literals;
        //-1 if not satisfied
        int m_satisfier;
        int m_aliveVars;
        bool m_isTot;
}; // Clause

#endif
