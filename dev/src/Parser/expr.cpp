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
#include <sstream>      //std::osstringstream
#include <string>       //std::string
#include <vector>       //std::vector
#include <algorithm>    //stdfind
#include <iostream>

// CLASS HEADER INCLUDE
#include "expr.hpp"

using namespace std;

/***********************************/
/**********  Variables   ***********/
/***********************************/

EVar::EVar(int i) : index(i) {}

Expr* EVar::tseitin(int &p_maxIndex, vector<Expr*> &p_exps, 
                    map<pair<int, int>, Expr*> &corresp, map<pair<int, int>, Expr*> &ncorresp, 
                    map<string, function_s*> &funcorresp, map<string, unsigned int> &arrities,
                    map<string, args_s*> &argscorresp, bool trans)
{
    // Then We create the new variable for this expression
    if(trans)
    {
        ++p_maxIndex;
        Expr* var = new EVar(p_maxIndex);
        Expr* varNeg = new ENeg(var);
        p_exps.push_back(new ECon(new EDis(varNeg, this), new EDis(var, new ENeg(this))));
        return var;
    }
    else
    {
        return this;
    }
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

int EVar::getVarTerm()
{
    string str = to_string();

    if(str[0] == '(')
        str = str.substr(1, str.size() - 2);

    return stoi(str);
} // int getVatTerm()

unsigned int EVar::size()
{
    return 1;
} // unsigned int size()

/***********************************/
/**********  Functions   ***********/
/***********************************/

EFun::EFun(char *name, Expr* e) : m_args(e) 
{
    string str_name = string(name);
    m_name = str_name.substr(0, str_name.find("("));
}

Expr* EFun::tseitin(int &p_maxIndex, vector<Expr*> &p_exps, 
                    map<pair<int, int>, Expr*> &corresp, map<pair<int, int>, Expr*> &ncorresp, 
                    map<string, function_s*> &funcorresp, map<string, unsigned int> &arrities,
                    map<string, args_s*> &argscorresp, bool trans)
{
    // Check arrity
    unsigned int arrity = m_args->size();
    if(arrities.find(m_name) != arrities.end())
        if(arrities[m_name] != arrity)
        {
            cerr << "Found functions with differents argumetns count\n" << endl;
            exit(1);
        }

    cout << m_name << " has arrity " << arrity << endl;
    fflush(stdout);

    string key = to_string();
    if(funcorresp.find(key) != funcorresp.end())
    {
        //p_exps.push_back(funcorresp[key]->ptr);
        return funcorresp[key]->ptr;
    }

    // Then We create the new variable for this expression
    m_args->tseitin(p_maxIndex, p_exps, corresp, ncorresp, funcorresp, arrities, argscorresp, false);

    string newkey = m_args->to_string();
    if(argscorresp.find(newkey) == argscorresp.end())
    {
        args_s *arg = new args_s;
        arg->ptr = m_args;
        arg->count = 1;
        arg->assoc_var = m_args->getVarTerm();

        argscorresp[newkey] = arg;
    } 
    
    
    // Then we create the new variable for this expression
    ++p_maxIndex;
    Expr* var = new EVar(p_maxIndex);

    function_s *fun  = new function_s;
    fun->name = key;
    fun->arrity = arrity;
    fun->assoc_var = p_maxIndex;
    fun->ptr = this;

    index = p_maxIndex;   

    funcorresp[key] = fun;

    // Then we return the newly created variable
    return var;
} // Expr* tseitin(int&, vector<Expr*>&)

void EFun::getVars(vector<int> &p_originalVars)
{
    m_args->getVars(p_originalVars);
} // getVars(vector<int> &)

string EFun::to_string()
{
    ostringstream oss;
    oss << m_name << "("; 
    oss << m_args->to_string();
    oss << ")";
    string result = oss.str();
    return result;
} // string tos_tring()

int EFun::getVarTerm()
{    
    return index;
} // int getVatTerm()

unsigned int EFun::size()
{
    return 1;
} // unsigned int size()

/***********************************/
/*********  Arguments     **********/
/***********************************/

EArg::EArg(Expr * e1, Expr * e2) : op1(e1), op2(e2) {}

Expr* EArg::tseitin(int &p_maxIndex, vector<Expr*> &p_exps, 
                    map<pair<int, int>, Expr*> &corresp, map<pair<int, int>, Expr*> &ncorresp, 
                    map<string, function_s*> &funcorresp, map<string, unsigned int> &arrities,
                    map<string, args_s*> &argscorresp, bool trans)
{
    op1->tseitin(p_maxIndex, p_exps, corresp, ncorresp, funcorresp, arrities, argscorresp, false);
    op2->tseitin(p_maxIndex, p_exps, corresp, ncorresp, funcorresp, arrities, argscorresp, false);
    string key = to_string();
    if(argscorresp.find(key) != argscorresp.end())
    {
        //p_exps.push_back(argscorresp[key]->ptr);
        
        return argscorresp[key]->ptr;
    }
    
    // Then we create the new variable for this expression
    ++p_maxIndex;
    Expr* var = new EVar(p_maxIndex);

    args_s *arg = new args_s;
    arg->ptr = this;
    arg->count = size();
    arg->assoc_var = p_maxIndex;

    argscorresp[key] = arg;

    // Then we return the newly created variable
    return var;
} // Expr* tseitin(int&, vector<Expr*>&)

void EArg::getVars(vector<int> &p_originalVars)
{
    // Get vars from the leaves of this expression
    op1->getVars(p_originalVars);
    op2->getVars(p_originalVars);
} // getVars(vector<int>&)

string EArg::to_string()
{
    return op1->to_string() + ", " +  op2->to_string();
} // string to_tring()

int EArg::getVarTerm()
{
    return index;
} // int getVatTerm()

unsigned int EArg::size()
{
    return op1->size() + op2->size();
} // unsigned int size()

/***********************************/
/*********  Equivalence   **********/
/***********************************/

EEqu::EEqu(Expr * e1, Expr * e2) : op1(e1), op2(e2) {}

Expr* EEqu::tseitin(int &p_maxIndex, vector<Expr*> &p_exps, 
                    map<pair<int, int>, Expr*> &corresp, map<pair<int, int>, Expr*> &ncorresp, 
                    map<string, function_s*> &funcorresp, map<string, unsigned int> &arrities,
                    map<string, args_s*> &argscorresp, bool trans)
{
    // First we transform every sub expressions int the expression
    // and get the newly added variable
    Expr* op1Tran = op1->tseitin(p_maxIndex, p_exps, corresp, ncorresp, funcorresp, arrities, argscorresp, true);
    Expr* op2Tran = op2->tseitin(p_maxIndex, p_exps, corresp, ncorresp, funcorresp, arrities, argscorresp, true);
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

int EEqu::getVarTerm()
{
    return 0;
} // int getVatTerm()

unsigned int EEqu::size()
{
    return 0;
} // unsigned int size()

/***********************************/
/**********  Implication   *********/
/***********************************/

EImp::EImp(Expr * e1, Expr * e2) : op1(e1), op2(e2) {}

Expr* EImp::tseitin(int &p_maxIndex, vector<Expr*> &p_exps, 
                    map<pair<int, int>, Expr*> &corresp, map<pair<int, int>, Expr*> &ncorresp, 
                    map<string, function_s*> &funcorresp, map<string, unsigned int> &arrities,
                    map<string, args_s*> &argscorresp, bool trans)
{
    Expr* op1Tran = op1->tseitin(p_maxIndex, p_exps, corresp, ncorresp, funcorresp, arrities, argscorresp, true);
    Expr* op2Tran = op2->tseitin(p_maxIndex, p_exps, corresp, ncorresp, funcorresp, arrities, argscorresp, true);

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

int EImp::getVarTerm()
{
    return 0;
} // int getVatTerm()

unsigned int EImp::size()
{
    return 0;
} // unsigned int size()

/***********************************/
/*********  Exclusive Or   *********/
/***********************************/

EXor::EXor(Expr * e1, Expr * e2) : op1(e1), op2(e2) {}

Expr* EXor::tseitin(int &p_maxIndex, vector<Expr*> &p_exps, 
                    map<pair<int, int>, Expr*> &corresp, map<pair<int, int>, Expr*> &ncorresp, 
                    map<string, function_s*> &funcorresp, map<string, unsigned int> &arrities,
                    map<string, args_s*> &argscorresp, bool trans)
{
    Expr* op1Tran = op1->tseitin(p_maxIndex, p_exps, corresp, ncorresp, funcorresp, arrities, argscorresp, true);
    Expr* op2Tran = op2->tseitin(p_maxIndex, p_exps, corresp, ncorresp, funcorresp, arrities, argscorresp, true);
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

int EXor::getVarTerm()
{
    return 0;
} // int getVatTerm()

unsigned int EXor::size()
{
    return 0;
} // unsigned int size()

/***********************************/
/**********  Disjunction   *********/
/***********************************/

EDis::EDis(Expr * e1, Expr * e2) : op1(e1), op2(e2) {}

Expr* EDis::tseitin(int &p_maxIndex, vector<Expr*> &p_exps, 
                    map<pair<int, int>, Expr*> &corresp, map<pair<int, int>, Expr*> &ncorresp, 
                    map<string, function_s*> &funcorresp, map<string, unsigned int> &arrities,
                    map<string, args_s*> &argscorresp, bool trans)
{
    Expr* op1Tran = op1->tseitin(p_maxIndex, p_exps, corresp, ncorresp, funcorresp, arrities, argscorresp, true);
    Expr* op2Tran = op2->tseitin(p_maxIndex, p_exps, corresp, ncorresp, funcorresp, arrities, argscorresp, true);
    
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

int EDis::getVarTerm()
{
    return 0;
} // int getVatTerm()

unsigned int EDis::size()
{
    return 0;
} // unsigned int size()

/***********************************/
/**********  Conjunction   *********/
/***********************************/

ECon::ECon(Expr * e1, Expr * e2) : op1(e1), op2(e2) {}

Expr* ECon::tseitin(int &p_maxIndex, vector<Expr*> &p_exps, 
                    map<pair<int, int>, Expr*> &corresp, map<pair<int, int>, Expr*> &ncorresp, 
                    map<string, function_s*> &funcorresp, map<string, unsigned int> &arrities,
                    map<string, args_s*> &argscorresp, bool trans)
{
    Expr* op1Tran = op1->tseitin(p_maxIndex, p_exps, corresp, ncorresp, funcorresp, arrities, argscorresp, true);
    Expr* op2Tran = op2->tseitin(p_maxIndex, p_exps, corresp, ncorresp, funcorresp, arrities, argscorresp, true);

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

int ECon::getVarTerm()
{
    return 0;
} // int getVatTerm()

unsigned int ECon::size()
{
    return 0;
} // unsigned int size()

/***********************************/
/**********  Antithesis   **********/
/***********************************/

EAnt::EAnt(Expr * e1) : op1(e1) {}

Expr* EAnt::tseitin(int &p_maxIndex, vector<Expr*> &p_exps, 
                    map<pair<int, int>, Expr*> &corresp, map<pair<int, int>, Expr*> &ncorresp, 
                    map<string, function_s*> &funcorresp, map<string, unsigned int> &arrities,
                    map<string, args_s*> &argscorresp, bool trans)
{
    Expr* op1Tran = op1->tseitin(p_maxIndex, p_exps, corresp, ncorresp, funcorresp, arrities, argscorresp, true);

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

int EAnt::getVarTerm()
{
    return 0;
} // int getVatTerm()

unsigned int EAnt::size()
{
    return 0;
} // unsigned int size()

/***********************************/
/**********  Negation   ************/
/***********************************/

ENeg::ENeg(Expr * e1) : op1(e1) {}

Expr* ENeg::tseitin(int &p_maxIndex, vector<Expr*> &p_exps, 
                    map<pair<int, int>, Expr*> &corresp, map<pair<int, int>, Expr*> &ncorresp, 
                    map<string, function_s*> &funcorresp, map<string, unsigned int> &arrities,
                    map<string, args_s*> &argscorresp, bool trans)
{
    //Expr* op1Tran = op1->tseitin(p_maxIndex, p_exps, corresp, ncorresp, funcorresp, arrities, argscorresp, true);

    ++p_maxIndex;
    Expr *var = new EVar(p_maxIndex);

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

int ENeg::getVarTerm()
{
    return 0;
} // int getVatTerm()

unsigned int ENeg::size()
{
    return 0;
} // unsigned int size()

/***********************************/
/**********  Equality   ************/
/***********************************/

EEqua::EEqua(Expr * e1, Expr * e2) : op1(e1), op2(e2) {}

Expr* EEqua::tseitin(int &p_maxIndex, vector<Expr*> &p_exps, 
                    map<pair<int, int>, Expr*> &corresp, map<pair<int, int>, Expr*> &ncorresp, 
                    map<string, function_s*> &funcorresp, map<string, unsigned int> &arrities,
                    map<string, args_s*> &argscorresp, bool trans)
{
    op1->tseitin(p_maxIndex, p_exps, corresp, ncorresp, funcorresp, arrities, argscorresp, false);
    op2->tseitin(p_maxIndex, p_exps, corresp, ncorresp, funcorresp, arrities, argscorresp, false);

    pair<int, int> pair_t = make_pair(op1->getVarTerm(), op2->getVarTerm());

    if(corresp.find(pair_t) != corresp.end())
    {
        p_exps.push_back(corresp[pair_t]);
        return corresp[pair_t];
    }

    // Then we create the new variable for this expression
    ++p_maxIndex;
    Expr* var = new EVar(p_maxIndex);
    Expr* ret = var->tseitin(p_maxIndex, p_exps, corresp, ncorresp, funcorresp, arrities, argscorresp, true);

    corresp[pair_t] = ret;

    // Then we return the newly created variable
    return ret;
} // Expr* tseitin(int&, vector<Expr*>&)

void EEqua::getVars(vector<int> &p_originalVars)
{
    // Get vars from the leaves of this expression
    op1->getVars(p_originalVars);
    op2->getVars(p_originalVars);
} // getVars(vector<int>&)

string EEqua::to_string()
{
    return "(" + op1->to_string() + " = " +  op2->to_string() + ")";
} // string to_tring()

int EEqua::getVarTerm()
{
    return 0;
} // int getVatTerm()

unsigned int EEqua::size()
{
    return 0;
} // unsigned int size()

/***********************************/
/********  Non Equality   **********/
/***********************************/

ENEqua::ENEqua(Expr * e1, Expr * e2) : op1(e1), op2(e2) {}

Expr* ENEqua::tseitin(int &p_maxIndex, vector<Expr*> &p_exps, 
                    map<pair<int, int>, Expr*> &corresp, map<pair<int, int>, Expr*> &ncorresp, 
                    map<string, function_s*> &funcorresp, map<string, unsigned int> &arrities,
                    map<string, args_s*> &argscorresp, bool trans)
{
    pair<int, int> pair_t = make_pair(op1->getVarTerm(), op2->getVarTerm());

    if(ncorresp.find(pair_t) != ncorresp.end())
    {
        p_exps.push_back(ncorresp[pair_t]);
        return ncorresp[pair_t];
    }

    // Then we create the new variable for this expression
    ++p_maxIndex;
    Expr* var = new EVar(p_maxIndex);
    Expr* ret = var->tseitin(p_maxIndex, p_exps, corresp, ncorresp, funcorresp, arrities, argscorresp, true);

    ncorresp[pair_t] = ret;

    // Then we return the newly created variable
    return ret;
} // Expr* tseitin(int&, vector<Expr*>&)

void ENEqua::getVars(vector<int> &p_originalVars)
{
    // Get vars from the leaves of this expression
    op1->getVars(p_originalVars);
    op2->getVars(p_originalVars);
} // getVars(vector<int>&)

string ENEqua::to_string()
{
    return "(" + op1->to_string() + " != " +  op2->to_string() + ")";
} // string to_tring()

int ENEqua::getVarTerm()
{
    return 0;
} // int getVatTerm()

unsigned int ENEqua::size()
{
    return 0;
} // unsigned int size()
