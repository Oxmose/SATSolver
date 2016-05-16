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

#include "../NewCore/SMT/SMTSolver_eq.h"

class Expr;


struct args_s
{
    std::string name;
    unsigned int count;
    int assoc_var;
    Expr *ptr;
};
typedef struct args_s args_s;

struct function_s : args_s
{
    std::string name;
    unsigned int arrity;
    int assoc_var;
    Expr *ptr;
};
typedef struct function_s function_s;

enum ineq_type
{
    INEQUAL   = 0,
    EQUAL     = 1,
    NOT_EQUAL = 2
};
struct ineq_s
{
    std::string name;
    std::string old_name;
    int left;
    int right;
    int coeff;
    int assoc_var;
    ineq_type type;
    Expr *ptr;
};
typedef struct ineq_s ineq_s;

/***********************************/
/*********  Expressions   **********/
/***********************************/
class EVar;

class Expr
{
    public:
        Expr(){}

        virtual Expr* tseitin(int &p_maxIndex, std::vector<Expr*> &p_exps, 
                              std::map<std::pair<int, int>, Expr*> &corresp, std::map<std::pair<int, int>, Expr*> &ncorresp, 
                              std::map<std::string, function_s*> &funcoresp, std::map<std::string, unsigned int> &arrities,
                              std::map<std::string, args_s*> &argscorresp, bool trans, struct smt_term *root,
			      std::map<string, ineq_s*> &str_to_ineq, std::map<int, ineq_s*> &int_to_ineq)=0;
        virtual void getVars(std::vector<int> &p_originalVars)=0;
        virtual std::string to_string()=0;
        virtual int getVarTerm()=0;
        virtual unsigned int size()=0;
    
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
        virtual Expr* tseitin(int &p_maxIndex, std::vector<Expr*> &p_exps, 
                              std::map<std::pair<int, int>, Expr*> &corresp, std::map<std::pair<int, int>, Expr*> &ncorresp, 
                              std::map<std::string, function_s*> &funcoresp, std::map<std::string, unsigned int> &arrities,
                              std::map<std::string, args_s*> &argscorresp, bool trans, struct smt_term *root,
			      std::map<string, ineq_s*> &str_to_ineq, std::map<int, ineq_s*> &int_to_ineq);
    
        // Return the variable indexes (in-out parameter) of the expression
        virtual void getVars(std::vector<int> &p_originalVars);

        // Stringify the expression
        virtual std::string to_string();

        // Get var assiociated to the term
        virtual int getVarTerm();

        // Get the number or arguments
        virtual unsigned int size();

        int index;
}; // EVar

/***********************************/
/******  Logival Variables   *******/
/***********************************/

class ELoVar : public Expr
{
    public:
        ELoVar(char name[256]); 

        // Tseitin transformation of this expression
        virtual Expr* tseitin(int &p_maxIndex, std::vector<Expr*> &p_exps, 
                              std::map<std::pair<int, int>, Expr*> &corresp, std::map<std::pair<int, int>, Expr*> &ncorresp, 
                              std::map<std::string, function_s*> &funcoresp, std::map<std::string, unsigned int> &arrities,
                              std::map<std::string, args_s*> &argscorresp, bool trans, struct smt_term *root,
			      std::map<string, ineq_s*> &str_to_ineq, std::map<int, ineq_s*> &int_to_ineq);
    
        // Return the variable indexes (in-out parameter) of the expression
        virtual void getVars(std::vector<int> &p_originalVars);

        // Stringify the expression
        virtual std::string to_string();

        // Get var assiociated to the term
        virtual int getVarTerm();

        // Get the number or arguments
        virtual unsigned int size();

        int index;
}; // ELoVar

/***********************************/
/**********  Constant    ***********/
/***********************************/

class ECst : public Expr
{
    public:
        ECst(char name[256]); 

        // Tseitin transformation of this expression
        virtual Expr* tseitin(int &p_maxIndex, std::vector<Expr*> &p_exps, 
                              std::map<std::pair<int, int>, Expr*> &corresp, std::map<std::pair<int, int>, Expr*> &ncorresp, 
                              std::map<std::string, function_s*> &funcoresp, std::map<std::string, unsigned int> &arrities,
                              std::map<std::string, args_s*> &argscorresp, bool trans, struct smt_term *root,
			      std::map<string, ineq_s*> &str_to_ineq, std::map<int, ineq_s*> &int_to_ineq);
    
        // Return the variable indexes (in-out parameter) of the expression
        virtual void getVars(std::vector<int> &p_originalVars);

        // Stringify the expression
        virtual std::string to_string();

        // Get var assiociated to the term
        virtual int getVarTerm();

        // Get the number or arguments
        virtual unsigned int size();

        std::string value;
	    int index;
}; // EVar

/***********************************/
/**********  Functions   ***********/
/***********************************/
class EFun : public Expr
{
    public:
        EFun(char name[256], Expr* e); 

        // Tseitin transformation of this expression
        virtual Expr* tseitin(int &p_maxIndex, std::vector<Expr*> &p_exps, 
                              std::map<std::pair<int, int>, Expr*> &corresp, std::map<std::pair<int, int>, Expr*> &ncorresp, 
                              std::map<std::string, function_s*> &funcoresp, std::map<std::string, unsigned int> &arrities,
                              std::map<std::string, args_s*> &argscorresp, bool trans, struct smt_term *root,
			                  std::map<string, ineq_s*> &str_to_ineq, std::map<int, ineq_s*> &int_to_ineq);
    
        // Return the variable indexes (in-out parameter) of the expression
        virtual void getVars(std::vector<int> &p_originalVars);

        // Stringify the expression
        virtual std::string to_string();

        // Get var assiociated to the term
        virtual int getVarTerm();

        // Get the number or arguments
        virtual unsigned int size();
        
        std::string m_name;
        Expr* m_args;
}; // EFun

/***********************************/
/**********  Arguments   ***********/
/***********************************/

class EArg : public Expr
{
    public:
        EArg(Expr* e1, Expr* e2); 

        // Tseitin transformation of this expression
        virtual Expr* tseitin(int &p_maxIndex, std::vector<Expr*> &p_exps, 
                              std::map<std::pair<int, int>, Expr*> &corresp, std::map<std::pair<int, int>, Expr*> &ncorresp, 
                              std::map<std::string, function_s*> &funcoresp, std::map<std::string, unsigned int> &arrities,
                              std::map<std::string, args_s*> &argscorresp, bool trans, struct smt_term *root,
			      std::map<string, ineq_s*> &str_to_ineq, std::map<int, ineq_s*> &int_to_ineq);
    
        // Return the variable indexes (in-out parameter) of the expression
        virtual void getVars(std::vector<int> &p_originalVars);

        // Stringify the expression
        virtual std::string to_string();

        // Get var assiociated to the term
        virtual int getVarTerm();

        // Get the number or arguments
        virtual unsigned int size();
        
        Expr * op1, * op2;
}; // EArg

/***********************************/
/*********  Equivalence   **********/
/***********************************/

class EEqu : public Expr
{
    public:
        EEqu(Expr * e1, Expr * e2);

        // Tseitin transformation of this expression
        virtual Expr* tseitin(int &p_maxIndex, std::vector<Expr*> &p_exps, 
                              std::map<std::pair<int, int>, Expr*> &corresp, std::map<std::pair<int, int>, Expr*> &ncorresp, 
                              std::map<std::string, function_s*> &funcoresp, std::map<std::string, unsigned int> &arrities,
                              std::map<std::string, args_s*> &argscorresp, bool trans, struct smt_term *root,
			      std::map<string, ineq_s*> &str_to_ineq, std::map<int, ineq_s*> &int_to_ineq);
    
        // Return the variable indexes (in-out parameter) of the expression
        virtual void getVars(std::vector<int> &p_originalVars);

        // Stringify the expression
        virtual std::string to_string();

        // Get var assiociated to the term
        virtual int getVarTerm();

        // Get the number or arguments
        virtual unsigned int size();

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
        virtual Expr* tseitin(int &p_maxIndex, std::vector<Expr*> &p_exps, 
                              std::map<std::pair<int, int>, Expr*> &corresp, std::map<std::pair<int, int>, Expr*> &ncorresp, 
                              std::map<std::string, function_s*> &funcoresp, std::map<std::string, unsigned int> &arrities,
                              std::map<std::string, args_s*> &argscorresp, bool trans, struct smt_term *root,
			      std::map<string, ineq_s*> &str_to_ineq, std::map<int, ineq_s*> &int_to_ineq);
    
        // Return the variable indexes (in-out parameter) of the expression
        virtual void getVars(std::vector<int> &p_originalVars);

        // Stringify the expression
        virtual std::string to_string();

        // Get var assiociated to the term
        virtual int getVarTerm();

        // Get the number or arguments
        virtual unsigned int size();

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
        virtual Expr* tseitin(int &p_maxIndex, std::vector<Expr*> &p_exps, 
                              std::map<std::pair<int, int>, Expr*> &corresp, std::map<std::pair<int, int>, Expr*> &ncorresp, 
                              std::map<std::string, function_s*> &funcoresp, std::map<std::string, unsigned int> &arrities,
                              std::map<std::string, args_s*> &argscorresp, bool trans, struct smt_term *root,
			      std::map<string, ineq_s*> &str_to_ineq, std::map<int, ineq_s*> &int_to_ineq);
    
        // Return the variable indexes (in-out parameter) of the expression
        virtual void getVars(std::vector<int> &p_originalVars);

        // Stringify the expression
        virtual std::string to_string();

        // Get var assiociated to the term
        virtual int getVarTerm();

        // Get the number or arguments
        virtual unsigned int size();

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
        virtual Expr* tseitin(int &p_maxIndex, std::vector<Expr*> &p_exps, 
                              std::map<std::pair<int, int>, Expr*> &corresp, std::map<std::pair<int, int>, Expr*> &ncorresp, 
                              std::map<std::string, function_s*> &funcoresp, std::map<std::string, unsigned int> &arrities,
                              std::map<std::string, args_s*> &argscorresp, bool trans, struct smt_term *root,
			      std::map<string, ineq_s*> &str_to_ineq, std::map<int, ineq_s*> &int_to_ineq);
    
        // Return the variable indexes (in-out parameter) of the expression
        virtual void getVars(std::vector<int> &p_originalVars);

        // Stringify the expression
        virtual std::string to_string();

        // Get var assiociated to the term
        virtual int getVarTerm();

        // Get the number or arguments
        virtual unsigned int size();

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
        virtual Expr* tseitin(int &p_maxIndex, std::vector<Expr*> &p_exps, 
                              std::map<std::pair<int, int>, Expr*> &corresp, std::map<std::pair<int, int>, Expr*> &ncorresp, 
                              std::map<std::string, function_s*> &funcoresp, std::map<std::string, unsigned int> &arrities,
                              std::map<std::string, args_s*> &argscorresp, bool trans, struct smt_term *root,
			      std::map<string, ineq_s*> &str_to_ineq, std::map<int, ineq_s*> &int_to_ineq);
    
        // Return the variable indexes (in-out parameter) of the expression
        virtual void getVars(std::vector<int> &p_originalVars);

        // Stringify the expression
        virtual std::string to_string();

        // Get var assiociated to the term
        virtual int getVarTerm();

        // Get the number or arguments
        virtual unsigned int size();

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
        virtual Expr* tseitin(int &p_maxIndex, std::vector<Expr*> &p_exps, 
                              std::map<std::pair<int, int>, Expr*> &corresp, std::map<std::pair<int, int>, Expr*> &ncorresp, 
                              std::map<std::string, function_s*> &funcoresp, std::map<std::string, unsigned int> &arrities,
                              std::map<std::string, args_s*> &argscorresp, bool trans, struct smt_term *root,
			      std::map<string, ineq_s*> &str_to_ineq, std::map<int, ineq_s*> &int_to_ineq);
    
        // Return the variable indexes (in-out parameter) of the expression
        virtual void getVars(std::vector<int> &p_originalVars);

        // Stringify the expression
        virtual std::string to_string();

        // Get var assiociated to the term
        virtual int getVarTerm();

        // Get the number or arguments
        virtual unsigned int size();

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
        virtual Expr* tseitin(int &p_maxIndex, std::vector<Expr*> &p_exps, 
                              std::map<std::pair<int, int>, Expr*> &corresp, std::map<std::pair<int, int>, Expr*> &ncorresp, 
                              std::map<std::string, function_s*> &funcoresp, std::map<std::string, unsigned int> &arrities,
                              std::map<std::string, args_s*> &argscorresp, bool trans, struct smt_term *root,
			      std::map<string, ineq_s*> &str_to_ineq, std::map<int, ineq_s*> &int_to_ineq);
    
        // Return the variable indexes (in-out parameter) of the expression
        virtual void getVars(std::vector<int> &p_originalVars);

        // Stringify the expression
        virtual std::string to_string();

        // Get var assiociated to the term
        virtual int getVarTerm();

        // Get the number or arguments
        virtual unsigned int size();

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
        virtual Expr* tseitin(int &p_maxIndex, std::vector<Expr*> &p_exps, 
                              std::map<std::pair<int, int>, Expr*> &corresp, std::map<std::pair<int, int>, Expr*> &ncorresp, 
                              std::map<std::string, function_s*> &funcoresp, std::map<std::string, unsigned int> &arrities,
                              std::map<std::string, args_s*> &argscorresp, bool trans, struct smt_term *root,
			      std::map<string, ineq_s*> &str_to_ineq, std::map<int, ineq_s*> &int_to_ineq);
    
        // Return the variable indexes (in-out parameter) of the expression
        virtual void getVars(std::vector<int> &p_originalVars);

        // Stringify the expression
        virtual std::string to_string();

        // Get var assiociated to the term
        virtual int getVarTerm();

        // Get the number or arguments
        virtual unsigned int size();

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
        virtual Expr* tseitin(int &p_maxIndex, std::vector<Expr*> &p_exps, 
                              std::map<std::pair<int, int>, Expr*> &corresp, std::map<std::pair<int, int>, Expr*> &ncorresp, 
                              std::map<std::string, function_s*> &funcoresp, std::map<std::string, unsigned int> &arrities,
                              std::map<std::string, args_s*> &argscorresp, bool trans, struct smt_term *root,
			      std::map<string, ineq_s*> &str_to_ineq, std::map<int, ineq_s*> &int_to_ineq);
    
        // Return the variable indexes (in-out parameter) of the expression
        virtual void getVars(std::vector<int> &p_originalVars);

        // Stringify the expression
        virtual std::string to_string();

        // Get var assiociated to the term
        virtual int getVarTerm();

        // Get the number or arguments
        virtual unsigned int size();

        Expr * op1, * op2;
}; // EEQua

/***********************************/
/********  LESS THAN      **********/
/***********************************/

class ELth : public Expr
{
    public:
        ELth(Expr * e1, Expr * e2, Expr * e3 = nullptr);

        // Tseitin transformation of this expression
        virtual Expr* tseitin(int &p_maxIndex, std::vector<Expr*> &p_exps, 
                              std::map<std::pair<int, int>, Expr*> &corresp, std::map<std::pair<int, int>, Expr*> &ncorresp, 
                              std::map<std::string, function_s*> &funcoresp, std::map<std::string, unsigned int> &arrities,
                              std::map<std::string, args_s*> &argscorresp, bool trans, struct smt_term *root,
			      std::map<string, ineq_s*> &str_to_ineq, std::map<int, ineq_s*> &int_to_ineq);
    
        // Return the variable indexes (in-out parameter) of the expression
        virtual void getVars(std::vector<int> &p_originalVars);

        // Stringify the expression
        virtual std::string to_string();

        // Get var assiociated to the term
        virtual int getVarTerm();

        // Get the number or arguments
        virtual unsigned int size();

        Expr * op1, * op2, * op3;
}; // ELth

/***********************************/
/********  GREATER THAN   **********/
/***********************************/

class EGth : public Expr
{
    public:
        EGth(Expr * e1, Expr * e2, Expr * e3 = nullptr);

        // Tseitin transformation of this expression
        virtual Expr* tseitin(int &p_maxIndex, std::vector<Expr*> &p_exps, 
                              std::map<std::pair<int, int>, Expr*> &corresp, std::map<std::pair<int, int>, Expr*> &ncorresp, 
                              std::map<std::string, function_s*> &funcoresp, std::map<std::string, unsigned int> &arrities,
                              std::map<std::string, args_s*> &argscorresp, bool trans, struct smt_term *root,
			      std::map<string, ineq_s*> &str_to_ineq, std::map<int, ineq_s*> &int_to_ineq);
    
        // Return the variable indexes (in-out parameter) of the expression
        virtual void getVars(std::vector<int> &p_originalVars);

        // Stringify the expression
        virtual std::string to_string();

        // Get var assiociated to the term
        virtual int getVarTerm();

        // Get the number or arguments
        virtual unsigned int size();

        Expr * op1, * op2, * op3;
}; // EGth

/***********************************/
/******  LESS THAN OR EQUAL ********/
/***********************************/

class ELeq : public Expr
{
    public:
        ELeq(Expr * e1, Expr * e2, Expr * e3 = nullptr);

        // Tseitin transformation of this expression
        virtual Expr* tseitin(int &p_maxIndex, std::vector<Expr*> &p_exps, 
                              std::map<std::pair<int, int>, Expr*> &corresp, std::map<std::pair<int, int>, Expr*> &ncorresp, 
                              std::map<std::string, function_s*> &funcoresp, std::map<std::string, unsigned int> &arrities,
                              std::map<std::string, args_s*> &argscorresp, bool trans, struct smt_term *root,
			      std::map<string, ineq_s*> &str_to_ineq, std::map<int, ineq_s*> &int_to_ineq);
    
        // Return the variable indexes (in-out parameter) of the expression
        virtual void getVars(std::vector<int> &p_originalVars);

        // Stringify the expression
        virtual std::string to_string();

        // Get var assiociated to the term
        virtual int getVarTerm();

        // Get the number or arguments
        virtual unsigned int size();

        Expr * op1, * op2, * op3;
}; // ELeq

/***********************************/
/****  GREATER THAN  OR EQUAL  *****/
/***********************************/

class EGeq : public Expr
{
    public:
        EGeq(Expr * e1, Expr * e2, Expr * e3 = nullptr);

        // Tseitin transformation of this expression
        virtual Expr* tseitin(int &p_maxIndex, std::vector<Expr*> &p_exps, 
                              std::map<std::pair<int, int>, Expr*> &corresp, std::map<std::pair<int, int>, Expr*> &ncorresp, 
                              std::map<std::string, function_s*> &funcoresp, std::map<std::string, unsigned int> &arrities,
                              std::map<std::string, args_s*> &argscorresp, bool trans, struct smt_term *root,
			      std::map<string, ineq_s*> &str_to_ineq, std::map<int, ineq_s*> &int_to_ineq);
    
        // Return the variable indexes (in-out parameter) of the expression
        virtual void getVars(std::vector<int> &p_originalVars);

        // Stringify the expression
        virtual std::string to_string();

        // Get var assiociated to the term
        virtual int getVarTerm();

        // Get the number or arguments
        virtual unsigned int size();

        Expr * op1, * op2, * op3;
}; // EGeq */

/***********************************/
/********  LOGICAL EQUAL  **********/
/***********************************/

class ELoEq : public Expr
{
    public:
        ELoEq(Expr * e1, Expr * e2, Expr * e3 = nullptr);

        // Tseitin transformation of this expression
        virtual Expr* tseitin(int &p_maxIndex, std::vector<Expr*> &p_exps, 
                              std::map<std::pair<int, int>, Expr*> &corresp, std::map<std::pair<int, int>, Expr*> &ncorresp, 
                              std::map<std::string, function_s*> &funcoresp, std::map<std::string, unsigned int> &arrities,
                              std::map<std::string, args_s*> &argscorresp, bool trans, struct smt_term *root,
			      std::map<string, ineq_s*> &str_to_ineq, std::map<int, ineq_s*> &int_to_ineq);
    
        // Return the variable indexes (in-out parameter) of the expression
        virtual void getVars(std::vector<int> &p_originalVars);

        // Stringify the expression
        virtual std::string to_string();

        // Get var assiociated to the term
        virtual int getVarTerm();

        // Get the number or arguments
        virtual unsigned int size();

        Expr * op1, * op2, * op3;
}; // ELoEq

/***********************************/
/******  LOGICAL NON EQUAL  ********/
/***********************************/

class ELoNeq : public Expr
{
    public:
        ELoNeq(Expr * e1, Expr * e2, Expr * e3 = nullptr);

        // Tseitin transformation of this expression
        virtual Expr* tseitin(int &p_maxIndex, std::vector<Expr*> &p_exps, 
                              std::map<std::pair<int, int>, Expr*> &corresp, std::map<std::pair<int, int>, Expr*> &ncorresp, 
                              std::map<std::string, function_s*> &funcoresp, std::map<std::string, unsigned int> &arrities,
                              std::map<std::string, args_s*> &argscorresp, bool trans, struct smt_term *root,
			      std::map<string, ineq_s*> &str_to_ineq, std::map<int, ineq_s*> &int_to_ineq);
    
        // Return the variable indexes (in-out parameter) of the expression
        virtual void getVars(std::vector<int> &p_originalVars);

        // Stringify the expression
        virtual std::string to_string();

        // Get var assiociated to the term
        virtual int getVarTerm();

        // Get the number or arguments
        virtual unsigned int size();

        Expr * op1, * op2, * op3;
}; // ELoNeq


#endif // DEF_EXPR_HPP

