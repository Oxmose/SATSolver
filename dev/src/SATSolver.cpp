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
                if(literals[i].bar)
                    m_strFormula += "-" + to_string(literals[i].index);
                else
                    m_strFormula += "" + to_string(literals[i].index);

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
    m_currentAssignement.push_back(bet);
    m_valuation[bet.index] = bet.value;

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
            c.setSatisfied();
            cout << "\t\t Is satisfied : " << c.toStr() << endl;
            m_satisfiedClause++;
        }

    }
}

void SATSolver::assignVarInClause(int p_index)
{
    for(Clause& c : m_formula)
        if(c.hasVar(p_index))
            c.setAssigned(p_index);
}

void SATSolver::unitProp()
{
    bool unitClauseExists = true;
    while(unitClauseExists)
    {
        unitClauseExists = false;
        printf("\tActuellement : ");
        cout << decisionToStr() + " # " << formulaToStr() << endl << endl;
        for(Clause& c : m_formula)
            if(!c.isSatisfied() && c.getAliveVars() == 1)
            {
                printf("\t\tDeduction grace a la clause : %s", c.toStr().c_str());
                unitClauseExists = true;
                int indexUnit;
                for(const literal& l : c.getLiterals())
                    if(!l.isAssigned)
                    {
                        indexUnit = l.index;
                        break;
                    }

                bool value = !c.getLiteral(indexUnit).bar;
                m_valuation[indexUnit] = value;
                decision deduction = decision(indexUnit,value,false);
                m_currentAssignement.push_back(deduction);

                dropSatisfiedBy(deduction);
                assignVarInClause(indexUnit);
            }
    }
            
}

bool SATSolver::isContradictory()
{
    for(Clause c : m_formula)
        if(!c.isSatisfied() && c.getAliveVars() == 0)
            return true;
    return false;
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

    while(m_satisfiedClause != m_formula.size())
    {
        cout << decisionToStr() + " # " << formulaToStr() << endl << endl;
        cout << "1)Making a bet : ";
        decision bet = takeABet();
        
        cout << "\tDroping satisfied clauses" << endl;
        dropSatisfiedBy(bet);

        cout << "\tRemove "+to_string(bet.index)+" from remaining clauses" << endl << endl;
        assignVarInClause(bet.index);

        cout << "So far : " << endl;
        cout << decisionToStr() + " # " << formulaToStr() << endl << endl;

        cout << "2)Deduce :" << endl;
        cout << "Unit prop" << endl;
        unitProp();

        cout << "3)Check for contradictions" << endl;
        if(isContradictory())
        {
            printf("Contradiction!!\n");
            exit(1);
        }

        cout << "Iteration done\n\n";
        //m_satisfiedClause = m_formula.size();
        //cout << "\tUnit clause :" << endl;

    /*
        cout << "\tDroping literals with "+to_string(bet) << "in remaining clauses" << endl;
    */
    }
    
    return true;
} // bool solve()

string SATSolver::getFormulaStr()
{
    return m_strFormula;
} // string getFormulaStr()
