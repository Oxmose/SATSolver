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
#include <queue>
#include <set>      // std::set
#include <map>      // std::map
#include <sstream>  // std::stringstream
#include <iostream> // std::cout std::cerr std::endl
#include <fstream>  // std::ifstream
#include <memory>

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
    bool value;
    bool bet; //bet or deduction
};

enum PARSE_TYPE
{
    CNF_PARSE,
    LOG_PARSE
};

/*
    Looks horrible but in fact simple, it represents a multiset of couples (iClause, iSatisfier)
    It allows us to efficiently get all clauses with given satisfier.
*/
typedef std::multiset<std::pair<int,int>, bool (*) (const std::pair<int,int>&,const std::pair<int,int>&)> satClausesSet;
static bool compareSat(const std::pair<int,int>& p_a, const std::pair<int,int>& p_b)
{
    return p_a.second < p_b.second;//Sort unsat Clauses by id for log(n) find
} // static bool compareUnsat(const Clause&, const Clause&)


class SATSolver
{
    public:
        SATSolver();
        virtual ~SATSolver();

        // Bet strategy
        void setStrategy(IBet* p_betMethod);

        void setMaxIndex(int p_maxIndex);
        void setOriginFormula(const std::vector<Clause> &p_clauses);
        void reset();

        /* DPLL algorithm */
        virtual void initializeMethod() = 0;
        bool solve();
        void showSolution(LOGParser &parser);
        void showSolution();

        /* Debug stuff */
        std::string currentStateToStr();//State = decision + formula
        std::string formulaToStr();
        std::string decisionToStr();

    protected:
        
        void flushTaut();
        void preprocess();

        virtual bool applyLastDecision() = 0;
        void satisfyClause(int p_iClause, int p_satisfier);

        bool deduce();
        virtual bool uniquePol(bool p_preprocess = false) = 0;

        bool isContradictory();
        void reviveClauseWithSatisfier(int p_satisfier);
        virtual bool backtrack(bool& p_unsat) = 0;

        decision takeABet();

        bool evaluate();
        

        std::unique_ptr<IBet> m_betHeuristic;

        /*Formula representation*/
        std::vector<Clause> m_clauses;
        std::set<int>  m_unsatClauses;
        std::unique_ptr<satClausesSet> m_satClauses;

        /*DPLL stuff*/
        std::vector<decision> m_currentAssignement;//Stack of decisions
        std::map<int,int> m_valuation;//Current valuation
        std::queue<decision> m_deductionQueue;
        std::queue<decision> m_preprocessQueue;

        bool m_isContradictory;


    private:
        /* DPLL intern */

        unsigned int    m_maxIndex;
        
}; // SATSolver

#endif // DEF_SATSOLVER_H
