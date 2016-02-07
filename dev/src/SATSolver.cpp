/*
 *
 *    CLASS SATSolver
 *
*/

// STD INCLUDES
#include <string>   // sts::string
#include <vector>   // std::vector
#include <set>      // std::set
#include <sstream>  // std::stringstream
#include <iostream> // std::cout std::cerr std::endl
#include <fstream>  // std::ifstream

// CLASS HEADER INCLUDE
#include "SATSolver.h"

// PROJECT INCLUDES
#include "Clause.h"    // Clause class

using namespace std;

SATSolver::SATSolver(const string &p_fileName)
{
    m_fileName = p_fileName;
} // SATSolver(const string&)

SATSolver::~SATSolver()
{
} // ~SATSolver()

bool SATSolver::parse()
{
    ifstream file(m_fileName);
    if(!file.is_open())
    {
        cerr << "Can't open file." << endl;
        return false;
    }

    bool noError = true;
    /*
    ** Simple format parse
    */

    m_strFormula = "";
    bool parsedHeader = false;
    string line;

    // Verification vars
    set<int> vars;
    unsigned int clausesCount = 0;

    // Read line by line
    while(getline(file, line))
    {
        // Remove possible spaces      
        for(unsigned int i = 0; line[i] == ' ' && i < line.size(); ++i)
            line = line.substr(i);

        if(line.size() == 0)
            continue;

        // We sound not to parse comment line
        if(line[0] == 'c' && line[1] == ' ')
            continue;
 
        
        // Parse header if not already parsed
        if(!parsedHeader)
        {
            stringstream splitter(line);

            // Process header validation
            string validator;
            splitter >> validator;
            if(validator != "p")
            {
                continue;
            }

            parsedHeader = true;

            validator.clear();
            splitter >> validator;
            if(validator != "cnf")
            {
                noError = false;
                cerr << "Not a CNF formula or header is corrupted." << endl;
            }

            // Retrive formula metadata into members
            splitter >> m_maxIndex;
            splitter >> m_clausesCount;
        }
        else
        {
            // Parse clauses

            //Create new clause
            stringstream splitter(line);

            vector<int> literals;
            int literal;
            while(splitter >> literal)
            {
                if(literal == 0)
                    break;

                literals.push_back(literal);

                // Vars count verification
                if(literal < 0)
                {
                    if(vars.find(-literal) == vars.end())
                    {
                        vars.insert(-literal);
                    }
                }
                else
                {
                    if(vars.find(literal) == vars.end())
                    {
                        vars.insert(literal);
                    }
                }
            }

            // Create and save the clause
            Clause clause(literals);
            m_formula.push_back(clause);

            // Add one to counter for verification purposes
            ++clausesCount;

            // Create string format of the formula
            if(clausesCount != 1)
                m_strFormula += " /\\ ";

            m_strFormula += '(';

            for(unsigned int i = 0; i < literals.size(); ++i)
            {
                if(literals[i] < 0)
                    m_strFormula += "-x" + to_string(-literals[i]);
                else
                    m_strFormula += "x" + to_string(literals[i]);

                if(i < literals.size() - 1)
                    m_strFormula += " \\/ ";
            } 
            m_strFormula += ')';
        }
    }

    // Error management
    if(clausesCount != m_clausesCount)
    {        
        cerr << "The file has " << clausesCount << " clauses but " << m_clausesCount << " were announced." << endl;
        m_clausesCount = clausesCount;
    }

    if(m_maxIndex != vars.size())
    {        
        cerr << "The file has " << vars.size() << " variables but " << m_maxIndex << " were announced." << endl;
        m_maxIndex = vars.size();
    }

    file.close();

    return noError;
} // bool parse()

bool SATSolver::solve()
{
    return true;
} // bool solve()

bool SATSolver::isSolvable()
{
    for(Clause clause : m_formula)
        if(!clause.isSolvable())
            return false;

    return true;
} // bool isSolvable()

bool SATSolver::evaluate(const vector<int, bool> &p_valuation)
{
    return true;
} // bool evaluation(const vecto<int, bool>&)

vector<bool> SATSolver::getValuation()
{
    vector<bool> result;

    return result;
} // vector<bool> getValuation()

string SATSolver::getFormulaStr()
{
    return m_strFormula;
} // string getFormulaStr()
