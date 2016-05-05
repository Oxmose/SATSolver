#ifndef DEF_SATSOLVER_H
#define DEF_SATSOLVER_H

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

#include "../Global/Global.h"
#include "clause.h"
#include "conflict_graph.h"


#define OUTDEBUG(Out) do {  \
    if(settings_s.debug_s) { \
        Out; \
    } \
} while(0)


using namespace std;

class SATSolver
{
	friend class conflict_graph;

	public:
		SATSolver() : iter(0), curr_level(-1), conflict_graph(this) {};

		bool add_clause(clause c, bool input=false);

		void            take_a_bet();
		bool add_pending_deduction();

		int apply_last_decision();
		int apply_last_decisionSTD();
		int apply_last_decisionWL();

		pair<clause,int> diagnose_conflict(int conflict_clause);
		pair<int,bool> backtrack(int bt_to, bool full_bt);

		vector<clause>                                        formula;
		map<unsigned int, int>                              valuation;

		void reset_valuation();
		bool solve();

		void print_current_state();

	private:
		/* DPLL interns */
		set<int>                                        unsat_clauses;
		queue<pair<int,int>>                          deduction_queue;//First: deduction, Second: clause coming from
		vector<pair<int,bool>>                         decision_stack;//First: decision, Second: is a bet ?
		unordered_map<int,vector<int>>   	           clauses_sat_by;

		unordered_map<unsigned int,set<int>>       clauses_watched_by;//WL
		unordered_map<unsigned int,set<int>>         clauses_with_var;//STD
		
		int                                                curr_level;
		int                                                      iter;

		struct conflict_graph conflict_graph;
};

#endif