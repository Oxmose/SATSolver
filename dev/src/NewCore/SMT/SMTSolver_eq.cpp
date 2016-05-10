#include "SMTSolver_eq.h"
#include "../SATSolver.h"

/* literal violated */
int SMTSolver_eq::apply_last_decision()
{
	int last_dec_index = solver->decision_stack.size()-1;
	decision last_decision = solver->decision_stack.back();

	if(solver->dpll_to_smt.find(abs(last_decision.dec)) == solver->dpll_to_smt.end())
		return 0;

	smt_literal_eq* corresponding_lit = (smt_literal_eq*)solver->dpll_to_smt[abs(last_decision.dec)];
	OUTDEBUG(fprintf(stderr, "\t[SMT]Handling SMT literal %s\n", corresponding_lit->to_str().c_str()));
	/*
		Cases:

		3 = 4 --> add edge ponderated with decision level
		3 != 4 --> stock inequality, check for conflict

		-(3 = 4) --> stock inequality, check for conflict
		-(3 != 4) -->add edge ponderated with decision level
	*/

	bool add_edge = ((last_decision.dec > 0) && corresponding_lit->equal) || ((last_decision.dec < 0) && !corresponding_lit->equal);
	

	int s1 = min(corresponding_lit->left, corresponding_lit->right);
	int s2 = max(corresponding_lit->left, corresponding_lit->right);

	if(connectivity_check.find(s1) == nullptr)
		connectivity_check.add(s1);
	if(connectivity_check.find(s2) == nullptr)
		connectivity_check.add(s2);

	if(add_edge)
	{
		/* Si il existe deja un chemin on s'en fiche*/
		if(connectivity_check.find(s1) != connectivity_check.find(s2))
		{
			edge[s1][s2] = last_dec_index;
			edge[s2][s1] = last_dec_index;
			OUTDEBUG(fprintf(stderr, "\t[SMT]Adding edge %d-%d, index in stack: %d, pond: %d, lit: %d.\n", corresponding_lit->left, corresponding_lit->right, edge[s1][s2], 
																				solver->decision_stack[edge[s1][s2]].level, solver->decision_stack[edge[s1][s2]].dec));


			connectivity_check.make_union(s1,s2);

			for(auto s1p : not_possible)
				for(auto s2p : s1p.second)
				{
					if(connectivity_check.find(s1p.first) == connectivity_check.find(s2p.first))
					{
						OUTDEBUG(fprintf(stderr, "\t[SMT]Clash, decision %d (%s) violated!\n", solver->decision_stack[s2p.second].dec, solver->dpll_to_smt[abs(solver->decision_stack[s2p.second].dec)]->to_str().c_str()));
						return s2p.second;
					}
				}
		}
		return 0;
	}

	not_possible[s1][s2] = last_dec_index;
	OUTDEBUG(fprintf(stderr, "\t[SMT]Remembering that %d and %d shouldn't be connected.\n", s1, s2));
	if(connectivity_check.find(s1) == connectivity_check.find(s2))
	{
		OUTDEBUG(fprintf(stderr, "\t[SMT]Clash, decision %d (%s) violated!\n", solver->decision_stack[last_dec_index].dec, solver->dpll_to_smt[abs(solver->decision_stack[last_dec_index].dec)]->to_str().c_str()));
		return last_dec_index;
	}
	return 0;
}

bool SMTSolver_eq::dfs_enumerate_paths(int curr, int dest, map<int,int>& succ)
{
	if(succ.find(curr) != succ.end())
		return false;

	if(curr == dest)
		return true;

	for(auto v : edge[curr])
	{
		succ[curr] = v.first;
		if(dfs_enumerate_paths(v.first,dest,succ))
			return true;
	}

	return false;
}


pair<clause,int> SMTSolver_eq::diagnose_conflict(int conflict_dec_index)
{
	decision conflict_dec = solver->decision_stack[conflict_dec_index];
	smt_literal_eq* corresponding_lit = (smt_literal_eq*)solver->dpll_to_smt[abs(conflict_dec.dec)];
	OUTDEBUG(fprintf(stderr, "\t[SMT]Diagnosing conflict because of %d (%s)\n", conflict_dec.dec, corresponding_lit->to_str().c_str()));
	
	int s1 = min(corresponding_lit->left, corresponding_lit->right);
	int s2 = max(corresponding_lit->left, corresponding_lit->right);

	OUTDEBUG(fprintf(stderr, "\t[SMT]Path from %d to %d: \n", s1, s2));
	map<int,int> succ;
	dfs_enumerate_paths(s1, s2, succ);
	int curr = s1;

	clause c(solver->formula.size());
	int bt_level = -1;
	while(curr != s2)
	{
		assert(edge[curr][succ[curr]] != conflict_dec_index);
		if(solver->decision_stack[edge[curr][succ[curr]]].level != solver->curr_level)
			bt_level = max(bt_level, solver->decision_stack[edge[curr][succ[curr]]].level);
		c.literal.push_back(-solver->decision_stack[edge[curr][succ[curr]]].dec);
		OUTDEBUG(fprintf(stderr, "\t\t%d %d, dec: %d lvl: %d\n", curr, succ[curr], solver->decision_stack[edge[curr][succ[curr]]].dec, solver->decision_stack[edge[curr][succ[curr]]].level));
		curr = succ[curr];
	}
	OUTDEBUG(fprintf(stderr, "\n"));

	c.literal.push_back(-solver->decision_stack[conflict_dec_index].dec);
	if(solver->decision_stack[conflict_dec_index].level != solver->curr_level)
		bt_level = max(bt_level, solver->decision_stack[conflict_dec_index].level);
	if(bt_level == -1)
		bt_level = solver->curr_level;

	OUTDEBUG(fprintf(stderr, "\t[SMT]Learning %s\n", c.to_str().c_str()));
	OUTDEBUG(fprintf(stderr, "\t[SMT]Backtracking to %d\n", bt_level));


	
	return make_pair(c,bt_level);
}