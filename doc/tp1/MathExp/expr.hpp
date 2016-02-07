#ifndef DEF_EXPR
#define DEF_EXPR

#include <iostream>
#include <sstream> 
#include <string>

/***********************************/
/*********  Expressions   **********/
/***********************************/

class Expr
{
public:
    Expr(){}
    virtual std::string to_string()=0;
    virtual int eval()=0;
};

/***********************************/
/**********  Constants   ***********/
/***********************************/

class EConst : public Expr
{
public:
    EConst(int val); 
    virtual std::string to_string();
    virtual int eval();
private:
    int value;
};

/***********************************/
/**********  Additions   ***********/
/***********************************/

class EAdd : public Expr
{
public:
    EAdd(Expr * e1, Expr * e2);
    virtual std::string to_string();
    virtual int eval();
private:
    Expr * op1, * op2;
};

/***********************************/
/********  Multiplications  ********/
/***********************************/

class EMul : public Expr
{
public:
    EMul(Expr * e1, Expr * e2);
    virtual std::string to_string();
    virtual int eval();
private:
    Expr * op1, * op2;
};

#endif // ! DEF_EXPR
