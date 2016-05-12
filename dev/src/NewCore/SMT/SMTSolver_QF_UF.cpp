#include "SMTSolver_QF_UF.h"
#include <string>

using namespace std;

string smt_term::to_str()
{
	if(var != 0)
		return to_string(var);
	if(args.size() == 0)
		return to_string(s);
	string s = this->s +"(";
	for(auto a : args)
		s += a.to_str();
	s += ")";
	return s;
}

string smt_literal_qf_uf::to_str()
{
	return to_string(index) + ": " + left.to_str() + ((equal) ? " = " : " != ") + right.to_str();
}
