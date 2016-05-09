#include "SMTSolver_eq.h"
#include "../SATSolver.h"

bool SMTSolver_eq::apply_last_decision()
{
	decision last_decision = solver->decision_stack.back();
	assert(solver->dpll_to_smt.find(abs(last_decision.dec)) != solver->dpll_to_smt.end());
	smt_literal_eq* corresponding_lit = (smt_literal_eq*)solver->dpll_to_smt[abs(last_decision.dec)];


	/*
		Cases:

		3 = 4 --> add edge ponderated with decision level
		3 != 4 --> stock inequality, check for conflict

		-(3 = 4) --> stock inequality, check for conflict
		-(3 != 4) -->add edge ponderated with decision level
	*/

	bool add_edge = ((last_decision.dec > 0) && corresponding_lit->equal) || ((last_decision.dec < 0) && !corresponding_lit->equal);
	if(add_edge)
	{
		int s1 = min(corresponding_lit->left, corresponding_lit->right);
		int s2 = max(corresponding_lit->left, corresponding_lit->right);

		/*if(not_possible[s1].find(s2) != not_possible[s1].end())
		{

		}*/

		/* Already exists (3 = 4), (4 = 3)*/
		if(edge.find(s1) != edge.end() && edge[s1].find(s2) != edge[s1].end())
		{
			assert(edge.find(s2) != edge.end() && edge[s2].find(s1) != edge[s2].end() && edge[s1][s2].second == edge[s2][s1].second);//costly 
			edge[s1][s2] = make_pair(last_decision.dec, last_decision.level);
			edge[s2][s1] = make_pair(last_decision.dec, last_decision.level);
		}

		if(connectivity_check.find(s1) == nullptr)
		{
			assert(connectivity_check.find(s2) == nullptr);
			connectivity_check.add(s1);
			connectivity_check.add(s2);
		}

		connectivity_check.make_union(s1,s2);
		OUTDEBUG(fprintf(stderr, "[SMT]Adding edge %d-%d, pond: %d, etiq: %d\n", corresponding_lit->left, corresponding_lit->right, edge[s1][s2].second, edge[s1][s2].first));
	}

	return false;
}


pair<clause,int> SMTSolver_eq::diagnose_conflict()
{

}