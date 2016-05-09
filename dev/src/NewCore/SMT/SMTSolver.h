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
        virtual ~smt_literal() {}
};

class SMTSolver
{
	public:
		SMTSolver(SATSolver* solver) : solver(solver) {}
		virtual bool apply_last_decision() = 0;
		virtual pair<clause,int> diagnose_conflict() = 0;


	protected:
		SATSolver* solver;
};


#endif
