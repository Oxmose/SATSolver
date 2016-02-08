/*
 *
 *    CLASS SATSolver
 *
*/

#ifndef DEF_SATSOLVER_H
#define DEF_SATSOLVER_H

// STD INCLUDES
#include <string>   // sts::string
#include <vector>   // std::vector
#include <set>      // std::set
#include <sstream>  // std::stringstream
#include <iostream> // std::cout std::cerr std::endl
#include <fstream>  // std::ifstream

// PROJECT INCLUDES
#include "Clause.h"    // Clause class

class SATSolver 
{
    public:
        SATSolver(const std::string &p_fileName);
        ~SATSolver();
        
        bool parse();
        int solve();
        
        std::string getFormulaStr();

    private:
        std::string     m_fileName;
        std::string     m_strFormula;
        unsigned int    m_maxIndex;
        unsigned int    m_clausesCount;

        std::vector<Clause> m_formula;
}; // SATSolver 

#endif // DEF_SATSOLVER_H
