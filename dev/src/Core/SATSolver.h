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
#include "Clause.h"                     // Clause class
#include "../LogExpParser/LOGParser.h"    // LOGParser class

// GLOBAL FLAGS/VARS
#include "../Global/Global.h"

struct decision
{
    decision(int index, bool value, bool bet) : index(index), value(value), bet(bet) {}

    int index;
    bool value;
    bool bet; //bet or deduction
};

enum PARSE_TYPE
{
    CNF_PARSE,
    LOG_PARSE
};

enum BET_METHOD
{
    NORM,
    RAND,
    MOMS,
    DLIS
};

class SATSolver
{
    public:
        SATSolver(bool p_watchedLitMeth, BET_METHOD p_betMethod);
        ~SATSolver();

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
        void applyDecisionWL(const decision& p_dec);
        void satisfyClause(It p_it, int p_satisfier);

        bool deduce();
        bool unitProp();
        bool unitProp(int p_iClause);
        bool uniquePol();

        bool backtrack(bool& p_unsat);
        bool isContradictory();
        void reviveClauseWithSatisfier(int p_satisfier);

        bool isWatchedIn(int p_index, int p_iClause);      
 
        bool evaluate();

        unsigned int    m_maxIndex;
        bool m_watchedLitMeht;
        BET_METHOD m_betMethod;

        std::vector<ClauseSet> m_formula;//0: unsat clauses, 1: sat clauses

        /*DPLL stuff*/
        std::vector<decision> m_currentAssignement;//Stack of decisions
        std::map<int,int> m_valuation;//Current valuation
        std::map<int,std::vector<int>> m_clauseWithVar;//Direct access to Clauses
        std::map<int,std::set<int>> m_clauseWatchedBy;//Says for each lit the list of watched clause
        
}; // SATSolver

#endif // DEF_SATSOLVER_H
