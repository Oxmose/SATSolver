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

// OTHER INCLUDES FROM PROJECT
#include "../Core/Clause.h" // Clause class

// PARSER INCLUDES
#include "expr.hpp"
#include "expr.tab.hpp"

// GLOBAL FLAGS/VARS
#include "../Global/Global.h"

// extern "C" int yyparse();
extern "C" FILE *yyin;
extern Expr *res;
extern int yy_flex_debug;

// Function that stop the programm on error (kind of excpetion)
void yyerror(const char *s);

class LOGParser
{
    public:
        // Constructor / Destructor
        LOGParser(const std::string &p_fileName = "");
        ~LOGParser();

        // Setters
        void setFileName(const std::string &p_fileName);

        // Parser methods
        bool parse(unsigned int &p_maxIndex, std::vector<Clause>& p_clauses);
	std::vector<Expr*> tseitinTransform(Expr *exp, unsigned int &p_maxIndex);
        bool tseitinResolution(std::map<int,int> &p_valuation, unsigned int &p_maxIndex);

    private:
        // Name of the file to parse
        std::string m_fileName;

	// Vars contained in the original formula
        std::vector<int> m_originalVars;
}; // Parser

#endif // DEF_LOGPARSER_H
