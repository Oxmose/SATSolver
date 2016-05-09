#ifndef DEF_SMT_SOLVER_EQ_H
#define DEF_SMT_SOLVER_EQ_H

#include "SMTSolver.h"

#include "../../Tools/UnionFind.h"
#include <unordered_map>

using namespace std;

struct smt_literal_eq : smt_literal
{
	smt_literal_eq(int index, int left, int right, bool equal) : smt_literal(index), left(left), right(right), equal(equal) {}
	~smt_literal_eq() {}

	string to_str()
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

		bool apply_last_decision();
		pair<clause,int> diagnose_conflict();

		unordered_map<int, unordered_map<int,pair<int,int>>> edge;//Lit, pond
		unordered_map<int, unordered_map<int, int>> not_possible;
		UnionFind<int> connectivity_check;
};

#endif