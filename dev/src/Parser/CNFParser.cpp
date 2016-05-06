/*
 *
 *  CLASS CNFParser
 *
*/

// STD INCLUDES
#include <fstream>  // std::ifstream
#include <vector>   // std::vector
#include <string>   // std::string
#include <set>      // std::set
#include <sstream>  // std::stringstream
#include <iostream> // std::cout std::cerr std::endl

// CLASS HEADER
#include "CNFParser.h"

// OTHER INCLUDES FROM PROJECT
#include "../NewCore/SATSolver.h" // SATSolver class
#include "../NewCore/clause.h" // Clause class

// GLOBAL FLAGS/VARS
#include "../Global/Global.h"

// INHERITANCE INCLUDE
#include "IParser.h"

using namespace std;

CNFParser::CNFParser(const string &p_fileName)
{
    m_fileName = p_fileName;
} // CNFParser(const string&)

CNFParser::~CNFParser()
{
} // ~CNFParser()

bool CNFParser::parse(SATSolver &p_solver, unsigned int &p_maxIndex)
{
    OUTDEBUG(fprintf(stderr, "CNF PARSE BEGIN\n"));

    // Open file
    ifstream file(m_fileName);
    if(!file.is_open())
    {
        cout << "[Error] Can't open file." << endl;
        assert(false);
    }

    bool noError = true;

    /*
    ** Simple format parse
    */
    bool parsedHeader = false;
    string line;

    // Verification vars
    set<int> vars;
    unsigned int clausesCount = 0;
    unsigned int maxIndex = 0;
    unsigned int givenClausesCount = 0;
    int defMaxIndex = 0;

    // Read line by line
    while(getline(file, line))
    {
        // Remove possible spaces
        unsigned int firstChar = 0;
        for(unsigned int i = 0; line[i] == ' ' && i < line.size(); ++i)
            firstChar = i + 1;

        if(firstChar == line.size())
            continue;

        // We sound not to parse comment line
        if(line[firstChar] == 'c' || line[firstChar] == ' ')
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
                cout << "[Error] Not a CNF formula or header is corrupted." << endl;
                assert(false);
            }

            // Retrive formula metadata into members
            splitter >> defMaxIndex;
            maxIndex = defMaxIndex;
            splitter >> givenClausesCount;
        }
        else
        {

            //Create new clause
            stringstream splitter(line);

            clause the_clause(p_solver.formula.size());

            map<int,bool> literals;
            int readLiteral;
            bool hasTaut = false;
            while(splitter >> readLiteral)
            {
                if(readLiteral == 0)
                    break;

                bool found = false;

                // Avoid mutiple same literals in the same clause
                // Also check for tautology
                for(auto lit : literals)
                {
                    //lit.first = index, lit.second = isBar?
                    if(lit.first == readLiteral)
		    {
                        found = true;
                    }
                    else if(lit.first == -readLiteral)
                    {
                        hasTaut = true;
                        p_solver.valuation[abs(readLiteral)] = -1;
                    }
                }

                if(!found)
                {
                    the_clause.literal.push_back(readLiteral);
                    literals[readLiteral] = (readLiteral > 0);
                }

                // Vars count verification
                if(readLiteral < 0)
                {
                    if(vars.find(-readLiteral) == vars.end())
                    {
                        vars.insert(-readLiteral);
                    }
                }
                else
                {
                    if(vars.find(readLiteral) == vars.end())
                    {
                        vars.insert(readLiteral);
                    }
                }

                if(readLiteral < 0)
                {
                    readLiteral = -readLiteral;
                }
                if(readLiteral > (int)defMaxIndex)
                {
                    OUTWARNING("The file has " << readLiteral << " for index variable but " << defMaxIndex << " was announced as maximum index.");
                    if(readLiteral > (int)maxIndex)
                        maxIndex = readLiteral;
                }
            }

            // Create and save the clause
            if(literals.size() > 0)
            {
                if(!hasTaut)
                {
                    p_solver.add_clause(the_clause, true);
                    // Add one to counter for verification purposes                                    
                }
                else
                    OUTDEBUG(fprintf(stderr,"%s is tautological, not added.\n", the_clause.to_str().c_str()));
                defMaxIndex = maxIndex;
		++clausesCount;    
            }
            else
            {
                OUTWARNING("Empty clause detected.");
                cout << "s UNSATISFIABLE" << endl;
                exit(EXIT_SUCCESS);
            }
        }
    }

    if(!parsedHeader)
    {
        cout << "[ERROR] Not a CNF file/formula." << endl;
        assert(false);        
    }
    /*
     * Error management
    */
    if(clausesCount != givenClausesCount)
    {
        OUTWARNING("The file has " << clausesCount << " clauses but " << givenClausesCount << " were announced.");
        givenClausesCount = clausesCount;
	    noError = false;
    }

    // If some variables were not used or too much were
    if(maxIndex != vars.size())
    {
        vector<bool> used;
        for(unsigned int i = 0 ; i < maxIndex + 1 ; i++)
            used.push_back(false);

        for(unsigned int i : vars)
        {
            used[i] = true;
        }

        // We check which ones were used and shouldn't have been
        for(unsigned int i = 1; i < maxIndex + 1; ++i)
            if(!used[i])
            {
                noError = false;
                OUTWARNING(i << " was not used whereas maximum index is " << maxIndex << ".");
            }
    }

    file.close();
    OUTDEBUG(fprintf(stderr, "CNF PARSE END WITH STATUS \n"));
    return noError;
} // bool parse(unsigned int &, ClauseSet&)

bool CNFParser::tseitinResolution(map<unsigned int,int> &p_valuation, unsigned int &defMaxIndex)
{
    return true;
} // bool tseitinResolution(map<int,int>&, unsigned int&);
