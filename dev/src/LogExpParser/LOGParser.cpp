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

using namespace std;

LOGParser::LOGParser(const string &p_fileName /* = ""  */)
{
    m_fileName = p_fileName;
} // LOGParser(const string&)

LOGParser::~LOGParser()
{
} // ~LOGParser()

void LOGParser::setFileName(const string &p_fileName)
{
    m_fileName = p_fileName;
} // setFileName(const string&)

bool LOGParser::parse(unsigned int &p_maxIndex, ClauseSet& p_formula)
{

    OUTDEBUG("LOG PARSE BEGIN");
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

    vector<Expr*> exps;
    do 
    {
        yy_flex_debug = 1;
        yyparse();
        exps = tseitinTransform(res, p_maxIndex);        
    } while (!feof(yyin));   

    fclose(yyin);

    vector<pair<vector<literal>, bool>> clauses;

    // Browse each clause extracted and create the formula
    vector<literal> clause;
    bool hasTot = false;
    for(Expr* exp : exps)
    {
        string strForm = exp->to_string();
	cout << strForm << endl;
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
                for(literal lit : clause)
                {
                    if((lit.bar && -(lit.index) == varInt) || (!lit.bar && lit.index == varInt))
                        found = true;           
                    else if((!lit.bar && -(lit.index) == varInt) || (lit.bar && lit.index == varInt))
                        hasTot = true;
                }

                // Add the variable
                if(!found)
                    clause.push_back(literal_from_int(varInt));

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
                for(literal lit : clause)
                {
                    if((lit.bar && -(lit.index) == varInt) || (!lit.bar && lit.index == varInt))
                        found = true;           
                    else if((!lit.bar && -(lit.index) == varInt) || (lit.bar && lit.index == varInt))
                        hasTot = true;
                }

                if(!found)
                    clause.push_back(literal_from_int(varInt));
            }
        }

        // Create and add the last clause
        clauses.push_back(make_pair(clause, hasTot));
        hasTot = false;
        clause.clear();
    }

    unsigned int clausesCount = 0;
    
    for(pair<vector<literal>, bool> clause : clauses)
    {
	for(unsigned int i = 0; i < clause.first.size(); ++i)
	{
		if(i != 0)
			cout << " \\/";
		cout << " " << (clause.first[i].bar ? "-" : "") << clause.first[i].index;
	}
	cout << endl;
        p_formula.insert(Clause(clause.first, clause.second, clausesCount));
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
    unsigned int maxIndex = 0;
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
    cerr << "Parse error!  Message: " << s << endl;
    exit(-1);
} // yyerror(const char*)
