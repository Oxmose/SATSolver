#include "SATSolver.h"
#include "SATSolverWL.h"

void SATSolverSTD::initializeMethod()
{
    for(auto iClause : m_unsatClauses)
        for(auto lit : m_clauses[iClause].getLiterals())
            m_valuation[lit.first] = -1;
            
}

void SATSolverSTD::applyLastDecision()
{

}

/*
    The function is in fact only used in preprocess for WL.
*/
bool SATSolverWL::deduce()
{
    if(m_unsatClauses.empty())
        return false;
    
    if(unitProp())
        return true;

    //If !p_first_bet we want to do uniquePol in all cases
    return uniquePol();
} // bool deduce()

void SATSolverWL::watchClauses()
{
    for(auto iClause: m_clauses)
    {
        if(m_clauses[iClause].getLiterals())
    }
}

bool SATSolverWL::solve()
{
    initializeMethod();
    flushTaut();
    bool unsat = false;

    //Preprocessing
    while(deduce())
    {}

    watchClauses();

    while(!m_unsatClauses.empty() && !unsat)
    {
        applyLastDecision();
        OUTDEBUG("\t" << currentStateToStr() );
        if(m_unsatClauses.empty())
            continue;

        if(backtrack(unsat))
            continue;

        OUTDEBUG("SAT rate: " << m_satClauses->size() << " " << m_satClauses->size()+m_unsatClauses.size() << endl);

        
        takeABet();
    }

    OUTDEBUG("evaluate : " << evaluate());
    return !unsat;
} // bool solve()