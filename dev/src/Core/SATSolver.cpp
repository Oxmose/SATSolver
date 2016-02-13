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
#include "SATSolver.h"	// SATSolver Class header

// PROJECT INCLUDES
#include "Clause.h"                 	// Clause class
#include "../CNFParser/CNFParser.h" 	// CNFParser class
#include "../LogExpParser/LOGParser.h"	// LOGParser class

using namespace std;

SATSolver::SATSolver(const string &p_fileName)
{
    m_fileName = p_fileName;
    m_formula.push_back(ClauseSet(compareUnsat));
    m_formula.push_back(ClauseSet(compareSat));
} // SATSolver(const string&)

bool SATSolver::parse(PARSE_TYPE p_parseType /* = CNF_PARSE */)
{
    m_parseType = p_parseType;
    if(p_parseType == CNF_PARSE)
    {
        // Create parser and parse
        CNFParser parser(m_fileName);
        return parser.parse(m_maxIndex, m_formula[0]);
    }
    else
    {
        m_parser.setFileName(m_fileName);
        return m_parser.parse(m_maxIndex, m_formula[0]);
    }
} // bool parse(PARSE_TYPE)

SATSolver::~SATSolver()
{
} // ~SATSolver()

/*
    Functions are stacked over solve() in more or less the order
in which they appear in it .
    Goto l.178
*/

void SATSolver::satisfyClause(It p_it, int p_satisfier)
{
    if(p_satisfier != -1)
    {
        Clause c = *p_it;
        c.setSatisfier(p_satisfier);
        m_formula[0].erase(p_it);
        m_formula[1].insert(c);
        return;
    }

    Clause c = *p_it;
    c.setSatisfier(-1);
    m_formula[1].erase(p_it);
    m_formula[0].insert(c);
}

void SATSolver::reviveClauseWithSatisfier(int p_satisfier)
{
    Clause searchClause = makeSearchClause2(p_satisfier);
    auto its = m_formula[1].equal_range(searchClause);
    vector<It> toRevive;
    for(auto it = its.first ; it != its.second ; ++it)
        toRevive.push_back(it);
    for(auto it : toRevive)
        satisfyClause(it,-1);
}


bool SATSolver::isContradictory()
{
    for(Clause c : m_formula[0])
        if(c.getLiterals(0).size() == 0)
        {
            OUTDEBUG("Contradiction");
            return true;
        }
    return false;
}

bool SATSolver::backtrack(bool& p_unsat)
{
    if(isContradictory())
    {
        OUTDEBUG("Backtracking");
        while(!m_currentAssignement.empty() && !m_currentAssignement.back().bet)
        {
            decision toErase = m_currentAssignement.back();
            reviveClauseWithSatisfier(toErase.index);
            for(int iClause : m_clauseWithVar[toErase.index])
                if(m_formula[0].find(makeSearchClause(iClause)) != m_formula[0].end())
                    m_formula[0].find(makeSearchClause(iClause))->setAssigned(toErase.index,false);
            m_valuation[toErase.index] = -1;
            m_currentAssignement.pop_back();
        }

        if(m_currentAssignement.empty())
        {
            p_unsat = true;
            return true;
        }

        reviveClauseWithSatisfier(m_currentAssignement.back().index);

        for(int iClause : m_clauseWithVar[m_currentAssignement.back().index])
            if(m_formula[0].find(makeSearchClause(iClause)) != m_formula[0].end())
                m_formula[0].find(makeSearchClause(iClause))->setAssigned(m_currentAssignement.back().index,false);

        m_currentAssignement.back().value = !m_currentAssignement.back().value;
        m_currentAssignement.back().bet = false;
        OUTDEBUG(currentStateToStr());

        return true;
    }
    return false;
}

bool SATSolver::unitProp()
{
    for(Clause c : m_formula[0])
        if(c.getLiterals(0).size() == 1)
        {
            int indexUnit = c.getLiterals(0).begin()->index;
            bool value = !c.getLiterals(0).begin()->bar;

            decision deduction = decision(indexUnit,value,false);
            OUTDEBUG("\tDeducing: " << indexUnit << " to " << ((value) ? string("True") : string("False")));
            m_currentAssignement.push_back(deduction);
            return true;
        }
    return false;
} // bool unitProp()

bool SATSolver::deduce()
{
    return unitProp();
}

void SATSolver::applyDecision(const decision& p_dec)
{
    std::vector<It> toErase;
    for(int iClause : m_clauseWithVar[p_dec.index])
    {
        Clause searchClause = makeSearchClause(iClause);
        It it = m_formula[0].find(searchClause);
        literal searchLit = literal(p_dec.index,!p_dec.value);
        if(it != m_formula[0].end() && (it->getLiterals(0).find(searchLit) != it->getLiterals(0).end()))
        {
            if(it->getLiterals(0).find(searchLit)->bar == !p_dec.value)
                toErase.push_back(it);
            it->setAssigned(p_dec.index);
        }
    }

    //Drop satisfied clauses
    for(It it : toErase)
        satisfyClause(it,p_dec.index);

    m_valuation[p_dec.index] = p_dec.value;
}

decision SATSolver::takeABet()
{
    int firstUnassigned = -1;

    for(auto it=m_formula[0].begin() ; it != m_formula[0].end() ; ++it)
        if(!it->getLiterals(0).empty())
        {
            firstUnassigned = it->getLiterals(0).begin()->index;
            break;
        }

    decision bet = decision(firstUnassigned,true,true);
    OUTDEBUG("Taking bet: " << firstUnassigned << " to True");
    m_currentAssignement.push_back(bet);
    return bet;
}

void SATSolver::flushTaut()
{
    for(It it = m_formula[0].begin() ; it != m_formula[0].begin() ; ++it)
        if(it->isTaut())
        {
            OUTDEBUG("Tautologie detected in " << it->toStr());
            satisfyClause(it,-2);//Special satisfier for taut
        }
}

int SATSolver::solve()
{
    //Get rid of tautologies
    //TODO : buffer overflow...
    //flushTaut();

    //Pre-calculus :
    //associates each variable to all the clause containing it as literal
    for(It it = m_formula[0].begin() ; it != m_formula[0].end() ; ++it)
    {
        for(auto l : it->getLiterals())
        {
            m_clauseWithVar[l.index].push_back(it->getId());
            m_valuation[l.index] = -1;//Initialization aswell
        }
    }

    OUTDEBUG(endl << currentStateToStr());
    takeABet();//Initial bet
    while(!m_currentAssignement.empty())
    {
        decision currDecision = m_currentAssignement.back();
        OUTDEBUG("Handling " << ((currDecision.bet) ? string("bet") : string("deduction")) << ": "
        << currDecision.index << " to " << ((currDecision.value) ? string("True") : string("False")));

        applyDecision(currDecision);
        OUTDEBUG("\t" << currentStateToStr());

        bool unsat = false;
        if(backtrack(unsat))
        {
            if(unsat)
                return false;//UNSATISFIABLE !
            continue;
        }

        OUTDEBUG("SAT rate : " << m_formula[1].size() << " " << m_formula[1].size()+m_formula[0].size());
        OUTDEBUG("");//endl

        if(m_formula[0].empty())//SATISFIABLE !
        {
            OUTDEBUG("Evaluation: " << evaluate());
            return true;
        }

        if(deduce())
            continue;

        takeABet();
    }
} // bool solve()

void SATSolver::showSolution()
{
    if(m_parseType == LOG_PARSE)
    {
        m_parser.tseitinResolution(m_valuation, m_maxIndex);
    }
    

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
    for(Clause c: m_formula[1])
    {
        val = val && c.evaluate(m_valuation);
        if(!val)
            break;
    }
    return val;
}

string SATSolver::currentStateToStr()
{
    return decisionToStr() + "# " + formulaToStr();
}

string SATSolver::formulaToStr()
{
    string toReturn = "";
    for(auto c : m_formula[0])
    {
        string con =  "/\\";
        toReturn +=  c.toStr() + con;
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
