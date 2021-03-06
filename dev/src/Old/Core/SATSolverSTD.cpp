#include "SATSolver.h"
#include "SATSolverSTD.h"

using namespace std;

SATSolverSTD::SATSolverSTD()
{
    OUTDEBUG("Using standard solver.");
}

SATSolverSTD::~SATSolverSTD()
{
}

void SATSolverSTD::initializeMethod()
{
    for(auto iClause : m_unsatClauses)
        for(auto lit : m_clauses[iClause].getLiterals())
        {
            m_valuation[lit.first] = -1;
            m_clausesWithVar[lit.first].push_back(iClause);
            m_aliveVarsIn[iClause].insert(lit.first);
        }
}


bool SATSolverSTD::backtrack(bool& p_unsat)
{
    if(isContradictory())
    {
        OUTDEBUG("Backtracking");
        bool lastBetFound = false;

        std::queue<decision> empty;
        std::swap(m_deductionQueue, empty );

        while(!m_currentAssignement.empty() && !lastBetFound)
        {
            decision toCancel = m_currentAssignement.back();
            lastBetFound = toCancel.bet;
            reviveClauseWithSatisfier(toCancel.index);

            for(int iClause : m_clausesWithVar[toCancel.index])
                m_aliveVarsIn[iClause].insert(toCancel.index);

            m_valuation[toCancel.index] = -1;
            if(!lastBetFound)
                m_currentAssignement.pop_back();
            else
            {
                //If we are on the last bet we turn it into a decision a change it
                m_currentAssignement.back().value = !m_currentAssignement.back().value;
                m_currentAssignement.back().bet = false;    
            }
        }

        if(m_currentAssignement.empty())
        {
            p_unsat = true;
            return true;
        }

        
        OUTDEBUG(currentStateToStr());

        return true;
    }
    return false;
} // bool backtrack(bool&)

bool SATSolverSTD::applyLastDecision()
{
    if(m_currentAssignement.empty())
        return false;

    decision p_dec = m_currentAssignement.back();
    OUTDEBUG("Handling " << ((p_dec.bet) ? string("bet") : string("deduction")) << ": "
            << p_dec.index << " to " << ((p_dec.value) ? string("True") : string("False")));
            
    //OUTDEBUG("\t" << currentStateToStr());
    
    m_valuation[p_dec.index] = p_dec.value;

    m_isContradictory = false;

    for(int iClause : m_clausesWithVar[p_dec.index])
    {
        if(m_unsatClauses.find(iClause) != m_unsatClauses.end())
        {
            if(m_clauses[iClause].getLiterals()[p_dec.index] == !p_dec.value)
                satisfyClause(iClause,p_dec.index);
            else
            {
                m_aliveVarsIn[iClause].erase(p_dec.index);
                if(m_aliveVarsIn[iClause].size()==1)
                {
                    int remainingVar = *m_aliveVarsIn[iClause].begin();
                    bool pol = m_clauses[iClause].getLiterals()[remainingVar];
                    m_deductionQueue.push(decision(remainingVar,!pol,false));
                    OUTDEBUG("\tDeducing (unit prop) : " << remainingVar << " to " << ((!pol) ? "True" : "False"));
                }
                m_isContradictory = m_aliveVarsIn[iClause].size() == 0;
                if(m_isContradictory)
                {
                    OUTDEBUG("\tContradiction spotted! : " << m_clauses[iClause].toStr());
                    return false;
                }
            }
        }
       }

    return m_unsatClauses.empty();
}


bool SATSolverSTD::uniquePol(bool p_preprocess /* = false */)
{
    //In the pair, first : sum of polarities, second: number of occurencies
    //We sum the polarities and check if it matches with +- #occurencies
    map<int,pair<int,int>> countPol;

    for(int iClause : m_unsatClauses)
        for(auto l : m_aliveVarsIn[iClause])
        {
            if(countPol.find(l) == countPol.end())
                countPol[l] = make_pair(0,0);
            countPol[l].first += (m_clauses[iClause].getLiterals()[l]) ? -1 : 1;
            countPol[l].second++;
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

double SATSolverSTD::getVarScores(int p_var)
{
    return -1;
}
