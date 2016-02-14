/*
 *
 *    CLASS SATSolver
 *
*/

// STD INCLUDES
#include <string>   // sts::string
#include <vector>   // std::vector
#include <set>      // std::set
#include <map>      // std::map
#include <sstream>  // std::stringstream
#include <iostream> // std::cout std::cerr std::endl
#include <fstream>  // std::ifstream

// CLASS HEADER INCLUDE
#include "SATSolver.h"

// PROJECT INCLUDES
#include "Clause.h"                 	// Clause class
#include "../CNFParser/CNFParser.h" 	// CNFParser class
#include "../LogExpParser/LOGParser.h"	// LOGParser class

using namespace std;

SATSolver::SATSolver(const string &p_fileName)
{
    m_fileName = p_fileName;
    m_satisfiedClause = 0;
} // SATSolver(const string&)

bool SATSolver::parse(PARSE_TYPE p_parseType /* = CNF_PARSE */)
{
    if(p_parseType == CNF_PARSE)
    {
        // Create parser and parse
        CNFParser parser(m_fileName);
        return parser.parse(m_maxIndex, m_formula);
    }
    else
    {
        LOGParser parser(m_fileName);
	return parser.parse(m_maxIndex, m_formula);
    }
} // bool parse(PARSE_TYPE)

SATSolver::~SATSolver()
{
} // ~SATSolver()

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
} // decision takeABet()

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
} // string formulaToStr()

string SATSolver::decisionToStr()
{
    string toReturn = "";
    for(auto d : m_currentAssignement)
        toReturn += string((d.value) ? "" : "-") + to_string(d.index) + string((d.bet) ? "b" : "d");
    return toReturn;
} // string decisionToStr()

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
} // dropSatisfiedBy(const decision&)

void SATSolver::assignVarInClause(int p_index, bool p_assign /* = true */)
{
    for(Clause& c : m_formula)
        if(c.hasVar(p_index))
            c.setAssigned(p_index, p_assign);
} // assigneVarInClause(int, bool)

bool SATSolver::deduce()
{
    return unitProp();
} // bool deduce()

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
} // bool unitProp()

bool SATSolver::isContradictory()
{
    for(Clause c : m_formula)
        if(!c.isSatisfied() && c.getAliveVars() == 0)
        {
            OUTDEBUG("Contradiction");
            return true;
        }
    return false;
} // bool isContradictory()

void SATSolver::reviveClauseWithSatisfier(int p_satisfier)
{
    for(Clause& c : m_formula)
        if(c.getSatisfier() == p_satisfier)
        {
            c.setSatisfier(-1);
            m_satisfiedClause--;
        }
} // reviveClauseWithSatisfier(int)

void SATSolver::applyDecision(decision p_dec)
{
    m_valuation[p_dec.index] = p_dec.value;
    dropSatisfiedBy(p_dec);
    assignVarInClause(p_dec.index);
} // applyDecision(decision)

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
} // showSolution()

bool SATSolver::evaluate()
{
    bool val = true;
    for(Clause c: m_formula)
        val = val && c.evaluate(m_valuation);
    return val;
} // bool evaluate()

string SATSolver::currentStateToStr()
{
    return decisionToStr() + "# " + formulaToStr();
} // string currentStateToStr()

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

} // int solve(bool)
