/*
 *
 *  CLASS CNFParser
 *
*/

#ifndef DEF_CNFPARSER_H
#define DEF_CNFPARSER_H

// STD INCLUDES
#include <fstream>  // std::ifstream
#include <vector>   // std::vector
#include <string>   // std::string
#include <set>      // std::set
#include <sstream>  // std::stringstream
#include <iostream> // std::cout std::cerr std::endl

// OTHER INCLUDES FROM PROJECT
#include "../Core/Clause.h" // Clause class

// GLOBAL FLAGS/VARS
#include "../Global/Global.h"

class CNFParser
{
    public:
        // Constructor / Destructor
        CNFParser(const std::string &p_fileName);
        ~CNFParser();
        
        // Parse
        bool parse(unsigned int &p_maxIndex, std::vector<Clause>& p_clauses);

    private:
        // Name of the file to parse
        std::string m_fileName;
	
}; // Parser

#endif // DEF_CNFPARSER_H
