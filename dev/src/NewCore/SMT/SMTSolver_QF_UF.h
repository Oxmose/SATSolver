#ifndef DEF_SMT_SOLVER_QF_UF_H
#define DEF_SMT_SOLVER_QF_UF_H

#include "SMTSolver.h"

using namespace std;

struct smt_term
{
	smt_term(int index = 0, string s="", int var=0, vector<smt_term> args = vector<smt_term>()) : in_literal(index), s(s), var(var), args(args) {}


	int in_literal;
	string s;
	int var; //L'un ou l'autre
	vector<smt_term> args;

	string to_str();
};

struct smt_literal_qf_uf : smt_literal
{
	smt_literal_qf_uf(int index, smt_term left, smt_term right, bool equal) : smt_literal(index), left(left), right(right), equal(equal) {}
	virtual ~smt_literal_qf_uf() {}
	
	smt_term left;
	smt_term right;

	bool equal;
	
	string to_str();
};

class SMTSolver_qf_uf : public SMTSolver
{
	public:
		SMTSolver_qf_uf(SATSolver* solver) : SMTSolver(solver) {}

		int apply_last_decision();
		void cancel_last_decision();
		void reset_method();
		pair<clause,int> diagnose_conflict(int conflict_dec_index);

		void visite_composante(int curr, int id, map<int,int>& id_composante);
		void get_solution();

		unordered_map<string, int> terms;
		unordered_map<int, string> reverse_terms;
		
		unordered_map<int, unordered_map<int,int>> not_possible;
		UnionFind connectivity_check;
};

#endif
