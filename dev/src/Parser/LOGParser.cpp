/*
 *
 *  CLASS LOGParser
 *
*/

// STD INCLUDES
#include <fstream>  // std::ifstream
#include <vector>   // std::vector
#include <string>   // std::string
#include <map>      // std::map
#include <sstream>  // std::stringstream
#include <iostream> // std::cerr std::endl

// CLASS HEADER INCLUDE
#include "LOGParser.h"

// OTHER INCLUDES FROM PROJECT
#include "../Core/Clause.h" // Clause class

// PARSER INCLUDES
#include "expr.hpp"
#include "expr.tab.hpp"

// GLOBAL FLAGS/VARS
#include "../Global/Global.h"

// INHERITANCE INCLUDE
#include "IParser.h"

using namespace std;

LOGParser::LOGParser(const string &p_fileName /* = ""  */)
{
    m_fileName = p_fileName;
} // LOGParser(const string&)

LOGParser::~LOGParser()
{
} // ~LOGParser()

bool LOGParser::parse(unsigned int &p_maxIndex, std::vector<Clause>& p_clauses)
{
    OUTDEBUG("LOG PARSE BEGIN");
    // Open file
    yyin = fopen(m_fileName.c_str(), "r");
    if(yyin == NULL)
    {
        OUTERROR("Can't open file.");
    }

    bool noParseError = true;

    /*
    ** Log expression format parse
    */

    vector<Expr*> exps;
    do 
    {
        yy_flex_debug = 1;
        yyparse();
        exps = tseitinTransform(res, p_maxIndex);        
    } while (!feof(yyin));   

    fclose(yyin);

    vector<pair<map<int,bool>, bool>> clauses;

    // Browse each clause extracted and create the formula
    map<int,bool> clause;
    bool hasTot = false;
    for(Expr* exp : exps)
    {
        string strForm = exp->to_string();
        for(unsigned int i = 0; i < strForm.size(); ++i)
        {
            // Check for parenthesis
            if(strForm[i] == '(') continue;

            // Check if var is negative
            if(strForm[i] == '-')
            {
                string var("");
                while(i < strForm.size() && isdigit(strForm[++i]))
                {
                    var += strForm[i];
                }

                int varInt = -stoi(var);

                // Avoid mutiple same literals in the same clause
                // Also check for tautology
                bool found = false;
                for(auto lit : clause)
                {
                    if((lit.second && -(lit.first) == varInt) || (!lit.second && lit.first == varInt))
                        found = true;           
                    else if((!lit.second && -(lit.first) == varInt) || (lit.second && lit.first == varInt))
                        hasTot = true;
                }

                // Add the variable
                if(!found)
                    clause[abs(varInt)] = varInt < 0;

                continue;
            }

            // If we are at the end of a clause
            if(i < strForm.size() -1 && strForm[i] == '/' && strForm[i + 1] == '\\')
            {
                // Create and add the new clause
                clauses.push_back(make_pair(clause, hasTot));
                hasTot = false;
                clause.clear();
                continue;
            }

            string var("");
            while(i < strForm.size() && isdigit(strForm[i]))
            {
                var += strForm[i];
                ++i;
            }
            if(var.size() > 0)
            {
                int varInt = stoi(var);
                // Avoid mutiple same literals in the same clause
                // Also check for tautology
                bool found = false;
                for(auto lit : clause)
                {
                    if((lit.second && -(lit.first) == varInt) || (!lit.second && lit.first == varInt))
                        found = true;           
                    else if((!lit.second && -(lit.first) == varInt) || (lit.second && lit.first == varInt))
                        hasTot = true;
                }

                if(!found)
                    clause[abs(varInt)] = varInt < 0;
            }
        }

        // Create and add the last clause
        clauses.push_back(make_pair(clause, hasTot));
        hasTot = false;
        clause.clear();
    }

    unsigned int clausesCount = 0;
    
    for(auto clause : clauses)
    {
        p_clauses.push_back(Clause(clause.first, clause.second, clausesCount));
	   ++clausesCount;
    }

    OUTDEBUG("LOG PARSE END WITH STATUS" << noParseError);

    return noParseError;
} // bool parse(unsigned int &, vector<Clause>&)

vector<Expr*> LOGParser::tseitinTransform(Expr *exp, unsigned int &p_maxIndex)
{
    // Get the set af vars in the expression
    res->getVars(m_originalVars);

    // Get the greatest index used
    int maxIndex = 0;
    for(unsigned int i = 0; i < m_originalVars.size(); ++i)
        if(maxIndex < m_originalVars[i])
            maxIndex = m_originalVars[i];

    vector<Expr*> exps;
    // Transform the expression
    Expr* global = res->tseitin(maxIndex, exps);
    exps.push_back(global);
    p_maxIndex = maxIndex;

    return exps;
} // bool tseitinTransform(Expr*, unsigned int&, std::vector<Clause>&)

bool LOGParser::tseitinResolution(map<int,int> &p_valuation, unsigned int &p_maxIndex)
{
    // For each vars that appeared in the original formula
    // Just keep them and forget about the others
    map<int, int> newValuation;
    int maxIndex = 0;
    for(int var : m_originalVars)
    {
        if(var > maxIndex)
            maxIndex = var;
        newValuation.emplace(var, p_valuation[var]);
    }

    p_maxIndex = maxIndex;
    p_valuation = newValuation;
    return true;
} // bool tseitinResolution(map<int,int>&, unsigned int&);

void yyerror(const char *s)
{
    OUTERROR("Parse error!  Message: " << s);
} // yyerror(const char*)
