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
    m_satisfiedClause = 0;
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
    unsigned int maxIndex = 0;

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
            maxIndex = m_maxIndex;
            splitter >> m_clausesCount;
        }
        else
        {
            // Parse clauses

            //Create new clause
            stringstream splitter(line);

            vector<literal> literals;
            int literal;
            while(splitter >> literal)
            {
                if(literal == 0)
                    break;

                literals.push_back(literal_from_int(literal));

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

		        if(literal < 0)
                {
			        literal = -literal;
                }
		        if(literal > m_maxIndex)
                {
			        cerr << "The file has " << literal << " for index variable but " << m_maxIndex << " was announced as maximum index." << endl;
                    if(literal > maxIndex)
                        maxIndex = literal;
                }
            }

            // Create and save the clause
            Clause clause(literals);
            m_formula.push_back(clause);

            // Add one to counter for verification purposes
            ++clausesCount;

            // Create string format of the formula
        }
    }
    // Error management
    if(clausesCount != m_clausesCount)
    {        
        cerr << "The file has " << clausesCount << " clauses but " << m_clausesCount << " were announced." << endl;
        m_clausesCount = clausesCount;
    }

    if(maxIndex != vars.size())
    {        
	    vector<bool> used;
        for(int i = 0 ; i < maxIndex+1 ; i++)
            used.push_back(false);

	    for(unsigned int i : vars)
        {
		    used[i] = true;
        }

        for(unsigned int i = 1; i < maxIndex+1; ++i)
        {
            if(!used[i])
            {
                cerr << i << " was not used whereas maximum index is " << maxIndex << "." << endl;
            }
        }
    }

    file.close();

    return noError;
} // bool parse()

decision SATSolver::takeABet()
{
    int firstUnassigned = -1;
    
    for(Clause c : m_formula)
        if(!c.isSatisfied())
        {
            for(literal l : c.getLiterals())
            {
                if(!l.isAssigned)
                {
                    firstUnassigned = l.index;
                    break;
                }
            }
            if(firstUnassigned != -1)
                break;
        }

    cout << to_string(firstUnassigned) + " to True" << endl;

    decision bet = decision(firstUnassigned,true,true);
    cout << "Stacking bet" << endl;
    m_currentAssignement.push_back(bet);
    return bet;
}

string SATSolver::formulaToStr()
{
    string toReturn = "";
    for(int iClause = 0 ; iClause < m_formula.size() ; iClause++)
    {
        const Clause& c = m_formula[iClause];
        if(!c.isSatisfied())
        {
            string con = (iClause == m_formula.size()-1) ? "" : "/\\";
            toReturn +=  c.toStr() + con;
        }
    }

    return toReturn;
}

string SATSolver::decisionToStr()
{
    string toReturn = "";
    for(auto d : m_currentAssignement)
        toReturn += string((d.value) ? "" : "-") + to_string(d.index) + string((d.bet) ? "d" : "");
    return toReturn;
}

void SATSolver::dropSatisfiedBy(const decision& p_bet)
{
    for(Clause& c : m_formula)
    {
        if(c.hasVar(p_bet.index) && c.getLiteral(p_bet.index).bar == !p_bet.value)
        {
            c.setSatisfier(p_bet.index);
            cout << "\t\t Is satisfied : " << c.toStr() << endl;
            m_satisfiedClause++;
        }

    }
}

void SATSolver::assignVarInClause(int p_index, bool p_assign /* = true */)
{
    for(Clause& c : m_formula)
        if(c.hasVar(p_index))
            c.setAssigned(p_index, p_assign);
}


bool SATSolver::deduce()
{
    return unitProp();
}


bool SATSolver::unitProp()
{
    
    printf("\tActuellement : ");
    cout << decisionToStr() + " # " << formulaToStr() << endl << endl;
    for(Clause& c : m_formula)
        if(!c.isSatisfied() && c.getAliveVars() == 1)
        {
            printf("\t\tDeduction grace a la clause : %s", c.toStr().c_str());
            int indexUnit;
            for(const literal& l : c.getLiterals())
                if(!l.isAssigned)
                {
                    indexUnit = l.index;
                    break;
                }

            bool value = !c.getLiteral(indexUnit).bar;
            decision deduction = decision(indexUnit,value,false);
            cout << "Stacking deduction" << endl;
            m_currentAssignement.push_back(deduction);   
            return true;
        }   
    return false;
}

bool SATSolver::isContradictory()
{
    for(Clause c : m_formula)
        if(!c.isSatisfied() && c.getAliveVars() == 0)
            return true;
    return false;
}

void SATSolver::reviveClauseWithSatisfier(int p_satisfier)
{
    for(Clause& c : m_formula)
        if(c.getSatisfier() == p_satisfier)
        {
            printf("!!\n");
            cout << c.toStr() << endl;
            c.setSatisfier(-1);
            m_satisfiedClause--;
        }   
}

void SATSolver::applyDecision(decision p_dec)
{
    m_valuation[p_dec.index] = p_dec.value;

    cout << "\tDroping satisfied clauses" << endl;
    dropSatisfiedBy(p_dec);

    cout << "\tRemove "+to_string(p_dec.index)+" from remaining clauses" << endl << endl;
    assignVarInClause(p_dec.index);
}

void SATSolver::showSolution()
{
    for(int i = 1 ; i <= m_maxIndex ; i++)
    {
        string sign="";
        if(m_valuation.find(i) != m_valuation.end())
            sign = (m_valuation[i] == 0) ? "-" : ""; 
        cout << sign+to_string(i)+" ";
    }
    cout << "0" << endl;
    m_currentAssignement.clear();
}

int SATSolver::solve(bool verbose)
{
    
    cout << endl << "DPLL Starts" << endl;
    cout << "Initialize assignments" << endl;

    for(auto c : m_formula)
        for(auto l : c.getLiterals())
            if(m_valuation.find(l.index) == m_valuation.end())
            {
                cout << "\tSet variable "+to_string(l.index)+" to unassigned" << endl;
                m_valuation[l.index] = -1;
            }
    
    
    cout << endl << "Main Loop" << endl;
    cout << decisionToStr() + " # " << formulaToStr() << endl << endl;
    cout << "First bet" << endl;
    takeABet();
    int k = 0;
    while(!m_currentAssignement.empty())
    {
        decision currDecision = m_currentAssignement.back();
        cout << "1)Apply decision : ";
        cout << decisionToStr() + " # " << formulaToStr() << endl << endl;
        applyDecision(currDecision);

        cout << "So far : " << endl;
        cout << decisionToStr() + " # " << formulaToStr() << endl << endl;

        cout << "2)Deduce :" << endl;
        bool hasDeduced = deduce();

        cout << "3)Check for contradictions" << endl;
        bool backtrack = false;
        if(isContradictory())
        {
            backtrack = true;
            printf("Contradiction!!\n");
            printf("Backtrack\n");
            while(!m_currentAssignement.back().bet)
            {
                decision toErase = m_currentAssignement.back();
                assignVarInClause(toErase.index, false);
                reviveClauseWithSatisfier(toErase.index);
                m_currentAssignement.pop_back();
            }
            assignVarInClause(m_currentAssignement.back().index, false);
            reviveClauseWithSatisfier(m_currentAssignement.back().index);
            if(m_currentAssignement.back().value)
                m_currentAssignement.back().value = !m_currentAssignement.back().value;
            else
                cout << "UNSATISFIABLE !" << endl;
            
            cout << decisionToStr() + " # " << formulaToStr() << endl << endl;
        }

        cout << "Iteration done\n\n";
        
        if(m_satisfiedClause == m_formula.size())
        {
            cout << "s SATISFIABLE" << endl;
            showSolution();
        }
        else if(!hasDeduced && !backtrack)
            takeABet();
        //m_satisfiedClause = m_formula.size();
        //cout << "\tUnit clause :" << endl;

    /*
        cout << "\tDroping literals with "+to_string(bet) << "in remaining clauses" << endl;
    */
    }
    
    return true;

} // bool solve()

