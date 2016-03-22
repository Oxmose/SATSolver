#include "SATSolver.h"
#include "SATSolverWL.h"

using namespace std;

SATSolverWL::~SATSolverWL()
{
    m_isWL = true;
}

void SATSolverWL::initializeMethod()
{
    for(auto iClause : m_unsatClauses)
        for(auto lit : m_clauses[iClause].getLiterals())
            m_valuation[lit.first] = -1;

    for(int iClause : m_unsatClauses)
    {
        if(m_clauses[iClause].getLiterals().size() >= 2)
        {
            int putTrigger = 0;
            for(auto lit : m_clauses[iClause].getLiterals())
                if(m_valuation[lit.first] == -1 && putTrigger < 2)
                {
                    m_clausesWatchedBy[lit.first].insert(iClause);
                    putTrigger++;
                }
        }
    }

}

bool SATSolverWL::backtrack(bool& p_unsat)
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

bool SATSolverWL::applyLastDecision()
{
    if(m_currentAssignement.empty())
        return false;

    decision p_dec = m_currentAssignement.back();
    OUTDEBUG("Handling " << ((p_dec.bet) ? string("bet") : string("deduction")) << ": "
            << p_dec.index << " to " << ((p_dec.value) ? string("True") : string("False")));
            
    OUTDEBUG("\t" << currentStateToStr());
    
    m_valuation[p_dec.index] = p_dec.value;

    m_isContradictory = false;

    vector<int> clausesToRemove;
    for(int iClause : m_clausesWatchedBy[p_dec.index])
        if(m_unsatClauses.find(iClause) != m_unsatClauses.end())
        {
            if(m_clauses[iClause].getLiterals()[p_dec.index] == !p_dec.value)
                satisfyClause(iClause,p_dec.index);
            else
            {
                int otherTrigger = -1;
                for(auto lit : m_clauses[iClause].getLiterals())
                    if(lit.first != p_dec.index && m_clausesWatchedBy[lit.first].find(iClause) != m_clausesWatchedBy[lit.first].end())
                    {
                        otherTrigger = lit.first;
                        break;
                    }

                if(otherTrigger == -1)
                    OUTERROR("Critical issue in apply decision : each clause should have two triggers");

                int newTrigger = p_dec.index;
                /* Is there a satisfying trigger ?*/
                for(auto lit : m_clauses[iClause].getLiterals())
                    if(m_valuation[lit.first] == !lit.second)
                    {
                        newTrigger = lit.first;
                        if(newTrigger == otherTrigger)
                            OUTERROR("Critical issue in apply decision : clause should be satisfied " << m_valuation[lit.first] << " " << !lit.second);
                        break;
                    }

                if(newTrigger != p_dec.index)
                {
                    m_clausesWatchedBy[newTrigger].insert(iClause);
                    clausesToRemove.push_back(iClause);
                    satisfyClause(iClause,newTrigger);
                    continue;
                }
                
                for(auto lit : m_clauses[iClause].getLiterals())
                    if(lit.first != otherTrigger && m_valuation[lit.first] == -1)
                    {
                        newTrigger = lit.first;
                        m_clausesWatchedBy[newTrigger].insert(iClause);
                        clausesToRemove.push_back(iClause);
                        break;
                    }

                if(newTrigger != p_dec.index)
                    continue;

                if(m_valuation[otherTrigger] == -1)
                {
                    int remainingVar = otherTrigger;
                    bool pol = m_clauses[iClause].getLiterals()[remainingVar];
                    m_deductionQueue.push(decision(remainingVar,!pol,false));
                    OUTDEBUG("\tDeducing (unit prop) : " << remainingVar << " to " << ((!pol) ? "True" : "False"));
                    continue;
                }

                m_isContradictory = true;
                OUTDEBUG("\tContradiction spotted! : " << m_clauses[iClause].toStr());
                return false;
            }
        }

    for(auto iClause: clausesToRemove)
        m_clausesWatchedBy[p_dec.index].erase(iClause);

    return m_unsatClauses.empty();
}

std::map<int,std::set<int>>& SATSolverWL::getAliveVars()
{
    for(int iClause : m_unsatClauses)
    {
        m_aliveVarsIn[iClause].clear();
        for(auto lit : m_clauses[iClause].getLiterals())
            if(m_valuation[lit.first] == -1)
                m_aliveVarsIn[iClause].insert(lit.first);
    }
    return m_aliveVarsIn;
}