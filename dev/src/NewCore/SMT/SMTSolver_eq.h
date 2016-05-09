#ifndef DEF_SMT_SOLVER_EQ_H
#define DEF_SMT_SOLVER_EQ_H

#include "SMTSolver.h"

using namespace std;

struct smt_literal_eq : smt_literal
{
	smt_literal_eq(int index, int left, int right, bool equal) : smt_literal(index), left(left), right(right), equal(equal) {}

        virtual ~smt_literal_eq() {}
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
};

#endif
