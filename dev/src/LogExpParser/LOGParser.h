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

// extern "C" int yyparse();
extern "C" FILE *yyin;
extern Expr *res;
extern int yy_flex_debug;

void yyerror(const char *s);

class LOGParser
{
    public:
        // Constructor / Destructor
        LOGParser(const std::string &p_fileName);
        ~LOGParser();
        
        // Parser methods
        bool parse(unsigned int &p_maxIndex, std::vector<Clause> &p_formula);
        bool tseitinResolution(std::vector<Clause> &p_formula, std::map<int,int> m_valuation);

    private:

        // Name of the file to parse
        std::string m_fileName;
        std::atomic<bool> m_noError;
}; // Parser

#endif // DEF_LOGPARSER_H
