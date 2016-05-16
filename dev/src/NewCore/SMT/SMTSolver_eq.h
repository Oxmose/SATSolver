#ifndef DEF_SMT_SOLVER_EQ_H
#define DEF_SMT_SOLVER_EQ_H

#include "SMTSolver.h"
#include <unordered_map>

#include "../../Tools/UnionFind.h"

using namespace std;

struct smt_term
{
	smt_term(int index = 0, string s="", int var=0, vector<smt_term> args = vector<smt_term>()) : in_literal(index), s(s), var(var), args(args) {}

	int term_id;

	int in_literal;
	string s;
	int var; //L'un ou l'autre
	vector<smt_term> args;

	string to_str();
};

struct smt_literal_eq : smt_literal
{
	smt_literal_eq(int index, int left, int right, bool equal) : smt_literal(index), left(left), right(right), equal(equal) {}
	virtual ~smt_literal_eq() {}
	
	virtual string to_str()
	{
		return to_string(index) + ": " + to_string(left) + " " + ((equal) ? "=" : "!=") + " " + to_string(right);
	}

	int left;
	int right;
	bool equal;
};

class SMTSolver_eq : public SMTSolver
{
	public:
		SMTSolver_eq(SATSolver* solver) : SMTSolver(solver) {}

		void init();

		int apply_last_decision();
		void cancel_last_decision();
		void reset_method();
		pair<clause,int> diagnose_conflict(int conflict_dec_index);

		bool dfs_enumerate_paths(int curr, int dest, map<int,int>& succ);

		void visite_composante(int curr, int id, map<int,int>& id_composante);
		void get_solution();

		/* QF_UF */
		bool sig(int x, int y);
		void merge(int s1, int s2);

		unordered_map<int, unordered_map<int,int>> edge;
		unordered_map<int, unordered_map<int,int>> not_possible;
		unordered_map<int, set<int>> has_as_arg;
		UnionFind connectivity_check;
};

#endif
