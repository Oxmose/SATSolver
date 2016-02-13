/*
 *
 *  CLASS Parser
 *
*/

#ifndef DEF_PARSER_H
#define DEF_PARSER_H

// STD INCLUDES
#include <fstream>  // std::ifstream
#include <vector>   // std::vector
#include <string>   // std::string
#include <set>      // std::set
#include <sstream>  // std::stringstream
#include <iostream> // std::cout std::cerr std::endl

// OTHER INCLUDES FROM PROJECT
#include "Clause.h"

class Parser
{
    public:
        Parser(const std::string &p_fileName);
        ~Parser();

        bool parse(unsigned int &p_maxIndex, ClauseSet& p_formula);

    private:
        std::string m_fileName;
}; // Parser

#endif // DEF_PARSER_H
