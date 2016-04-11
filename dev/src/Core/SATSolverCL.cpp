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
        OUTDEBUG("Backtracking");
        bool lastBetFound = false;

        std::queue<decision> empty;
        std::swap(m_deductionQueue, empty );

        m_conflictGraph.remove_node(make_pair(m_currentAssignement.back().index,!m_currentAssignement.back().value));

        while(!m_currentAssignement.empty() && !lastBetFound)
        {
            decision toCancel = m_currentAssignement.back();
            lastBetFound = toCancel.bet;
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

                m_currLevel--;
                m_conflictGraph.add_node(make_pair(make_pair(m_currentAssignement.back().index,m_currentAssignement.back().value),m_currLevel));
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


decision SATSolverCL::takeABet()
{
    decision bet = m_betHeuristic->takeABet(m_clauses, m_unsatClauses, m_valuation);
    m_currentAssignement.push_back(bet);

    m_currLevel++;
    m_conflictGraph.add_node(make_pair(make_pair(bet.index,bet.value),m_currLevel));
    return bet;
} // decision takeABet()




/*pair<vector<pair<int,Clause>>,bool> SATSolverCL::clauseForConflict(set<int>& old)
{
    map<node,set<node>> g = m_conflictGraph.get_graph();
    set<node> nodes;
    for(auto a: g)
        nodes.insert(a.first);
    vector<pair<int,Clause>> toReturn;
    map<int,int> curr_map;
    for(auto& n : nodes)
        curr_map[n.first] = n.second;
    bool conflictFound = false;
    for(auto clause : m_clauses)
    {
        if(old.find(clause.getId()) == old.end())
        {
            int nbIn = clause.getLiterals().size();
            pair<int,bool> theL;

            for(auto l : clause.getLiterals())
            {
                if(curr_map.find(l.first) == curr_map.end())
                    curr_map[l.first] = -1;
                if(nodes.find(make_pair(l.first,l.second)) != nodes.end())
                    nbIn--;
                else
                    theL = l;
            }
           
            //printf("%d %s\n", nbIn, clause.toStr().c_str());
            
            if(nbIn == 1 && !clause.evaluate(curr_map))
            {
                bool oldEnough = true;
                for(auto l : clause.getLiterals())
                    oldEnough = oldEnough && (m_levelOfVar[theL.first] >= m_levelOfVar[l.first]);
                if(oldEnough)
                {
                    old.insert(clause.getId());
                    toReturn.push_back(make_pair(theL.first,clause));
                }
            }
            if(nbIn == 0)
            {
                old.insert(clause.getId());
                int maxLevel = 0;
                int iMaxL;
                for(auto l: clause.getLiterals())
                {
                    if(maxLevel < m_levelOfVar[l.first])
                    {
                        maxLevel = m_levelOfVar[l.first];
                        iMaxL = l.first;
                    }
                }
                conflictFound = true;
                printf("C %d %s\n", iMaxL, clause.toStr().c_str());
                toReturn.push_back(make_pair(iMaxL,clause));
                return make_pair(toReturn,conflictFound);
            }
        }
    }   

    return make_pair(toReturn,conflictFound);
}
void SATSolverCL::constructConflictGraph()
{
    static int i = 0;
    m_conflictGraph.clear();
    
    int levelMax = -1;
    for(unsigned int i = 0 ; i < m_currentAssignement.size() ; i++)
        if(m_currentAssignement[i].bet || m_currentAssignement[i].ancien_bet)
            levelMax++;
           
    m_conflictGraph.levelMax = levelMax;

    int level = -1;
    for(unsigned int i = 0 ; i < m_currentAssignement.size() ; i++)
    {
        if(m_currentAssignement[i].bet || m_currentAssignement[i].ancien_bet)
        {
            level++;
            m_conflictGraph.add_node(make_pair(make_pair(m_currentAssignement[i].index,m_currentAssignement[i].value),level));

        }
        m_levelOfVar[m_currentAssignement[i].index] = level;
    }
    printf("\n");

    set<int> old;
    auto res = clauseForConflict(old);
    auto pair_clause_i = res.first; 
    bool conflictFound = res.second;
    int k = 0;
    while(!pair_clause_i.empty() && !conflictFound)
    {
        conflictFound = res.second;
        for(auto& p : pair_clause_i)
        {
            int index = p.first;
            Clause c = p.second;
            m_conflictGraph.add_node(make_pair(make_pair(index, !c.getLiterals()[index]),m_levelOfVar[index]));
            printf("%d %d %d %s\n", index, !c.getLiterals()[index], m_levelOfVar[index], c.toStr().c_str());
            for(auto& parent: c.getLiterals())
                if(parent.first != index)
                    m_conflictGraph.add_edge(make_pair(parent.first,parent.second), make_pair(index,!c.getLiterals()[index]));
        }

        res = clauseForConflict(old);
        pair_clause_i = res.first; 
    }

    map<node, bool> a;
    m_conflictGraph.output("seum.dot", make_pair(0,0), a, 0);
    OUTERROR("FINI\n");

    node uip;
    int conflictIndex;
    m_conflictGraph.getUIP(uip, conflictIndex);
    map<node, bool> uipCut;
    m_conflictGraph.getUIPCut(uipCut, uip);*/

    /* 

        HERE Alexy
    */
 /*   
    m_conflictGraph.output("conflictGraph"+to_string(i)+".dot", uip, uipCut, conflictIndex);
    i++;
}*/

bool SATSolverCL::applyLastDecision()
{
    static int iFile = 0;

    if(m_currentAssignement.empty())
        return false;

    decision p_dec = m_currentAssignement.back();

    OUTDEBUG("Handling " << ((p_dec.bet) ? string("bet") : string("deduction")) << ": "
            << p_dec.index << " to " << ((p_dec.value) ? string("True") : string("False")));
    
    if(m_parentsOf.find(p_dec.index) != m_parentsOf.end())
    {
        m_conflictGraph.add_node(make_pair(make_pair(p_dec.index,p_dec.value),m_currLevel));
        //printf("Clause for %d %d %s\n",p_dec.index, m_parentsOf[p_dec.index], m_clauses[m_parentsOf[p_dec.index]].toStr().c_str());
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
                    OUTDEBUG("\tDeducing (unit prop) : " << remainingVar << " to " << ((!pol) ? "True" : "False") << " from " << m_clauses[iClause].toStr());
                    //printf("Clause %d\n", iClause);
                    if(m_parentsOf.find(remainingVar) == m_parentsOf.end())
                    	m_parentsOf[remainingVar] = iClause;
                }
                m_isContradictory = m_aliveVarsIn[iClause].size() == 0;
                if(m_isContradictory)
                {
                	m_conflictGraph.add_node(make_pair(make_pair(p_dec.index,!m_valuation[p_dec.index]),m_currLevel));
                	for(auto l : m_clauses[iClause].getLiterals())
                		if(l.first != p_dec.index)
                			m_conflictGraph.add_edge(make_pair(l.first,m_valuation[l.first]),make_pair(p_dec.index,!m_valuation[p_dec.index]));


                    OUTDEBUG("\tContradiction spotted! : " << m_clauses[iClause].toStr());
                    printf("Contradiction spotted\n");
                    printf("Choice [g/c/t] : ");
                    char c;
                    cin >> c;
                    if(c == 'g')
                    {
                        node uip;
                        //int conflictIndex;
                        //m_conflictGraph.getUIP(uip, conflictIndex);
                        map<node, bool> uipCut;
                        //m_conflictGraph.getUIPCut(uipCut, uip);
                        int the_bet = -1;
                        for(int i = m_currentAssignement.size()-1  ; i >= 0 ; i--)
                            if(m_currentAssignement[i].bet)
                            {
                                the_bet = m_currentAssignement[i].index;
                                break;
                            }

                    	m_conflictGraph.UIPSearch(make_pair(the_bet,m_valuation[the_bet]),make_pair(p_dec.index, m_valuation[p_dec.index]));
                        m_conflictGraph.output("conflictGraph"+to_string(iFile)+".dot", the_bet, p_dec.index);

                        OUTDEBUG("Output : " << "conflictGraph"+to_string(iFile)+".dot");
                        iFile++;
                    }
                    else
                        OUTERROR("Option does not exist");

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
