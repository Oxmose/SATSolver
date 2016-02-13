#include "expr.hpp"

using namespace std;

/***********************************/
/**********  Variables   ***********/
/***********************************/

EVar::EVar(int i) : index(i) {}

string EVar::to_string()
{
    ostringstream oss;
    oss << index;
    string result = oss.str();
    return result;
}

/***********************************/
/*********  Equivalence   **********/
/***********************************/

EEqu::EEqu(Expr * e1, Expr * e2) : op1(e1), op2(e2) {}

string EEqu::to_string()
{
    return "(" + op1->to_string() + " <=> " +  op2->to_string() + ")";
}

EImp::EImp(Expr * e1, Expr * e2) : op1(e1), op2(e2) {}

string EImp::to_string()
{
    return "(" + op1->to_string() + " => " +  op2->to_string() + ")";
}

EXor::EXor(Expr * e1, Expr * e2) : op1(e1), op2(e2) {}

string EXor::to_string()
{
    return "(" + op1->to_string() + " X " +  op2->to_string() + ")";
}

EDis::EDis(Expr * e1, Expr * e2) : op1(e1), op2(e2) {}

string EDis::to_string()
{
    return "(" + op1->to_string() + " \\/ " +  op2->to_string() + ")";
}

ECon::ECon(Expr * e1, Expr * e2) : op1(e1), op2(e2) {}

string ECon::to_string()
{
    return "(" + op1->to_string() + " /\\ " +  op2->to_string() + ")";
}

EAnt::EAnt(Expr * e1) : op1(e1) {}

string EAnt::to_string()
{
    return "(~" + op1->to_string() + ")";
}

ENeg::ENeg(Expr * e1) : op1(e1) {}

string ENeg::to_string()
{
    return "(-" + op1->to_string() + ")";
}