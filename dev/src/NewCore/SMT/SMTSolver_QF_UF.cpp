#include "SMTSolver_QF_UF.h"
#include <string>
#include <iostream>
#include <sstream>
using namespace std;

string smt_term::to_str()
{
	if(var != 0)
	{
		return to_string(var);
	}

	if(args.size() == 0)
	{
		return string(1, s);
	}

	

	string str = string(1, s) +"(";
	size_t size = args.size();
	for(unsigned int i = 0; i < size; ++i)
	{
		if(i == size - 1)
			str += args[i].to_str();
		else
			str += args[i].to_str() + ",";
	}
	str += ")";
	return str;
}

string smt_literal_qf_uf::to_str()
{
	return to_string(index) + ": " + left.to_str() + ((equal) ? " = " : " != ") + right.to_str();
}
