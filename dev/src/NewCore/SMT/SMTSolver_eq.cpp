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
		/*
			Si deja existante (4=3 puis 3=4) ne rien faire car de toute maniere osef
		*/
		if(edge.find(s1) == edge.end() || edge[s1].find(s2) == edge[s1].end())
		{
			assert(edge.find(s2) == edge.end() || edge[s2].find(s1) == edge[s2].end());//costly 
			edge[s1][s2] = last_dec_index;
			edge[s2][s1] = last_dec_index;
			OUTDEBUG(fprintf(stderr, "\t[SMT]Adding edge %d-%d, index in stack: %d, pond: %d, lit: %d.\n", corresponding_lit->left, corresponding_lit->right, edge[s1][s2], 
																				solver->decision_stack[edge[s1][s2]].level, solver->decision_stack[edge[s1][s2]].dec));
		}

		//exit(0);

		connectivity_check.make_union(s1,s2);

		for(auto s1p : not_possible)
			for(auto s2p : s1p.second)
			{
				if(connectivity_check.find(s1p.first) == connectivity_check.find(s2p.first))
				{
					OUTDEBUG(fprintf(stderr, "\t[SMT]Clash, literal %s violated!\n", solver->dpll_to_smt[abs(solver->decision_stack[s2p.second].dec)]->to_str().c_str()));
					return s2p.second;
				}
			}

		return 0;
	}

	not_possible[s1][s2] = last_dec_index;
	OUTDEBUG(fprintf(stderr, "\t[SMT]Remembering %d and %d shouldn't be connected.\n", s1, s2));
	if(connectivity_check.find(s1) == connectivity_check.find(s2))
	{
		OUTDEBUG(fprintf(stderr, "\t[SMT]Clash, literal %s violated!\n", solver->dpll_to_smt[abs(solver->decision_stack[last_dec_index].dec)]->to_str().c_str()));
		return last_dec_index;
	}

}


pair<clause,int> SMTSolver_eq::diagnose_conflict()
{

}