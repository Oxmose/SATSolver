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

struct literal
{
	literal(int index, bool bar) : index(index), bar(bar) {}

	int index; // index of the variable
	bool bar; // x or bar x
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

    private:
        std::vector<literal> m_literals;
}; // Clause

#endif
