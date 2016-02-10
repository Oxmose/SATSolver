/*
 *
 *    CLASS SATSolver
 *
*/

#ifndef DEF_SATSOLVER_H
#define DEF_SATSOLVER_H

// STD INCLUDES
#include <string>   // sts::string
#include <vector>   // std::vector
#include <set>      // std::set
#include <map>
#include <sstream>  // std::stringstream
#include <iostream> // std::cout std::cerr std::endl
#include <fstream>  // std::ifstream

// PROJECT INCLUDES
#include "Clause.h"    // Clause class

struct decision
{
    decision(int index, bool value, bool bet) : index(index), value(value), bet(bet) {}

    int index;
    bool value;//useless
    bool bet; //bet or deduction
};

class SATSolver 
{
    public:
        SATSolver(const std::string &p_fileName);
        ~SATSolver();
        
        bool parse();

        /* DPLL algorithm */
        int solve(bool verbose = 1);
        
        std::string formulaToStr();
        std::string getFormulaStr();
        std::string decisionToStr();

    private:

        /* DPLL intern */
        decision takeABet();
        void applyDecision(decision p_dec);
        void dropSatisfiedBy(const decision& p_bet);
        void assignVarInClause(int p_index, bool p_assign=true);
        bool deduce();
        bool unitProp();
        bool isContradictory();
        void reviveClauseWithSatisfier(int p_satisfier);

        void showSolution();

        std::string     m_fileName;
        std::string     m_strFormula;
        unsigned int    m_maxIndex;
        unsigned int    m_clausesCount;

        std::vector<Clause> m_formula;

        /*DPLL stuff*/
        std::vector<decision> m_currentAssignement;
        std::map<int,int> m_valuation;
        std::map<int,std::vector<Clause>> m_clausesForVar;
        int m_satisfiedClause;

}; // SATSolver 

#endif // DEF_SATSOLVER_H
