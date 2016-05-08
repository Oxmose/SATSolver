#ifndef DEF_SMT_SOLVER_EQ_H
#define DEF_SMT_SOLVER_EQ_H

#include "SMTSolver.h"

struct smt_literal_eq : smt_literal
{
	smt_literal_eq(int index) : smt_literal(index) {}
	int left;
	int right;
};

class SMTSolver_eq : SMTSolver
{
	public:
		SMTSolver_eq(SATSolver* solver) : SMTSolver(solver) {}

		bool apply_last_decision();
		pair<clause,int> diagnose_conflict();
};

#endif