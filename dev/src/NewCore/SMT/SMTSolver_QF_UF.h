#ifndef DEF_SMT_SOLVER_QF_UF_H
#define DEF_SMT_SOLVER_QF_UF_H

#include "SMTSolver.h"

#include "../../Tools/UnionFind.h"

using namespace std;

struct smt_term : smt_literal
{
	smt_term(int index, char s=0, int var=0, vector<smt_term> args=vector<smt_term>()) : smt_literal(index), s(s), var(var), args(args) {}
	~smt_term() {}
	
	char s;
	int var; //L'un ou l'autre
	vector<smt_term> args;

	string to_str();
};

#endif