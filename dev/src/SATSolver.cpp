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
#include <iostream> // std:://cout std::cerr std::endl
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

    decision bet = decision(firstUnassigned,true,true);
    OUTDEBUG("Taking bet: " << firstUnassigned << " to True");
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
        toReturn += string((d.value) ? "" : "-") + to_string(d.index) + string((d.bet) ? "b" : "d");
    return toReturn;
}

void SATSolver::dropSatisfiedBy(const decision& p_bet)
{
    for(Clause& c : m_formula)
    {
        if(!c.isSatisfied() && c.hasVar(p_bet.index) && c.getLiteral(p_bet.index).bar == !p_bet.value)
        {
            c.setSatisfier(p_bet.index);
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

    for(Clause& c : m_formula)
        if(!c.isSatisfied() && c.getAliveVars() == 1)
        {
            int indexUnit;
            for(const literal& l : c.getLiterals())
                if(!l.isAssigned)
                {
                    indexUnit = l.index;
                    break;
                }

            bool value = !c.getLiteral(indexUnit).bar;

            decision deduction = decision(indexUnit,value,false);
            OUTDEBUG("\tDeducing: " << indexUnit << " to " << ((value) ? string("True") : string("False")));
            m_currentAssignement.push_back(deduction);
            return true;
        }

    return false;
}

bool SATSolver::isContradictory()
{
    for(Clause c : m_formula)
        if(!c.isSatisfied() && c.getAliveVars() == 0)
        {
            OUTDEBUG("Contradiction");
            return true;
        }
    return false;
}

void SATSolver::reviveClauseWithSatisfier(int p_satisfier)
{
    for(Clause& c : m_formula)
        if(c.getSatisfier() == p_satisfier)
        {
            c.setSatisfier(-1);
            m_satisfiedClause--;
        }
}

void SATSolver::applyDecision(decision p_dec)
{
    m_valuation[p_dec.index] = p_dec.value;
    dropSatisfiedBy(p_dec);
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

bool SATSolver::evaluate()
{
    bool val = true;
    for(Clause c: m_formula)
        val = val && c.evaluate(m_valuation);
    return val;
}

string SATSolver::currentStateToStr()
{
    return decisionToStr() + "# " + formulaToStr();
}

int SATSolver::solve(bool verbose)
{
    for(auto c : m_formula)
        for(auto l : c.getLiterals())
            if(m_valuation.find(l.index) == m_valuation.end())
                m_valuation[l.index] = -1;

    OUTDEBUG(endl << currentStateToStr());
    takeABet();//Initial bet
    while(!m_currentAssignement.empty())
    {
        decision currDecision = m_currentAssignement.back();
        OUTDEBUG("Handling " << ((currDecision.bet) ? string("bet") : string("deduction")) << ": "
        << currDecision.index << " to " << ((currDecision.value) ? string("True") : string("False")));

        applyDecision(currDecision);
        OUTDEBUG("\t" << currentStateToStr());

        bool backtrack = isContradictory();

        if(backtrack)
        {
            OUTDEBUG("Backtracking");
            while(!m_currentAssignement.empty() && !m_currentAssignement.back().bet)
            {
                decision toErase = m_currentAssignement.back();
                assignVarInClause(toErase.index, false);
                reviveClauseWithSatisfier(toErase.index);
                m_currentAssignement.pop_back();
            }

            if(m_currentAssignement.empty())
            {
                cout << "s UNSATISFIABLE" << endl;
                return false;
            }

            assignVarInClause(m_currentAssignement.back().index, false);
            reviveClauseWithSatisfier(m_currentAssignement.back().index);
            m_currentAssignement.back().value = !m_currentAssignement.back().value;
            m_currentAssignement.back().bet = false;
            OUTDEBUG(currentStateToStr());
            continue;
        }

        bool hasDeduced = deduce();

        OUTDEBUG("SAT rate : " << m_satisfiedClause << " " << m_formula.size());
        OUTDEBUG("");//endl
        if(m_satisfiedClause == m_formula.size())
        {
            cout << "s SATISFIABLE" << endl;
            showSolution();
            OUTDEBUG("Evaluation: " << evaluate());
        }
        else if(!hasDeduced && !backtrack)
            takeABet();
    }


    return true;

} // bool solve()
