#ifndef DEF_SMT_SOLVER_QF_UF_H
#define DEF_SMT_SOLVER_QF_UF_H

#include "SMTSolver.h"

using namespace std;

struct smt_term
{
	smt_term(int index = 0, char s=0, int var=0, vector<smt_term> args = vector<smt_term>()) : in_literal(index), s(s), var(var), args(args) {}


	int in_literal;
	char s;
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

#endif