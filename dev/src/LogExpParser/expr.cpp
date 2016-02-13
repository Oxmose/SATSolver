/*
 *
 *    CLASS Expr and derivate
 *
 *
 *  We explained the mechanism for EVar and EEqu classes
 *  The comments would be the smae for each other classes
 *
 *
*/

// STD INCLUDES
#include <sstream>      //osstringstream
#include <string>       //string
#include <vector>       //sd::vector
#include <algorithm>    //find

// CLASS HEADER INCLUDE
#include "expr.hpp"

using namespace std;

/***********************************/
/**********  Variables   ***********/
/***********************************/

EVar::EVar(int i) : index(i) {}

Expr* EVar::tseitin(int &p_maxIndex, vector<Expr*> &p_exps)
{
    // Since vars are leaves of the trees, returns itselve
    return this;
} // Expr* tseitin(int&, vector<Expr*>&)

void EVar::getVars(vector<int> &p_originalVars)
{
    // Fill the vector with the index of the var if not already in
    if(find(p_originalVars.begin(), p_originalVars.end(), index) == p_originalVars.end())
        p_originalVars.push_back(index);
} // getVars(vector<int> &)

string EVar::to_string()
{
    ostringstream oss;
    oss << index;
    string result = oss.str();
    return result;
} // string tos_tring()


/***********************************/
/*********  Equivalence   **********/
/***********************************/

EEqu::EEqu(Expr * e1, Expr * e2) : op1(e1), op2(e2) {}

Expr* EEqu::tseitin(int &p_maxIndex, vector<Expr*> &p_exps)
{
    // First we transform every sub expressions int the expression
    // and get the newly added variable
    Expr* op1Tran = op1->tseitin(p_maxIndex, p_exps);
    Expr* op2Tran = op2->tseitin(p_maxIndex, p_exps);
    Expr* op1TranNeg = new ENeg(op1Tran);
    Expr* op2TranNeg = new ENeg(op2Tran);

    // Then we create the new variable for this expression
    ++p_maxIndex;
    Expr* var = new EVar(p_maxIndex);
    Expr* varNeg = new ENeg(var);

    // And then we transform the expression (CF internet or logical expressions)
    Expr* clauseL = new EDis(varNeg, op1TranNeg);
    Expr* clauseR = new EDis(varNeg, op2TranNeg);
    Expr* clauseTL = new EDis(op1TranNeg, op2TranNeg);
    Expr* clauseTR = new EDis(op1Tran, op2Tran);
    
    p_exps.push_back(new ECon(
                        new EDis(clauseL, op2Tran),
                        new ECon(
                            new EDis(clauseR, op1Tran),
                            new ECon(
                                new EDis(clauseTL, var),
                                new EDis(clauseTR, var)))));

    // Then we return the newly created variable
    return var;
} // Expr* tseitin(int&, vector<Expr*>&)

void EEqu::getVars(vector<int> &p_originalVars)
{
    // Get vars from the leaves of this expression
    op1->getVars(p_originalVars);
    op2->getVars(p_originalVars);
} // getVars(vector<int>&)

string EEqu::to_string()
{
    return "(" + op1->to_string() + " <=> " +  op2->to_string() + ")";
} // string to_tring()


/***********************************/
/**********  Implication   *********/
/***********************************/

EImp::EImp(Expr * e1, Expr * e2) : op1(e1), op2(e2) {}

Expr* EImp::tseitin(int &p_maxIndex, vector<Expr*> &p_exps)
{
    Expr* op1Tran = op1->tseitin(p_maxIndex, p_exps);
    Expr* op2Tran = op2->tseitin(p_maxIndex, p_exps);

    ++p_maxIndex;
    Expr* var = new EVar(p_maxIndex);

    Expr* clauseR = new EDis(new ENeg(var), new ENeg(op1Tran));
    Expr* clauseL = new EDis(new ENeg(op2Tran), var);

    p_exps.push_back(new ECon(
                        new EDis(op1Tran, var),
                        new ECon(clauseL, new EDis(clauseR, op2Tran))));

    return var;
} // Expr* tseitin(int&, vector<Expr*>&)

void EImp::getVars(vector<int> &p_originalVars)
{
    op1->getVars(p_originalVars);
    op2->getVars(p_originalVars);
} // getVars(vector<int>&)

string EImp::to_string()
{
    return "(" + op1->to_string() + " => " +  op2->to_string() + ")";
} // string to_string()


/***********************************/
/*********  Exclusive Or   *********/
/***********************************/

EXor::EXor(Expr * e1, Expr * e2) : op1(e1), op2(e2) {}

Expr* EXor::tseitin(int &p_maxIndex, vector<Expr*> &p_exps)
{
    Expr* op1Tran = op1->tseitin(p_maxIndex, p_exps);
    Expr* op2Tran = op2->tseitin(p_maxIndex, p_exps);
    Expr* op1TranNeg = new ENeg(op1Tran);
    Expr* op2TranNeg = new ENeg(op2Tran);

    ++p_maxIndex;
    Expr* var = new EVar(p_maxIndex);
    Expr* varNeg = new ENeg(var);    

    Expr* clauseL = new EDis(op1TranNeg, op2TranNeg);
    Expr* clauseR = new EDis(op1Tran, op2Tran);
    Expr* clauseTL = new EDis(op1Tran, op2TranNeg);
    Expr* clauseTR = new EDis(op1TranNeg, op2Tran);

    p_exps.push_back(new ECon(
                        new EDis(clauseL, varNeg),
                        new ECon(
                            new EDis(clauseR, varNeg),
                            new ECon(
                                new EDis(clauseTL, var),
                                new EDis(clauseTR, var)))));

    return var;
} // Expr* tseitin(int&, vector<Expr*>&)

void EXor::getVars(vector<int> &p_originalVars)
{
    op1->getVars(p_originalVars);
    op2->getVars(p_originalVars);
} // getVars(vector<int>&)

string EXor::to_string()
{
    return "(" + op1->to_string() + " X " +  op2->to_string() + ")";
} // string to_string()


/***********************************/
/**********  Disjunction   *********/
/***********************************/

EDis::EDis(Expr * e1, Expr * e2) : op1(e1), op2(e2) {}

Expr* EDis::tseitin(int &p_maxIndex, vector<Expr*> &p_exps)
{
    Expr* op1Tran = op1->tseitin(p_maxIndex, p_exps);
    Expr* op2Tran = op2->tseitin(p_maxIndex, p_exps);
    
    ++p_maxIndex;
    Expr* var = new EVar(p_maxIndex);

    Expr* clauseL = new EDis(new ENeg(op1Tran), var);
    Expr* clauseR = new EDis(new ENeg(op2Tran), var);
    Expr* clauseT = new EDis(op1Tran, op2Tran);

    p_exps.push_back(new ECon(
                        new ECon(clauseL, clauseR),
                        new EDis(clauseT, new ENeg(var))));

    return var;
} // Expr* tseitin(int&, vector<Expr*>&)

void EDis::getVars(vector<int> &p_originalVars)
{
    op1->getVars(p_originalVars);
    op2->getVars(p_originalVars);
} // getVars(vector<int>&)

string EDis::to_string()
{
    return "(" + op1->to_string() + " \\/ " +  op2->to_string() + ")";
} // string to_string()


/***********************************/
/**********  Conjunction   *********/
/***********************************/

ECon::ECon(Expr * e1, Expr * e2) : op1(e1), op2(e2) {}

Expr* ECon::tseitin(int &p_maxIndex, vector<Expr*> &p_exps)
{
    Expr* op1Tran = op1->tseitin(p_maxIndex, p_exps);
    Expr* op2Tran = op2->tseitin(p_maxIndex, p_exps);

    ++p_maxIndex;
    Expr* var = new EVar(p_maxIndex);
    Expr* varNeg = new ENeg(var);

    Expr* clauseL = new EDis(varNeg, op1Tran);
    Expr* clauseR = new EDis(varNeg, op2Tran);
    Expr* clauseT = new EDis(new ENeg(op1Tran), new ENeg(op2Tran));

    p_exps.push_back(new ECon(
                        new ECon(clauseL, clauseR),
                        new EDis(clauseT, var)));

    return var;
} // Expr* tseitin(int&, vector<Expr*>&)

void ECon::getVars(vector<int> &p_originalVars)
{
    op1->getVars(p_originalVars);
    op2->getVars(p_originalVars);
} // getVars(vector<int>&)

string ECon::to_string()
{
    return "(" + op1->to_string() + " /\\ " +  op2->to_string() + ")";
} // string to_string()


/***********************************/
/**********  Antithesis   **********/
/***********************************/

EAnt::EAnt(Expr * e1) : op1(e1) {}

Expr* EAnt::tseitin(int &p_maxIndex, vector<Expr*> &p_exps)
{
    Expr* op1Tran = op1->tseitin(p_maxIndex, p_exps);

    ++p_maxIndex;
    Expr *var = new EVar(p_maxIndex);

    Expr* clauseNeg = new EDis(new ENeg(var), new ENeg(op1Tran));
    Expr* clausePos = new EDis(var, op1Tran);

    p_exps.push_back(new ECon(clauseNeg,clausePos));

    return var;
} // Expr* tseitin(int&, vector<Expr*>&)

void EAnt::getVars(vector<int> &p_originalVars)
{
    op1->getVars(p_originalVars);
} // getVars(vector<int>&)

string EAnt::to_string()
{
    return "(~" + op1->to_string() + ")";
} // string to_string()


/***********************************/
/**********  Negation   ************/
/***********************************/

ENeg::ENeg(Expr * e1) : op1(e1) {}

Expr* ENeg::tseitin(int &p_maxIndex, vector<Expr*> &p_exps)
{
    ++p_maxIndex;
    Expr* var = new EVar(p_maxIndex);

    Expr* clauseNeg = new EDis(new ENeg(var), new ENeg(op1));
    Expr* clausePos = new EDis(var, op1);

    p_exps.push_back(new ECon(clauseNeg,clausePos));
    return var;
                    
} // Expr* tseitin(int&, vector<Expr*>&)

void ENeg::getVars(vector<int> &p_originalVars)
{
    op1->getVars(p_originalVars);
} // getVars(vector<int>&)

string ENeg::to_string()
{
    return "(-" + op1->to_string() + ")";
} // string to_string()
