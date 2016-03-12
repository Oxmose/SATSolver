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
#include "SATSolver.h"    // SATSolver Class header

// PROJECT INCLUDES
#include "Clause.h"                         // Clause class
#include "../CNFParser/CNFParser.h"         // CNFParser class
#include "../LogExpParser/LOGParser.h"      // LOGParser class
#include "../BETHeuristic/IBet.h"           // Bet Heuristic Interface

// GLOBAL FLAGS/VARS
#include "../Global/Global.h"   // DEBUG

using namespace std;

SATSolver::SATSolver(bool p_watchedLitMeth)
{
    m_watchedLitMeht = p_watchedLitMeth;
    m_formula.push_back(ClauseSet(compareUnsat));
    m_formula.push_back(ClauseSet(compareSat));
} // SATSolver(bool)

void SATSolver::reset()
{
    m_formula.clear();
    m_formula.push_back(ClauseSet(compareUnsat));
    m_formula.push_back(ClauseSet(compareSat));
    m_maxIndex = 0;
} // reset()

void SATSolver::setStrategy(IBet *p_betMethod)
{
    m_betHeuristic = p_betMethod;
} // setStrategy(IBet*)

void SATSolver::setMaxIndex(int p_maxIndex)
{
    m_maxIndex = p_maxIndex;
} // setMaxIndex(int)

void SATSolver::setOriginFormula(const ClauseSet &initClauseSet)
{
    m_formula[0] = initClauseSet;
} // setOriginFormula(const ClauseSet&)

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
    //p_satisfier == -1 signifie rétrograder la clause
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
} // satisfyClause(It, int)

void SATSolver::reviveClauseWithSatisfier(int p_satisfier)
{

    Clause searchClause = makeSearchClause2(p_satisfier);
    auto its = m_formula[1].equal_range(searchClause);
    vector<It> toRevive;

    // Collect clauses to revive
    for(auto it = its.first ; it != its.second ; ++it)
        toRevive.push_back(it);

    // Satisfy clauses
    for(auto it : toRevive)
        satisfyClause(it,-1);
} // reviveClauseWithSatisfier(int)


bool SATSolver::isContradictory()
{
    // Simple check on the Clause size
    for(Clause c : m_formula[0])
        if(c.getLiterals(0).size() == 0)
        {
            OUTDEBUG("Contradiction");
            return true;
        }
    return false;
} // bool isContradictory()

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
} // bool backtrack(bool&)

bool SATSolver::unitProp()
{
    for(Clause c : m_formula[0])
        if(c.getLiterals().size() == 1)
        {
            int indexUnit = c.getLiterals().begin()->index;
            bool value = !c.getLiterals().begin()->bar;

            decision deduction = decision(indexUnit,value,false);
            OUTDEBUG("\tDeducing (unit prop): " << indexUnit << " to " << ((value) ? string("True") : string("False")));
            m_currentAssignement.push_back(deduction);
            return true;
        }
    return false;
} // bool unitProp()

bool SATSolver::uniquePol()
{
    //In the pair, first : sum of polarities, second: number of occurencies
    //We sum the polarities and check if it matches with +- #occurencies
    map<int,pair<int,int>> countPol;

    for(Clause c : m_formula[0])
        for(literal l : c.getLiterals())
        {
            if(countPol.find(l.index) == countPol.end())
                countPol[l.index] = make_pair(0,0);
            countPol[l.index].first += (l.bar) ? -1 : 1;
            countPol[l.index].second++;
        }

    for(auto it = countPol.begin(); it != countPol.end(); ++it)
        if(it->second.first == it->second.second || it->second.first == -1*it->second.second)
        {
            decision deduction = decision(it->first,(it->second.first > 0),false);
            m_currentAssignement.push_back(deduction);
            OUTDEBUG("\tDeducing (unique pol): " << it->first << " to " << ((it->second.first > 0) ? "True" : "False"));
            return true;
        }

    return false;
}

bool SATSolver::deduce()
{
    if(m_formula[0].empty())
        return false;

    if(unitProp())
        return true;

    return uniquePol();
} // bool deduce()

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
} // applyDecision(const decision&)

decision SATSolver::takeABet()
{
    return m_betHeuristic->takeABet(m_formula, m_currentAssignement);
} // decision takeABet()

void SATSolver::flushTaut()
{
    vector<It> toSatisfy;
    // For each tautologie, add it to the list of  the clauses to satisfy
    for(It it = m_formula[0].begin() ; it != m_formula[0].end() ; ++it)
    {
        if(it->isTaut())
        {
            OUTDEBUG("Tautologie detected in " << it->toStr());
            toSatisfy.push_back(it);
        }
    }

    // Avoid concurency (we can't delet elements in the previous loop
    for(It it : toSatisfy)
        satisfyClause(it, -2);//Special satisfier for taut
} // flushTaut()

bool SATSolver::solve()
{
    //Pre-calculus :
    //associates each variable to all the clause containing it as literal
    for(It it = m_formula[0].begin() ; it != m_formula[0].end() ; ++it)
        for(auto l : it->getLiterals())
        {
            m_valuation[l.index] = -1;
            m_clauseWithVar[l.index].push_back(it->getId());
        }

    flushTaut();//Get rid of tautologie

    /*  The preprocessing (initial init prop etc) are
        part of the main loop. (when m_currentAssignement is empty)
    */
    bool unsat = false;
    while(!m_formula[0].empty() && !unsat)
    {
        if(!m_currentAssignement.empty())
        {
            decision currDecision = m_currentAssignement.back();
            OUTDEBUG("Handling " << ((currDecision.bet) ? string("bet") : string("deduction")) << ": "
            << currDecision.index << " to " << ((currDecision.value) ? string("True") : string("False")));

            applyDecision(currDecision);
            OUTDEBUG("\t" << currentStateToStr());
        }

        if(backtrack(unsat))
            continue;

        OUTDEBUG("SAT rate: " << m_formula[1].size() << " " << m_formula[1].size()+m_formula[0].size() << endl);

        if(deduce())
            continue;

        takeABet();
    }

    OUTDEBUG("evaluate : " << evaluate());
    return !unsat;
} // bool solve()

void SATSolver::showSolution(LOGParser &parser)
{
    // If using tseitin transformation, we have to display only the originals variables
    parser.tseitinResolution(m_valuation, m_maxIndex);


    showSolution();
} // showSolution(LOGParser&)

void SATSolver::showSolution()
{
    // Display user-friendly output
    for(int i = 1 ; i <= m_maxIndex ; i++)
    {
        string sign="";
        if(m_valuation.find(i) != m_valuation.end())
        {
            sign = (m_valuation[i] == 0) ? "-" : "";
            cout << sign+to_string(i)+" ";
        }
    }

    // End of the valuation
    cout << "0" << endl;
    m_currentAssignement.clear();
} // showSolution()

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
} // bool evaluate()

string SATSolver::currentStateToStr()
{
    return decisionToStr() + "# " + formulaToStr();
} // string currentStateToStr()

string SATSolver::formulaToStr()
{
    string toReturn = "";
    for(auto c : m_formula[0])
    {
        string con =  "/\\";
        toReturn +=  c.toStr() + con;
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
