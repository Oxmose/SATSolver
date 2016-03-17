#include "SATSolver.h"
#include "SATSolverSTD.h"

using namespace std;

SATSolverSTD::~SATSolverSTD()
{
}

void SATSolverSTD::initializeMethod()
{
	for(auto iClause : m_unsatClauses)
        for(auto lit : m_clauses[iClause].getLiterals())
        {
            m_valuation[lit.first] = -1;
            m_clauseWithVar[lit.first].push_back(iClause);
            m_aliveVarIn[iClause].insert(lit.first);
        }
}


bool SATSolverSTD::backtrack(bool& p_unsat)
{
    if(isContradictory())
    {
        OUTDEBUG("Backtracking");
        bool lastBetFound = false;
        while(!m_currentAssignement.empty() && !lastBetFound)
        {
            decision toErase = m_currentAssignement.back();
            lastBetFound = toErase.bet;
            reviveClauseWithSatisfier(toErase.index);

            for(int iClause : m_clauseWithVar[toErase.index])
                m_aliveVarIn[iClause].insert(toErase.index);

            m_valuation[toErase.index] = -1;
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


bool SATSolverSTD::uniquePol()
{
    //In the pair, first : sum of polarities, second: number of occurencies
    //We sum the polarities and check if it matches with +- #occurencies
    map<int,pair<int,int>> countPol;

    for(int iClause : m_unsatClauses)
        for(auto l : m_aliveVarIn[iClause])
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
            m_currentAssignement.push_back(deduction);
            OUTDEBUG("\tDeducing (unique pol): " << it->first << " to " << ((it->second.first > 0) ? "True" : "False"));
            return true;
        }

    return false;
}

bool SATSolverSTD::unitProp()
{
	for(auto iClause : m_unsatClauses)
        if(m_aliveVarIn[iClause].size() == 1)
        {
            int indexUnit = *m_aliveVarIn[iClause].begin();
            bool value = !m_clauses[iClause].getLiterals()[indexUnit];

            decision deduction = decision(indexUnit,value,false);
            OUTDEBUG("\tDeducing (unit prop): " << indexUnit << " to " << ((value) ? string("True") : string("False")));
            m_currentAssignement.push_back(deduction);
            return true;
        }
    return false;
}


bool SATSolverSTD::deduce()
{
    if(m_unsatClauses.empty())
        return false;
    
    if(unitProp())
        return true;

    //If !p_first_bet we want to do uniquePol in all cases
    return uniquePol();
} // bool deduce()

void SATSolverSTD::applyLastDecision()
{
	if(m_currentAssignement.empty())
		return;

	decision p_dec = m_currentAssignement.back();
    OUTDEBUG("Handling " << ((p_dec.bet) ? string("bet") : string("deduction")) << ": "
            << p_dec.index << " to " << ((p_dec.value) ? string("True") : string("False")));
            
    OUTDEBUG("\t" << currentStateToStr());
    
	m_valuation[p_dec.index] = p_dec.value;
    m_isContradictory = false;
    for(int iClause : m_clauseWithVar[p_dec.index])
        if(m_unsatClauses.find(iClause) != m_unsatClauses.end())
        {
            if(m_clauses[iClause].getLiterals()[p_dec.index] == !p_dec.value)
                satisfyClause(iClause,p_dec.index);
            else
            {
                m_aliveVarIn[iClause].erase(p_dec.index);
                m_isContradictory = m_aliveVarIn[iClause].size() == 0;
                if(m_isContradictory)
                    return;
            }
        }
   	

}

bool SATSolverSTD::solve()
{
    
    
    initializeMethod();
    flushTaut();
    bool unsat = false;
    while(!m_unsatClauses.empty() && !unsat)
    {
        applyLastDecision();
        OUTDEBUG("\t" << currentStateToStr() );
        if(m_unsatClauses.empty())
            continue;

        if(backtrack(unsat))
            continue;

        OUTDEBUG("SAT rate: " << m_satClauses->size() << " " << m_satClauses->size()+m_unsatClauses.size() << endl);

        if(deduce())
            continue;
        
        takeABet();
    }

    OUTDEBUG("evaluate : " << evaluate());
    return !unsat;
} // bool solve()
