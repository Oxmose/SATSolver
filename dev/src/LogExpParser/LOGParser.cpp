/*
 *
 *  CLASS LOGParser
 *
*/

// STD INCLUDES
#include <fstream>  // std::ifstream
#include <vector>   // std::vector
#include <string>   // std::string
#include <map>      // std::mapa
#include <sstream>  // std::stringstream
#include <iostream> // std::cout std::cerr std::endl

// CLASS HEADER INCLUDE
#include "LOGParser.h"

// OTHER INCLUDES FROM PROJECT
#include "../Core/Clause.h" // Clause class

// PARSER INCLUDES
#include "expr.hpp"
#include "expr.tab.hpp"

using namespace std;

LOGParser::LOGParser(const string &p_fileName)
{
    m_fileName = p_fileName;
} // LOGParser(const string&)

LOGParser::~LOGParser()
{
} // ~LOGParser()

bool LOGParser::parse(unsigned int &p_maxIndex, ClauseSet& p_formula)
{
    // Open file
    yyin = fopen(m_fileName.c_str(), "r");
    if(yyin == NULL)
    {
        cerr << "Can't open file." << endl;
        return false;
    }

    bool noParseError = true;

    /*
    ** Log expression format parse
    */

    do
    {
        yy_flex_debug = 1;
        yyparse();
        cout << res->to_string() << endl;
    } while (!feof(yyin));


    fclose(yyin);
    return noParseError;
} // bool parse(unsigned int &, vector<Clause>&)

void yyerror(const char *s)
{
    cout << "Parse error!  Message: " << s << endl;
} // yyerror(const char*)
