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
    m_isWL = false;
    m_isCL = false;
} // SATSolver(bool)

void SATSolver::reset()
{
    m_clauses.clear();
    m_satClauses->clear();
    m_unsatClauses.clear();
    m_maxIndex = 0;
} // reset()

void SATSolver::setStrategy(shared_ptr<IBet> p_betMethod)
{
    m_betHeuristic = p_betMethod;
} // setStrategy(IBet*)

void SATSolver::setParser(shared_ptr<IParser> p_parser)
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

bool SATSolver::uniquePol(bool p_preprocess /* = false */)
{
    if(m_isWL)
        return false;
    if(m_isCL)
        return false;
    //We always want uniquePol in preprocess
    if(p_preprocess || !m_isWL )
    {
        //In the pair, first : sum of polarities, second: number of occurencies
        //We sum the polarities and check if it matches with +- #occurencies
        map<int,pair<int,int>> countPol;
        for(int iClause : m_unsatClauses)
            for(auto l : m_clauses[iClause].getLiterals())
                if(m_valuation[l.first] == -1)
                {
                    if(countPol.find(l.first) == countPol.end())
                        countPol[l.first] = make_pair(0,0);
                    countPol[l.first].first += (l.second) ? -1 : 1;
                    countPol[l.first].second++;
                }

        for(auto it = countPol.begin(); it != countPol.end(); ++it)
            if(it->second.first == it->second.second || it->second.first == -1*it->second.second)
            {
                decision deduction = decision(it->first,(it->second.first > 0),false);
                OUTDEBUG("\tDeducing (unique pol): " << it->first << " to " << ((it->second.first > 0) ? "True" : "False"));
                if(!p_preprocess)
                {
                    m_deductionQueue.push(deduction);
                    return true;
                }
                else
                    m_preprocessQueue.push(deduction);
            }

        return false;
    }

    return false;
}

void SATSolver::preprocess()
{
    OUTDEBUG("Preprocessing formula");
    /*
        Dedect unitary clauses in input
    */
    map<int,bool> deductionOn;
    vector<int> toErase;
    for(int iClause: m_unsatClauses)
    {        
        if(m_clauses[iClause].getLiterals().size() == 1)
        {
            int indexUnit = m_clauses[iClause].getLiterals().begin()->first;
            if(m_clauses[iClause].getLiterals().begin()->second)
                indexUnit = -indexUnit;
    

            if(deductionOn.find(-indexUnit) != deductionOn.end())
            {
                // Contradiction on unitary clause
                OUTDEBUG("\tContradiction (unitary clause): " << indexUnit << " is already assigned with an other value.");
                cout << "s UNSATISFIABLE" << endl;
                exit(0);
            }
            else if(deductionOn.find(indexUnit) == deductionOn.end())
            {
                bool value = !m_clauses[iClause].getLiterals()[abs(indexUnit)];
                m_preprocessQueue.push(decision(abs(indexUnit),value,false));
                deductionOn[indexUnit] = true;
                toErase.push_back(iClause);
                OUTDEBUG("\tDeducing (unitary clause): " << indexUnit << " to " << ((value) ? "True" : "False"));
            } 
	        else
                toErase.push_back(iClause);
        }
    }
    for(auto c: toErase)
    {
        OUTDEBUG("\tRemoving unitary clause: " << c);
        m_unsatClauses.erase(c);
    }

    uniquePol(true);

}   

bool SATSolver::deduce()
{  
    if(m_unsatClauses.empty())
        return true;

    /*
        We stack deduction only if their variable hasn't been assigned yet.    
    */

    if(!m_preprocessQueue.empty())
    {
        while(!m_preprocessQueue.empty() && m_valuation[m_preprocessQueue.front().index] != -1)
            m_preprocessQueue.pop();
        if(!m_preprocessQueue.empty())
        {
            m_currentAssignement.push_back(m_preprocessQueue.front());
            m_preprocessQueue.pop();
            return true;
        }
    }
    

    if(!m_deductionQueue.empty() || uniquePol())
    {
        while(!m_deductionQueue.empty() && m_valuation[m_deductionQueue.front().index] != -1)
            m_deductionQueue.pop();
        
        if(!m_deductionQueue.empty())
        {
            m_currentAssignement.push_back(m_deductionQueue.front());
            //OUTDEBUG("\tStack deduction: " << m_deductionQueue.front().index << " to " << m_deductionQueue.front().value << " " << m_valuation[m_deductionQueue.front().index]  << endl);
            m_deductionQueue.pop();

            return true;
        }
        return false;
    }

    return false;
} // bool deduce()

bool SATSolver::solve()
{   
    initializeMethod();
    flushTaut();

    preprocess();

    bool unsat = false;
    while(!m_unsatClauses.empty() && !unsat)
    {
        if(applyLastDecision())
            continue;

        if(backtrack(unsat))
            continue;

        if(deduce())
            continue;

        takeABet();
    }

    //OUTDEBUG("evaluate : " << evaluate());
    return !unsat;
} // bool solve()



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
