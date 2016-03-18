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
#include "Clause.h"                     // Clause class
#include "../Parser/IParser.h"          // Parsers Interface
#include "../BETHeuristic/IBet.h"       // Bet Heuristic Interface

// GLOBAL FLAGS/VARS
#include "../Global/Global.h"   // DEBUG

using namespace std;

SATSolver::SATSolver()
{
    m_satClauses = unique_ptr<satClausesSet>(new satClausesSet(compareSat));
} // SATSolver(bool)

void SATSolver::reset()
{
    m_clauses.clear();
    m_satClauses->clear();
    m_unsatClauses.clear();
    m_maxIndex = 0;
} // reset()

void SATSolver::setStrategy(IBet* p_betMethod)
{
    m_betHeuristic = unique_ptr<IBet>(p_betMethod);
} // setStrategy(IBet*)

void SATSolver::setParser(IParser* p_parser)
{
    m_parser = p_parser;
} // setParser(IParser*)

void SATSolver::setMaxIndex(int p_maxIndex)
{
    m_maxIndex = p_maxIndex;
} // setMaxIndex(int)

void SATSolver::setOriginFormula(const std::vector<Clause> &p_clauses)
{
    m_clauses = p_clauses;
    //Set all to unsat
    for(auto c : p_clauses)
        m_unsatClauses.insert(c.getId());
} // setOriginFormula(const ClauseSet&)

SATSolver::~SATSolver()
{
} // ~SATSolver()

/*
    Functions are stacked over solve() in more or less the order
in which they appear in it .
    Goto l.178
*/

void SATSolver::satisfyClause(int p_iClause, int p_satisfier)
{
    m_satClauses->insert(make_pair(p_iClause,p_satisfier));
    m_unsatClauses.erase(p_iClause);
} // satisfyClause(It, int)

void SATSolver::reviveClauseWithSatisfier(int p_satisfier)
{

    auto its = m_satClauses->equal_range(make_pair(-1,p_satisfier));
    vector<decltype(its.first)> toRevive;

    // Collect clauses to revive
    for(auto it = its.first ; it != its.second ; ++it)
        toRevive.push_back(it);

    // Set clauses unsat
    for(auto it : toRevive)
    {
        m_unsatClauses.insert(it->first);
        m_satClauses->erase(it);
    }
} // reviveClauseWithSatisfier(int)


bool SATSolver::isContradictory()
{
    return m_isContradictory;
} // bool isContradictory()

decision SATSolver::takeABet()
{
    decision bet = m_betHeuristic->takeABet(m_clauses, m_unsatClauses, m_valuation);
    m_currentAssignement.push_back(bet);
    return bet;
} // decision takeABet()

void SATSolver::flushTaut()
{
    vector<int> toSatisfy;
    // For each tautologie, add it to the list of  the clauses to satisfy
    for(auto iClause: m_unsatClauses)
        if(m_clauses[iClause].isTaut())
        {
            OUTDEBUG("Tautologie detected in " << m_clauses[iClause].toStr());
            toSatisfy.push_back(iClause);
        }

    // Avoid concurency (we can't delet elements in the previous loop
    for(auto iClause : toSatisfy)
        satisfyClause(iClause, -2);//Special satisfier for taut
} // flushTaut()

void SATSolver::showSolution()
{
    // If using tseitin transformation, we have to display only the originals variables
    // Strategy handle the choice of resolution (tseitin of cnf)
    m_parser->tseitinResolution(m_valuation, m_maxIndex);

    // Display user-friendly output
    for(unsigned int i = 1 ; i <= m_maxIndex ; i++)
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
    for(auto c: *m_satClauses)
    {
        val = val && m_clauses[c.first].evaluate(m_valuation);
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
    for(auto iClause : m_unsatClauses)
    {
        string con =  "/\\";
        toReturn +=  m_clauses[iClause].toStr() + con;
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
