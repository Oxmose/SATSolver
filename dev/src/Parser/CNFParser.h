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
#include "../NewCore/SATSolver.h" // SATSolver class
#include "../NewCore/clause.h" // Clause class

// GLOBAL FLAGS/VARS
#include "../Global/Global.h"

// INHERITANCE INCLUDE
#include "IParser.h"

class CNFParser : public IParser
{
    public:
        // Constructor / Destructor
        CNFParser(const std::string &p_fileName);
        ~CNFParser();
        
        // Parse
        bool parse(SATSolver &p_solver, unsigned int &p_maxIndex);
        bool tseitinResolution(std::map<unsigned int,int> &p_valuation, unsigned int &p_maxIndex);

    private:
        // Name of the file to parse
        std::string m_fileName;
	
}; // CNFParser

#endif // DEF_CNFPARSER_H
