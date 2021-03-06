#ifndef DEF_SMT_SOLVER_H
#define DEF_SMT_SOLVER_H

#include "../clause.h"

using namespace std;

class SATSolver;

struct smt_literal
{
	smt_literal(int index) : index(index) {}
	virtual ~smt_literal() {}

	int index;
	virtual string to_str() = 0;
};

class SMTSolver
{
	public:
		SMTSolver(SATSolver* solver) : solver(solver) {}
		virtual void init() = 0;
		virtual int apply_last_decision() = 0;
		virtual void cancel_last_decision() = 0;
		virtual void reset_method() = 0;
		virtual pair<clause,int> diagnose_conflict(int conflict_dec_index) = 0;
		virtual void get_solution() = 0;

	protected:
		SATSolver* solver;
};


#endif
