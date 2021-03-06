/*
 *
 *  CLASS LOGParser
 *
*/

#ifndef DEF_LOGPARSER_H
#define DEF_LOGPARSER_H

// STD INCLUDES
#include <fstream>  // std::ifstream
#include <vector>   // std::vector
#include <string>   // std::string
#include <map>      // std::map
#include <sstream>  // std::stringstream
#include <iostream> // std::cout std::cerr std::endl
#include <atomic>   // std::atomic
#include <unordered_map> // std::unordered_map

// OTHER INCLUDES FROM PROJECT
#include "../Core/SMT/SMTSolver_eq.h" // SMTSolver_eq class
#include "../Core/SATSolver.h" // SATSolver class
#include "../Core/clause.h" // Clause class

// PARSER INCLUDES
#include "expr.hpp"
#include "expr.tab.hpp"

// GLOBAL FLAGS/VARS
#include "../Global/Global.h"

// INHERITANCE INCLUDE
#include "IParser.h"

// extern "C" int yyparse();
extern "C" FILE *yyin;
extern Expr *res;
extern int yy_flex_debug;

// Function that stop the programm on error (kind of excpetion)
void yyerror(const char *s);

void dfsdeb(struct smt_term *root, std::unordered_map<int, smt_term*> &term_corresp, int tab, bool output);

class LOGParser : public IParser
{
    public:
        // Constructor / Destructor
        LOGParser(const std::string &p_fileName);
        ~LOGParser();

        // Parser methods
        bool parse(SATSolver &p_solver, unsigned int &p_maxIndex);
        bool tseitinResolution(std::map<unsigned int,int> &p_valuation, unsigned int &p_maxIndex);

    private:
	    std::vector<Expr*> tseitinTransform(Expr *exp, unsigned int &p_maxIndex, 
			                                std::map<std::pair<int, int>, Expr*> &corresp, std::map<std::pair<int, int>, Expr*> &ncorresp,  
                                            std::map<std::string, function_s*> &funcorresp, std::map<std::string, unsigned int> &arrities, 
                                            std::map<std::string, args_s*> &argscorresp, struct smt_term *root, 
                                            std::map<string, ineq_s*> &str_to_ineq, std::map<int, ineq_s*> &int_to_ineq);

        // Name of the file to parse
        std::string m_fileName;

	    // Vars contained in the original formula
        std::vector<int> m_originalVars;
}; // LOGParser

#endif // DEF_LOGPARSER_H
