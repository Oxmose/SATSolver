#include "SMTSolver_eq.h"
#include "../SATSolver.h"

void SMTSolver_eq::reset_method()
{
	assert(settings_s.smte_s);
	OUTDEBUG(fprintf(stderr,"Reconstructing union find\n"));
	connectivity_check.clear();
	for(auto a : edge)
		for(auto b : a.second)
		{
			int s1 = a.first;
			int s2 = b.first;
			OUTDEBUG(fprintf(stderr, "\t[SMT]Reseting %d-%d\n", s1, s2));
			if(connectivity_check.find(s1) == nullptr)
				connectivity_check.add(s1);
			if(connectivity_check.find(s2) == nullptr)
				connectivity_check.add(s2);
			connectivity_check.make_union(s1,s2);
		}
	OUTDEBUG(fprintf(stderr,"Non equalities hold: \n"));
	for(auto s1p : not_possible)
		for(auto s2p : s1p.second)
		{
			if(connectivity_check.find(s1p.first) == nullptr)
				connectivity_check.add(s1p.first);
			if(connectivity_check.find(s2p.first) == nullptr)
				connectivity_check.add(s2p.first);
			OUTDEBUG(fprintf(stderr, "%d != %d\n", s1p.first, s2p.first));
		}
}

void SMTSolver_eq::cancel_last_decision()
{
	assert(settings_s.smte_s);
	int last_dec_index = solver->decision_stack.size()-1;
	decision last_decision = solver->decision_stack.back();

	if(solver->dpll_to_smt.find(abs(last_decision.dec)) == solver->dpll_to_smt.end())
		return;

	smt_literal_eq* corresponding_lit = (smt_literal_eq*)solver->dpll_to_smt[abs(last_decision.dec)];

	int s1 = min(corresponding_lit->left, corresponding_lit->right);
	int s2 = max(corresponding_lit->left, corresponding_lit->right);

	bool add_edge = ((last_decision.dec > 0) && corresponding_lit->equal) || ((last_decision.dec < 0) && !corresponding_lit->equal);

	if(add_edge)
	{
		edge[s1].erase(s2);
		edge[s2].erase(s1);
		if(edge[s1].size() == 0)
			edge.erase(s1);
		if(edge[s2].size() == 0)
			edge.erase(s2);
		OUTDEBUG(fprintf(stderr,"\t[SMT]Canceling %d-%d\n", s1, s2));
		return;
	}	
	OUTDEBUG(fprintf(stderr,"\t[SMT]Canceling %d != %d\n", s1, s2));
	not_possible[s1].erase(s2);
}

/* literal violated */
int SMTSolver_eq::apply_last_decision()
{
	assert(settings_s.smte_s);
	int last_dec_index = solver->decision_stack.size()-1;
	decision last_decision = solver->decision_stack.back();

	if(solver->dpll_to_smt.find(abs(last_decision.dec)) == solver->dpll_to_smt.end())
		return -1;

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
					if(connectivity_check.find(s1p.first) != nullptr && connectivity_check.find(s1p.first) == connectivity_check.find(s2p.first))
					{
						OUTDEBUG(fprintf(stderr, "\t[SMT]Clash, decision %d (%s) violated, returning %d!\n", solver->decision_stack[s2p.second].dec, solver->dpll_to_smt[abs(solver->decision_stack[s2p.second].dec)]->to_str().c_str(),s2p.second));
						return s2p.second;
					}
				}
		}
		return -1;
	}

	not_possible[s1][s2] = last_dec_index;
	OUTDEBUG(fprintf(stderr, "\t[SMT]Remembering that %d and %d shouldn't be connected.\n", s1, s2));
	if(connectivity_check.find(s1)  != nullptr && connectivity_check.find(s1) == connectivity_check.find(s2))
	{
		OUTDEBUG(fprintf(stderr, "\t[SMT]Clash, decision %d (%s) violated, returning %d!\n", solver->decision_stack[last_dec_index].dec, solver->dpll_to_smt[abs(solver->decision_stack[last_dec_index].dec)]->to_str().c_str(),last_dec_index));
		return last_dec_index;
	}
	return -1;
}

bool SMTSolver_eq::dfs_enumerate_paths(int curr, int dest, map<int,int>& succ)
{
	assert(settings_s.smte_s);
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
	assert(settings_s.smte_s);

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
	int bt_level = 0;
	int uip_like = solver->decision_stack.size()-1;
	while(curr != s2)
	{
		assert(edge[curr][succ[curr]] != conflict_dec_index);
		if(edge[curr][succ[curr]] != uip_like) 
			bt_level = max(bt_level, solver->decision_stack[edge[curr][succ[curr]]].level);
		if(solver->decision_stack[edge[curr][succ[curr]]].level != -1)
		{
			c.literal.push_back(-solver->decision_stack[edge[curr][succ[curr]]].dec);
			c.assoc_lit[-solver->decision_stack[edge[curr][succ[curr]]].dec] = true;
		}
		OUTDEBUG(fprintf(stderr, "\t\t%d %d, dec: %d lvl: %d\n", curr, succ[curr], solver->decision_stack[edge[curr][succ[curr]]].dec, solver->decision_stack[edge[curr][succ[curr]]].level));
		curr = succ[curr];
	}

	OUTDEBUG(fprintf(stderr, "\n"));

	if(solver->decision_stack[conflict_dec_index].level != -1)
	{
		c.literal.push_back(-solver->decision_stack[conflict_dec_index].dec);
		c.assoc_lit[-solver->decision_stack[conflict_dec_index].dec] = true;
	}
	if(conflict_dec_index != uip_like)
		bt_level = max(bt_level, solver->decision_stack[conflict_dec_index].level);
	
	if(c.literal.size() != 1 && settings_s.wl_s)
	{
		c.triggers.insert(-solver->decision_stack[uip_like].dec);
		for(int i = uip_like-1 ; i >= 0 ; i--)
			if(c.assoc_lit.find(-solver->decision_stack[i].dec) != c.assoc_lit.end())
			{
				c.triggers.insert(-solver->decision_stack[i].dec);
				break;
			}
	}

	OUTDEBUG(fprintf(stderr, "\t[SMT]Learning %s\n", c.to_str().c_str()));
	OUTDEBUG(fprintf(stderr, "\t[SMT]Backtracking to %d\n", bt_level));


	
	return make_pair(c,bt_level);
}

void SMTSolver_eq::visite_composante(int curr, int id, map<int,int>& id_composante)
{
	if(id_composante.find(curr) != id_composante.end())
		return;
	id_composante[curr] = id;
	for(auto v : edge[curr])
		visite_composante(v.first, id, id_composante);
}

void SMTSolver_eq::get_solution()
{
	OUTDEBUG(cerr << "[SMT]Computing solution." << endl);
	map<int,int> id_composante;
	int id = 0;
	for(auto s : edge)
		if(id_composante.find(s.first) == id_composante.end())
		{
			visite_composante(s.first, id, id_composante);
			id++;
		}

	for(auto s1 : not_possible)
		for(auto s2: s1.second)
		{
			if(id_composante.find(s1.first) == id_composante.end())
			{
				id_composante[s1.first] = id;
				id++;
			}
			if(id_composante.find(s2.first) == id_composante.end())
			{
				id_composante[s2.first] = id;
				id++;
			}
		}

	map<int,vector<int>> composante;
	for(auto a : id_composante)
		composante[a.second].push_back(a.first);

	for(auto c : composante)
	{
		OUTDEBUG(cerr << "C" << c.first << ", "<<  connectivity_check.find(c.second[0])->getValue() << ": {");
		int k = 0;
		for(auto b: c.second)
		{
			OUTDEBUG(cerr << b << ((k == c.second.size()-1) ? "" : ", "));
			k ++;
		}
		OUTDEBUG(cerr << "}" << endl);
	}

	OUTDEBUG(cerr << "Non equalities : " << endl);
	for(auto s1 : not_possible)
		for(auto s2: s1.second)
			OUTDEBUG(cerr << s1.first << " != " << s2.first << " " << connectivity_check.find(s1.first)->getValue() << " != " << connectivity_check.find(s2.first)->getValue() << endl);
	
	for(int i = 0 ; i < id ; i++)
	{
		for(int j = i+1 ; j < id ; j++)
		{
			OUTDEBUG(cerr << "Trying to merge C" << i << " and " << "C" << j << endl);
			int r1 = connectivity_check.find(composante[i][0])->getValue();
			int r2 = connectivity_check.find(composante[j][0])->getValue();
			bool possible = true;
			for(auto s1 : not_possible)
				for(auto s2: s1.second)
					if(connectivity_check.find(s1.first)->getValue() == r1 && connectivity_check.find(s2.first)->getValue() == r2 ||
				   		connectivity_check.find(s1.first)->getValue() == r2 && connectivity_check.find(s2.first)->getValue() == r1)
				   		possible = false;
			if(possible)
			{
				OUTDEBUG(cerr << "\tRepresented by " << r1 << " and " << r2 << endl);
				OUTDEBUG(cerr << "\tMerging possible !" << endl);
				connectivity_check.make_union(r1, r2);
				edge[r1][r2] = -1;
				edge[r2][r1] = -1;	

				get_solution();
				return;
			}
		}
	}

	for(auto c : composante)
	{
		cout << "C(" << connectivity_check.find(c.second[0])->getValue() << "): {";
		int k = 0;
		for(auto b: c.second)
		{
			cout << b << ((k == c.second.size()-1) ? "" : ", ");
			k ++;
		}
		cout << "}" << endl;
	}

}