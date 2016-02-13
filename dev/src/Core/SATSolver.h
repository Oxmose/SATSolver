/*
 *
 *  CLASS SATSolver
 *
*/

#ifndef DEF_SATSOLVER_H
#define DEF_SATSOLVER_H

/*
    Uncomment the following define for debug output
*/
#define NDEBUG
#ifndef NDEBUG
#define OUTDEBUG(Out) do {  \
   std::cerr << Out << std::endl; \
} while(0)
#else
#define OUTDEBUG(Out) do {}while(0)
#endif

// STD INCLUDES
#include <string>   // sts::string
#include <vector>   // std::vector
#include <set>      // std::set
#include <map>      // std::map
#include <sstream>  // std::stringstream
#include <iostream> // std::cout std::cerr std::endl
#include <fstream>  // std::ifstream

// PROJECT INCLUDES
#include "Clause.h"                 	// Clause class
#include "../CNFParser/CNFParser.h"    	// CNFParser class
#include "../LogExpParser/LOGParser.h"	// LOGParser class

struct decision
{
    decision(int index, bool value, bool bet) : index(index), value(value), bet(bet) {}

    int index;
    bool value;//useless
    bool bet; //bet or deduction
};

enum PARSE_TYPE
{
    CNF_PARSE,
    LOG_PARSE
};

class SATSolver
{
    public:
        SATSolver(const std::string &p_fileName);
        ~SATSolver();

        bool parse(PARSE_TYPE p_parseType = CNF_PARSE);

        /* DPLL algorithm */
        int solve();
        void showSolution();

        /* Debug stuff */
        std::string currentStateToStr();//State = decision + formula
        std::string formulaToStr();
        std::string decisionToStr();

    private:

	// LOGParser
	PARSE_TYPE m_parseType;
	LOGParser m_parser;

        /* DPLL intern */
        void flushTaut();
        decision takeABet();
        void satisfyClause(It p_it, int p_satisfier);
        void applyDecision(const decision& p_dec);
        bool deduce();
        bool unitProp();
        bool backtrack(bool& p_unsat);
        bool isContradictory();
        void reviveClauseWithSatisfier(int p_satisfier);


        bool evaluate();

        std::string     m_fileName;
        unsigned int    m_maxIndex;

        std::vector<ClauseSet> m_formula;//0: unsat clauses, 1: sat clauses

        /*DPLL stuff*/
        std::vector<decision> m_currentAssignement;//Stack of decisions
        std::map<int,int> m_valuation;//Current valuation
        std::map<int,std::vector<int>> m_clauseWithVar;//Direct access to Clauses
}; // SATSolver

#endif // DEF_SATSOLVER_H
