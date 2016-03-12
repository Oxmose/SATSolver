/*
 *
 *  CLASS SATSolver
 *
*/

#ifndef DEF_SATSOLVER_H
#define DEF_SATSOLVER_H

// STD INCLUDES
#include <string>   // sts::string
#include <vector>   // std::vector
#include <set>      // std::set
#include <map>      // std::map
#include <sstream>  // std::stringstream
#include <iostream> // std::cout std::cerr std::endl
#include <fstream>  // std::ifstream

// PROJECT INCLUDES
#include "Clause.h"                         // Clause class
#include "../CNFParser/CNFParser.h"         // CNFParser class
#include "../LogExpParser/LOGParser.h"      // LOGParser class
#include "../BETHeuristic/IBet.h"           // Bet Heuristic Interface

// GLOBAL FLAGS/VARS
#include "../Global/Global.h" // DEBUG

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
        SATSolver(bool p_watchedLitMeth);
        ~SATSolver();

        // Bet strategy
        void setStrategy(IBet *p_betMethod);

        void setMaxIndex(int p_maxIndex);
        void setOriginFormula(const ClauseSet &initClauseSet);
        void reset();
        

        /* DPLL algorithm */
        bool solve();
        void showSolution(LOGParser &parser);
        void showSolution();

        /* Debug stuff */
        std::string currentStateToStr();//State = decision + formula
        std::string formulaToStr();
        std::string decisionToStr();

    private:

        /* DPLL intern */
        void flushTaut();

        decision takeABet();
        void applyDecision(const decision& p_dec);
        void satisfyClause(It p_it, int p_satisfier);

        bool deduce();
        bool unitProp();
        bool uniquePol();

        bool backtrack(bool& p_unsat);
        bool isContradictory();
        void reviveClauseWithSatisfier(int p_satisfier);


        bool evaluate();

        unsigned int    m_maxIndex;
        bool m_watchedLitMeht;

	    IBet *m_betHeuristic;

        std::vector<ClauseSet> m_formula;//0: unsat clauses, 1: sat clauses

        /*DPLL stuff*/
        std::vector<decision> m_currentAssignement;//Stack of decisions
        std::map<int,int> m_valuation;//Current valuation
        std::map<int,std::vector<int>> m_clauseWithVar;//Direct access to Clauses
}; // SATSolver

#endif // DEF_SATSOLVER_H
