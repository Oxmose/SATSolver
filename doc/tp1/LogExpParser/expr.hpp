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
};

/***********************************/
/**********  Variables   ***********/
/***********************************/

class EVar : public Expr
{
public:
    EVar(int i); 
    virtual std::string to_string();
private:
    int index;
};

/***********************************/
/*********  Equivalence   **********/
/***********************************/

class EEqu : public Expr
{
public:
    EEqu(Expr * e1, Expr * e2);
    virtual std::string to_string();
private:
    Expr * op1, * op2;
};

/***********************************/
/**********  Implication   *********/
/***********************************/

class EImp : public Expr
{
public:
    EImp(Expr * e1, Expr * e2);
    virtual std::string to_string();
private:
    Expr * op1, * op2;
};

/***********************************/
/*********  Exclusive Or   *********/
/***********************************/

class EXor : public Expr
{
public:
    EXor(Expr * e1, Expr * e2);
    virtual std::string to_string();
private:
    Expr * op1, * op2;
};

/***********************************/
/**********  Disjunction   *********/
/***********************************/

class EDis : public Expr
{
public:
    EDis(Expr * e1, Expr * e2);
    virtual std::string to_string();
private:
    Expr * op1, * op2;
};

/***********************************/
/**********  Conjunction   *********/
/***********************************/

class ECon : public Expr
{
public:
    ECon(Expr * e1, Expr * e2);
    virtual std::string to_string();
private:
    Expr * op1, * op2;
};

/***********************************/
/**********  Antithesis   **********/
/***********************************/

class EAnt : public Expr
{
public:
    EAnt(Expr * e1);
    virtual std::string to_string();
private:
    Expr * op1;
};

/***********************************/
/**********  Negation   ************/
/***********************************/

class ENeg : public Expr
{
public:
    ENeg(Expr * e1);
    virtual std::string to_string();
private:
    Expr * op1;
};

#endif // ! DEF_EXPR
