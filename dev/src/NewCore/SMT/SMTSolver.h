#ifndef DEF_SMT_SOLVER_H
#define DEF_SMT_SOLVER_H

#include "../clause.h"

class SATSolver;

struct smt_literal
{
	smt_literal(int index) : index(index) {}
	int index;
};

class SMTSolver
{
	public:
		SMTSolver(SATSolver* solver) : solver(solver) {}

		virtual bool apply_last_decision() = 0;
		virtual pair<clause,int> diagnose_conflict() = 0;


	private:

		SATSolver* solver;
};


#endif