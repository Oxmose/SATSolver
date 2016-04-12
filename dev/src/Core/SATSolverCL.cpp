#include "SATSolver.h"
#include "SATSolverCL.h"
#include "ConflictGraph.h"

using namespace std;

SATSolverCL::~SATSolverCL()
{
}

void SATSolverCL::initializeMethod()
{
    for(auto iClause : m_unsatClauses)
        for(auto lit : m_clauses[iClause].getLiterals())
        {
            m_valuation[lit.first] = -1;
            m_clausesWithVar[lit.first].push_back(iClause);
            m_aliveVarsIn[iClause].insert(lit.first);
        }
}


bool SATSolverCL::backtrack(bool& p_unsat)
{
    if(isContradictory())
    {
    	//if(m_btLevel == -1)
    	//	OUTERROR("Bt level shouldn't be -1 " << m_currentAssignement.size());

        OUTDEBUG("Backtracking");
        bool lastBetFound = false;

        while(!m_deductionQueue.empty())
        {
        	auto dec = m_deductionQueue.front();
        	m_parentsOf.clear();
        	OUTDEBUG("Free " << dec.index);
        	m_deductionQueue.pop();
        }

        m_conflictGraph.remove_node(make_pair(m_currentAssignement.back().index,!m_currentAssignement.back().value));

        while(!m_currentAssignement.empty() && !lastBetFound)
        {
            decision toCancel = m_currentAssignement.back();
            lastBetFound = toCancel.bet && (m_currLevel == m_btLevel);
            if(toCancel.bet)
            	m_currLevel--;

            reviveClauseWithSatisfier(toCancel.index);
            m_conflictGraph.remove_node(make_pair(toCancel.index,toCancel.value));
            for(int iClause : m_clausesWithVar[toCancel.index])
                m_aliveVarsIn[iClause].insert(toCancel.index);

            m_valuation[toCancel.index] = -1;
            if(!lastBetFound)
                m_currentAssignement.pop_back();
            else
            {
                //If we are on the last bet we turn it into a deduction and change it
                m_currentAssignement.back().value = !m_currentAssignement.back().value;
                m_currentAssignement.back().bet = false;
                m_currentAssignement.back().ancien_bet = true; 
                m_conflictGraph.add_node(make_pair(make_pair(m_currentAssignement.back().index,m_currentAssignement.back().value),m_currLevel));
            }
        }

        if(m_currentAssignement.empty())
        {
            p_unsat = true;
            return true;
        }

        /*auto g = m_conflictGraph.get_graph();
        for(auto a : g)
        	printf("%d %d\n", a.first.first, a.first.second);*/

        addResolutionClause();
        OUTDEBUG(currentStateToStr());

        return true;
    }
    return false;
} // bool backtrack(bool&)


decision SATSolverCL::takeABet()
{
    decision bet = m_betHeuristic->takeABet(m_clauses, m_unsatClauses, m_valuation);
    m_currentAssignement.push_back(bet);

    m_currLevel++;
    m_conflictGraph.add_node(make_pair(make_pair(bet.index,bet.value),m_currLevel));
    return bet;
} // decision takeABet()

void SATSolverCL::addResolutionClause()
{
	m_clauses.push_back(m_resolutionClause);
	m_unsatClauses.insert(m_resolutionClause.getId());

	int iClause = m_resolutionClause.getId();

    for(auto lit : m_clauses[iClause].getLiterals())
    {
        m_clausesWithVar[lit.first].push_back(iClause);
        m_aliveVarsIn[iClause].insert(lit.first);
    }

    OUTDEBUG("Resolution clause " << m_resolutionClause.toStr() << " added");
}

bool SATSolverCL::applyLastDecision()
{
    static int iFile = 0;
    static int iter = 0;
    if(m_currentAssignement.empty())
        return false;

    decision p_dec = m_currentAssignement.back();
    //printf("%d\n", m_deductionQueue.size());
    OUTDEBUG("ITER " << iter);
    //OUTDEBUG(m_deductionQueue.size());
    iter++;
    OUTDEBUG("Handling " << ((p_dec.bet) ? string("bet") : string("deduction")) << ": "
            << p_dec.index << " to " << ((p_dec.value) ? string("True") : string("False")));
    
    if(!p_dec.bet && !p_dec.ancien_bet)
    {
    	OUTDEBUG("Coming from " << m_clauses[m_parentsOf[p_dec.index]].toStr() << " " << m_parentsOf[p_dec.index]);
        m_conflictGraph.add_node(make_pair(make_pair(p_dec.index,p_dec.value),m_currLevel));
        //printf("Clause for %d %d %s\n",p_dec.index, m_parentsOf[p_dec.index], m_clauses[m_parentsOf[p_dec.index]].toStr().c_str());
        if(m_parentsOf.find(p_dec.index) != m_parentsOf.end())
        	for(auto l : m_clauses[m_parentsOf[p_dec.index]].getLiterals())
            	if(l.first != p_dec.index)
                	m_conflictGraph.add_edge(make_pair(l.first,m_valuation[l.first]),make_pair(p_dec.index,p_dec.value));
        m_parentsOf.erase(p_dec.index);
    }
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
                    OUTDEBUG("\tDeducing (unit prop) : " << remainingVar << " to " << ((!pol) ? "True" : "False") << " from " << m_clauses[iClause].toStr() << " " << iClause);
                    //printf("Clause %d\n", iClause);
                    
                    if(m_parentsOf.find(remainingVar) == m_parentsOf.end())
                    {
                    	//OUTDEBUG("OK\n" << *m_aliveVarsIn[iClause].begin()); 
                    	m_parentsOf[remainingVar] = iClause;
                    }
                }
                m_isContradictory = m_aliveVarsIn[iClause].size() == 0;
                if(m_isContradictory)
                {
                	m_conflictGraph.add_node(make_pair(make_pair(p_dec.index,!m_valuation[p_dec.index]),m_currLevel));
                	for(auto l : m_clauses[iClause].getLiterals())
                		if(l.first != p_dec.index)
                			m_conflictGraph.add_edge(make_pair(l.first,m_valuation[l.first]),make_pair(p_dec.index,!m_valuation[p_dec.index]));


                    OUTDEBUG("\tContradiction spotted! : " << m_clauses[iClause].toStr());

                    int the_bet = -1;
                    for(int i = m_currentAssignement.size()-1  ; i >= 0 ; i--)
                        if(m_currentAssignement[i].bet)
                        {
                            the_bet = m_currentAssignement[i].index;
                            break;
                        }


                	pair<Clause,int> whatINeed = m_conflictGraph.resolution(make_pair(the_bet,m_valuation[the_bet]),make_pair(p_dec.index, m_valuation[p_dec.index]), m_clauses.size());
                    //printf("%s\n", whatINeed.first.toDIMACS().c_str());
                    //printf("%d\n", whatINeed.second);
                    m_btLevel = whatINeed.second;
                    m_resolutionClause = whatINeed.first;

                    if(m_interact)
                    {
	                    printf("%s\n", whatINeed.first.toDIMACS().c_str());
	                    printf("%d\n", whatINeed.second);
	                    
	                    printf("Choice [g/c/t] : ");
	                    char c;
	                    cin >> c;
	                    if(c == 'g')
	                    {
	                        m_conflictGraph.output("conflictGraph"+to_string(iFile)+".dot", the_bet, p_dec.index);
	                        OUTDEBUG("Output : " << "conflictGraph"+to_string(iFile)+".dot");
	                        iFile++;
	                    }
	                    else if(c == 'c')
	                    {
                            OUTDEBUG("Hoping to the next conflict");
                        }
	                	else if(c == 't')
	                		m_interact = false;
	                    else
	                    	OUTERROR("Option does not exist");
	                }

                    return false;
                }
            }
        }
    }

    return m_unsatClauses.empty();
}

double SATSolverCL::getVarScores(int p_var)
{
    return m_varScores[p_var];
}
