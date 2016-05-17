#ifndef DEF_SMT_SOLVER_EQ_H
#define DEF_SMT_SOLVER_EQ_H

#include "SMTSolver.h"
#include <unordered_map>

using namespace std;

/*

Everything reduces to:

	-type 0: x_i < x_j + c
	-type 1: x_i = a
	-type 2: x_i != a
*/
struct smt_literal_d : smt_literal
{
	smt_literal_d(int index=0, int type=0, int left=0, int coeff=0, int right=0) : smt_literal(index), type(type), left(left), coeff(coeff), right(right) {}
	virtual ~smt_literal_d() {}
	
	virtual string to_str()
	{
		string sig = (coeff < 0) ? " - " : " + ";
		switch(type)
		{
			case 0:
				return to_string(index) + ": " + to_string(left) + " < " + to_string(right) + sig + to_string(abs(coeff)); 
			case 1:
				return to_string(index) + ": " + to_string(left) + " = " + to_string(coeff);
			case 2:
				return to_string(index) + ": " + to_string(left) + " != " + to_string(coeff);
		}
	}

	int type, left, coeff, right;
};