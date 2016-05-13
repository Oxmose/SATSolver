#include "SMTSolver_QF_UF.h"
#include <string>

using namespace std;

string smt_term::to_str()
{
	if(var != 0)
		return to_string(var);

	if(args.size() == 0)
		return s;

	string str = s +"(";
	for(auto a : args)
		str += a.to_str();
	str += ")";
	return str;
}

string smt_literal_qf_uf::to_str()
{
	return left.s;
	//return to_string(index) + ": " + left.to_str() + ((equal) ? " = " : " != ") + right.to_str();
}
