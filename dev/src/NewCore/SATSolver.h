#ifndef DEF_SATSOLVER_H
#define DEF_SATSOLVER_H

// STD INCLUDES
#include <iostream>
#include <cassert>
#include <fstream>
#include <sstream>
#include <cstdio>
#include <string>
#include <vector>
#include <queue>
#include <set>
#include <map>
#include <unordered_map>

 // PROJECT INCLUDES
#include "../Global/Global.h"
#include "../BETHeuristic/IBet.h"
#include "clause.h"
#include "conflict_graph.h"

#include "SMT/SMTSolver.h"
#include "SMT/SMTSolver_eq.h"

// MACRO DEFINE
#define OUTDEBUG(Out) do {  \
    if(settings_s.debug_s) { \
        Out; \
    } \
} while(0)


using namespace std;

class IBet;

struct decision
{
    decision(int dec=0, int level=-2, bool bet=false) : dec(dec), level(level), bet(bet) {}
    int dec;
    int level;
    bool bet;
};

class SATSolver
{
    friend class conflict_graph;
    friend class IBet;
    friend class DLISBet;
    friend class RandomBet;
    friend class StandardBet;
    friend class MOMSBet;
    friend class VSIDSBet;

    friend class SMTSolver;
    friend class SMTSolver_eq;

    public:
        SATSolver();
        ~SATSolver();

        bool add_clause(clause c, bool input=false);

        void            take_a_bet();
        bool add_pending_deduction();

        int apply_last_decision();
        int apply_last_decisionSTD();
        int apply_last_decisionWL();

        pair<clause,int> diagnose_conflict(int conflict_clause);
        decision backtrack(int bt_to, bool full_bt);

        vector<clause>                                        formula;
        map<unsigned int, int>                              valuation;

        void reset_valuation();
        bool solve();

        void print_current_state();

        void getStrFormula();
        void setBet(IBet *p_bet);

        double getVarScores(int p_var);
        double VSIDSScoreFunction(double oldScore, bool inLearnedClause);

        void emplace_eq(unsigned int var, struct smt_literal *eq);

    private:

        
        /* DPLL interns */
        set<int>                                        unsat_clauses;
        queue<pair<int,int>>                          deduction_queue;//First: deduction, Second: clause coming from
        vector<decision>                               decision_stack;//First: decision, Second: is a bet ?
        unordered_map<int,vector<int>>                  clauses_sat_by;

        unordered_map<unsigned int,set<int>>       clauses_watched_by;//WL
        unordered_map<unsigned int,set<int>>         clauses_with_var;//STD
        
        int                                                curr_level;
        int                                                      iter;

        struct conflict_graph conflict_graph;

        IBet                                                   *m_bet;

        unordered_map<int, double>                        m_varScores;//vsids

        /* SMT */
        SMTSolver* smt_solver;
        map<unsigned int, smt_literal*>                   dpll_to_smt;
};

#endif // DEF_SATSOLVER_H
