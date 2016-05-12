#include "SMTSolver_QF_UF.h"

string smt_term::to_str()
{
	if(var != 0)
		return to_string(var);
	if(args.size() == 0)
		return to_string(s);
	string s = to_string(s)+"(";
	for(auto a : args)
		s += a.to_str();
	s += ")";
	return s;
}