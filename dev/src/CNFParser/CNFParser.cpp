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
#include "../Core/Clause.h" // Clause class

using namespace std;

CNFParser::CNFParser(const string &p_fileName)
{
    m_fileName = p_fileName;
} // CNFParser(const string&)

CNFParser::~CNFParser()
{
} // ~CNFParser()

bool CNFParser::parse(unsigned int &p_maxIndex, ClauseSet& p_formula)
{
    // Open file
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
    bool parsedHeader = false;
    string line;

    // Verification vars
    set<int> vars;
    unsigned int clausesCount = 0;
    unsigned int maxIndex = 0;
    unsigned int givenClausesCount = 0;

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
        if(line[firstChar] == 'c' && line[firstChar] == ' ')
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
                return noError;
            }

            // Retrive formula metadata into members
            splitter >> p_maxIndex;
            maxIndex = p_maxIndex;
            splitter >> givenClausesCount;
        }
        else
        {
            // Parse clauses

            //Create new clause
            stringstream splitter(line);

            vector<literal> literals;
            int readLiteral;
            bool hasTot = false;
            while(splitter >> readLiteral)
            {
                if(readLiteral == 0)
                    break;

                bool found = false;

                // Avoid mutiple same literals in the same clause
                // Also check for tautology
                for(literal lit : literals)
                {
                    if((lit.bar && -(lit.index) == readLiteral) || (!lit.bar && lit.index == readLiteral))
                        found = true;
                    else if((!lit.bar && -(lit.index) == readLiteral) || (lit.bar && lit.index == readLiteral))
                    {
                        hasTot = true;
                    }
                }


                if(!found)
                    literals.push_back(literal_from_int(readLiteral));

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
                if(readLiteral > p_maxIndex)
                {
                    cerr << "The file has " << readLiteral << " for index variable but " << p_maxIndex << " was announced as maximum index." << endl;
                    if(readLiteral > maxIndex)
                        maxIndex = readLiteral;
                }
            }

            // Create and save the clause
            Clause clause(literals, hasTot, clausesCount);
            p_formula.insert(clause);
            p_maxIndex = maxIndex;
            // Add one to counter for verification purposes
            ++clausesCount;
        }
    }

    /*
     * Error management
    */

    if(clausesCount != givenClausesCount)
    {
        cerr << "The file has " << clausesCount << " clauses but " << givenClausesCount << " were announced." << endl;
        givenClausesCount = clausesCount;
    }

    // If some variables were not used or too much were
    if(maxIndex != vars.size())
    {
        vector<bool> used;
        for(int i = 0 ; i < maxIndex+1 ; i++)
            used.push_back(false);

        for(unsigned int i : vars)
        {
            used[i] = true;
        }

        // We check which ones were used and shouldn't have been
        for(unsigned int i = 1; i < maxIndex+1; ++i)
            if(!used[i])
                cerr << i << " was not used whereas maximum index is " << maxIndex << "." << endl;
    }

    file.close();

    return noError;
} // bool parse(unsigned int &, ClauseSet&)
