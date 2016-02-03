#include "expr.hpp"

using namespace std;

/***********************************/
/**********  Constants   ***********/
/***********************************/

EConst::EConst(int val) : value(val) {}

string EConst::to_string()
{
    ostringstream oss;
    oss << value;
    string result = oss.str();
    return result;
}

int EConst::eval()
{
    return value;
}

/***********************************/
/**********  Additions   ***********/
/***********************************/

EAdd::EAdd(Expr * e1, Expr * e2) : op1(e1), op2(e2) {}

string EAdd::to_string()
{
    return "(" + op1->to_string() + " + " +  op2->to_string() + ")";
}

int EAdd::eval()
{
    return op1->eval() + op2->eval();
}

/***********************************/
/********  Multiplications   *******/
/***********************************/

EMul::EMul(Expr * e1, Expr * e2) : op1(e1), op2(e2) {}

string EMul::to_string()
{
    return "(" + op1->to_string() + " x " +  op2->to_string() + ")";
}

int EMul::eval()
{
    return op1->eval() * op2->eval();
}

