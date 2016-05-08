#ifndef DEF_EXPR_HPP
#define DEF_EXPR_HPP

/*
 *
 *    CLASS Expr and derivate
 *
*/

// STD INCLUDES
#include <sstream>      //std::osstringstream
#include <string>       //std::string
#include <vector>       //sd::vector
#include <algorithm>    //std::find
#include <map>          //std::map

/***********************************/
/*********  Expressions   **********/
/***********************************/
class EVar;

class Expr
{
    public:
        Expr(){}

        virtual Expr* tseitin(int &p_maxIndex, std::vector<Expr*> &p_exps, std::map<std::pair<int, int>, Expr*> &corresp, std::map<std::pair<int, int>, Expr*> &ncorresp)=0;
        virtual void getVars(std::vector<int> &p_originalVars)=0;
        virtual std::string to_string()=0;
    
        Expr * op1, * op2;
        int index;
}; // Expr

/***********************************/
/**********  Variables   ***********/
/***********************************/

class EVar : public Expr
{
    public:
        EVar(int i); 

        // Tseitin transformation of this expression
        virtual Expr* tseitin(int &p_maxIndex, std::vector<Expr*> &p_exps, std::map<std::pair<int, int>, Expr*> &corresp, std::map<std::pair<int, int>, Expr*> &ncorresp);
    
        // Return the variable indexes (in-out parameter) of the expression
        virtual void getVars(std::vector<int> &p_originalVars);

        // Stringify the expression
        virtual std::string to_string();
        int index;
}; // EVar

/***********************************/
/*********  Equivalence   **********/
/***********************************/

class EEqu : public Expr
{
    public:
        EEqu(Expr * e1, Expr * e2);

        // Tseitin transformation of this expression
        virtual Expr* tseitin(int &p_maxIndex, std::vector<Expr*> &p_exps, std::map<std::pair<int, int>, Expr*> &corresp, std::map<std::pair<int, int>, Expr*> &ncorresp);
    
        // Return the variable indexes (in-out parameter) of the expression
        virtual void getVars(std::vector<int> &p_originalVars);

        // Stringify the expression
        virtual std::string to_string();
        Expr * op1, * op2;
}; // EEqu

/***********************************/
/**********  Implication   *********/
/***********************************/

class EImp : public Expr
{
    public:
        EImp(Expr * e1, Expr * e2);

        // Tseitin transformation of this expression
        virtual Expr* tseitin(int &p_maxIndex, std::vector<Expr*> &p_exps, std::map<std::pair<int, int>, Expr*> &corresp, std::map<std::pair<int, int>, Expr*> &ncorresp);
    
        // Return the variable indexes (in-out parameter) of the expression
        virtual void getVars(std::vector<int> &p_originalVars);

        // Stringify the expression
        virtual std::string to_string();

        Expr * op1, * op2;
}; // EImp

/***********************************/
/*********  Exclusive Or   *********/
/***********************************/

class EXor : public Expr
{
    public:
        EXor(Expr * e1, Expr * e2);

        // Tseitin transformation of this expression
        virtual Expr* tseitin(int &p_maxIndex, std::vector<Expr*> &p_exps, std::map<std::pair<int, int>, Expr*> &corresp, std::map<std::pair<int, int>, Expr*> &ncorresp);
    
        // Return the variable indexes (in-out parameter) of the expression
        virtual void getVars(std::vector<int> &p_originalVars);

        // Stringify the expression
        virtual std::string to_string();

        Expr * op1, * op2;
}; // EXor

/***********************************/
/**********  Disjunction   *********/
/***********************************/

class EDis : public Expr
{
    public:
        EDis(Expr * e1, Expr * e2);

        // Tseitin transformation of this expression
        virtual Expr* tseitin(int &p_maxIndex, std::vector<Expr*> &p_exps, std::map<std::pair<int, int>, Expr*> &corresp, std::map<std::pair<int, int>, Expr*> &ncorresp);
    
        // Return the variable indexes (in-out parameter) of the expression
        virtual void getVars(std::vector<int> &p_originalVars);

        // Stringify the expression
        virtual std::string to_string();

        Expr * op1, * op2;
}; // EDis

/***********************************/
/**********  Conjunction   *********/
/***********************************/

class ECon : public Expr
{
    public:
        ECon(Expr * e1, Expr * e2);

        // Tseitin transformation of this expression
        virtual Expr* tseitin(int &p_maxIndex, std::vector<Expr*> &p_exps, std::map<std::pair<int, int>, Expr*> &corresp, std::map<std::pair<int, int>, Expr*> &ncorresp);
    
        // Return the variable indexes (in-out parameter) of the expression
        virtual void getVars(std::vector<int> &p_originalVars);

        // Stringify the expression
        virtual std::string to_string();

        Expr * op1, * op2;
}; // ECon

/***********************************/
/**********  Antithesis   **********/
/***********************************/

class EAnt : public Expr
{
    public:
        EAnt(Expr * e1);

        // Tseitin transformation of this expression
        virtual Expr* tseitin(int &p_maxIndex, std::vector<Expr*> &p_exps, std::map<std::pair<int, int>, Expr*> &corresp, std::map<std::pair<int, int>, Expr*> &ncorresp);
    
        // Return the variable indexes (in-out parameter) of the expression
        virtual void getVars(std::vector<int> &p_originalVars);

        // Stringify the expression
        virtual std::string to_string();

        Expr * op1;
}; // EAnt

/***********************************/
/**********  Negation   ************/
/***********************************/

class ENeg : public Expr
{
    public:
        ENeg(Expr * e1);

        // Tseitin transformation of this expression
        virtual Expr* tseitin(int &p_maxIndex, std::vector<Expr*> &p_exps, std::map<std::pair<int, int>, Expr*> &corresp, std::map<std::pair<int, int>, Expr*> &ncorresp);
    
        // Return the variable indexes (in-out parameter) of the expression
        virtual void getVars(std::vector<int> &p_originalVars);

        // Stringify the expression
        virtual std::string to_string();

        Expr * op1;
}; // ENeg

/***********************************/
/**********  Equality   ************/
/***********************************/

class EEqua : public Expr
{
    public:
        EEqua(Expr * e1, Expr * e2);

        // Tseitin transformation of this expression
        virtual Expr* tseitin(int &p_maxIndex, std::vector<Expr*> &p_exps, std::map<std::pair<int, int>, Expr*> &corresp, std::map<std::pair<int, int>, Expr*> &ncorresp);
    
        // Return the variable indexes (in-out parameter) of the expression
        virtual void getVars(std::vector<int> &p_originalVars);

        // Stringify the expression
        virtual std::string to_string();

        Expr * op1, * op2;
}; // EEQua

/***********************************/
/********  Non Equality   **********/
/***********************************/

class ENEqua : public Expr
{
    public:
        ENEqua(Expr * e1, Expr * e2);

        // Tseitin transformation of this expression
        virtual Expr* tseitin(int &p_maxIndex, std::vector<Expr*> &p_exps, std::map<std::pair<int, int>, Expr*> &corresp, std::map<std::pair<int, int>, Expr*> &ncorresp);
    
        // Return the variable indexes (in-out parameter) of the expression
        virtual void getVars(std::vector<int> &p_originalVars);

        // Stringify the expression
        virtual std::string to_string();

        Expr * op1, * op2;
}; // EEQua

#endif // DEF_EXPR_HPP

