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
#include <iostream> // std::cout std::cerr std::endl
#include <atomic>   // std::atomic

// OTHER INCLUDES FROM PROJECT
#include "../NewCore/SMT/SMTSolver_eq.h" // SMTSolver_eq class
#include "../NewCore/SATSolver.h" // SATSolver class
#include "../NewCore/clause.h" // Clause class

// PARSER INCLUDES
#include "expr.hpp"
#include "expr.tab.hpp"

// GLOBAL FLAGS/VARS
#include "../Global/Global.h"

// INHERITANCE INCLUDE
#include "IParser.h"

// CLASS HEADER INCLUDE
#include "LOGParser.h"

using namespace std;

LOGParser::LOGParser(const string &p_fileName /* = ""  */)
{
    m_fileName = p_fileName;
} // LOGParser(const string&)

LOGParser::~LOGParser()
{
} // ~LOGParser()

bool LOGParser::parse(SATSolver &p_solver, unsigned int &p_maxIndex)
{
    OUTDEBUG(fprintf(stderr, "LOG PARSE BEGIN\n"));
    // Open file
    yyin = fopen(m_fileName.c_str(), "r");
    if(yyin == NULL)
    {
        cout << "Can't open file." << endl;
        assert(false);
    }

    bool noParseError = true;

    /*
    ** Log expression format parse
    */
    map<pair<int, int>, Expr*> corresp;
    map<pair<int, int>, Expr*> ncorresp;
    vector<Expr*> exps;
    do 
    {
        yy_flex_debug = 1;
        yyparse();
        exps = tseitinTransform(res, p_maxIndex, corresp, ncorresp);        
    } while (!feof(yyin));   

    fclose(yyin);
    vector<pair<map<int,bool>, bool>> clauses;   

    // Browse each clause extracted and create the formula
    map<int,bool> clause;

    struct clause the_clause(p_solver.formula.size());

    bool hasTaut = false;
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
                    {
                        p_solver.valuation[abs(varInt)] = -1;
                        hasTaut = true;
                    }
                }

                // Add the variable
                if(!found)
                {
                    the_clause.literal.push_back(varInt);
                    clause[abs(varInt)] = varInt < 0;
                }

                continue;
            }
		
            // If we are at the end of a clause
            if(i < strForm.size() -1 && strForm[i] == '/' && strForm[i + 1] == '\\')
            {

                // Create and add the new clause
                if(!hasTaut)
                {
                    p_solver.add_clause(the_clause, true);
                    
                }
                else
                    OUTDEBUG(fprintf(stderr,"%lu is tautological, not added.\n", p_solver.formula.size()));

                the_clause.id = p_solver.formula.size();
	            the_clause.literal.clear();
                clause.clear();
                hasTaut = false;
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
                    {
                        p_solver.valuation[abs(varInt)] = -1;
                        hasTaut = true;
                    }
                }
		

                if(!found)
                {
                    the_clause.literal.push_back(varInt);
                    clause[abs(varInt)] = (varInt < 0);
                }
			
            }
        }
        // Create and add the last clause
        if(!hasTaut)
        {
            p_solver.add_clause(the_clause, true);
        }
        else
            OUTDEBUG(fprintf(stderr,"%lu is tautological, not added.\n", p_solver.formula.size()));

        the_clause.id = p_solver.formula.size();
	    the_clause.literal.clear();
        hasTaut = false;
        clause.clear();
    }
    for(auto entry : corresp)
    {
        struct smt_literal *eq = new smt_literal_eq(atoi(entry.second->to_string().c_str()), entry.first.first, entry.first.second, true);
        p_solver.emplace_eq(atoi(entry.second->to_string().c_str()), eq);
    }
    for(auto entry : ncorresp)
    {
        struct smt_literal *eq = new smt_literal_eq(atoi(entry.second->to_string().c_str()), entry.first.first, entry.first.second, false);
        p_solver.emplace_eq(atoi(entry.second->to_string().c_str()), eq);
    }

    OUTDEBUG(fprintf(stderr, "LOG PARSE END WITH STATUS %d\n", noParseError));
    return noParseError;
} // bool parse(unsigned int &, vector<Clause>&)

vector<Expr*> LOGParser::tseitinTransform(Expr *exp, unsigned int &p_maxIndex, map<pair<int, int>, Expr*> &corresp, map<pair<int, int>, Expr*> &ncorresp)
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
    Expr* global = res->tseitin(maxIndex, exps, corresp, ncorresp);
    exps.push_back(global);
    p_maxIndex = maxIndex;

    return exps;
} // bool tseitinTransform(Expr*, unsigned int&, std::vector<Clause>&)

bool LOGParser::tseitinResolution(map<unsigned int, int> &p_valuation, unsigned int &p_maxIndex)
{
    // For each vars that appeared in the original formula
    // Just keep them and forget about the others
    map<unsigned int, int> newValuation;
    unsigned int maxIndex = 0;
    for(unsigned int var : m_originalVars)
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
    cout << "Parse error!  Message: " << s << endl;
    assert(false);
} // yyerror(const char*)
