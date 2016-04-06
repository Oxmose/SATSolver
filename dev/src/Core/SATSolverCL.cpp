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
                //If we are on the last bet we turn it into a deduction and change it
                m_currentAssignement.back().value = !m_currentAssignement.back().value;
                m_currentAssignement.back().bet = false;
                m_currentAssignement.back().ancien_bet = true; 
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


vector<pair<int,Clause>> SATSolverCL::clauseForConflict(const set<node>& nodes, set<int>& old)
{
    vector<pair<int,Clause>> toReturn;
    map<int,int> curr_map;
    for(auto& n : nodes)
        curr_map[n.first] = n.second;
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
                printf("%d %s\n", theL.first, clause.toStr().c_str());
                old.insert(clause.getId());
                toReturn.push_back(make_pair(theL.first,clause));
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
                toReturn.push_back(make_pair(iMaxL,clause));
            }
        }
    }   

    return toReturn;
}
void SATSolverCL::constructConflictGraph()
{
    static int i = 0;
    m_conflictGraph.clear();
    
    int levelMax = -1;
    for(int i = 0 ; i < m_currentAssignement.size() ; i++)
        if(m_currentAssignement[i].bet || m_currentAssignement[i].ancien_bet)
            levelMax++;
           
    m_conflictGraph.levelMax = levelMax;

    int level = -1;
    for(int i = 0 ; i < m_currentAssignement.size() ; i++)
    {
        m_levelOfVar[m_currentAssignement[i].index] = level;
        if(m_currentAssignement[i].bet || m_currentAssignement[i].ancien_bet)
        {
            level++;
            m_conflictGraph.add_node(make_pair(make_pair(m_currentAssignement[i].index,m_currentAssignement[i].value),level));
        }
    }
    printf("\n");

    set<int> old;
    auto pair_clause_i = clauseForConflict(m_conflictGraph.get_nodes(), old); 
    while(!pair_clause_i.empty())
    {
        for(auto& p : pair_clause_i)
        {
            int index = p.first;
            Clause c = p.second;
            m_conflictGraph.add_node(make_pair(make_pair(index, !c.getLiterals()[index]),m_levelOfVar[index]));
            for(auto& parent: c.getLiterals())
                if(parent.first != index)
                    m_conflictGraph.add_edge(make_pair(parent.first,parent.second), make_pair(index,!c.getLiterals()[index]));
        }
        pair_clause_i = clauseForConflict(m_conflictGraph.get_nodes(), old); 
    }

    m_conflictGraph.output("conflictGraph"+to_string(i)+".dot");
    i++;
}

bool SATSolverCL::applyLastDecision()
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
                    printf("Contradiction spotted\n");
                    printf("Choice [g/c/t] : ");
                    char c = getchar();
                    if(c == 'g')
                        constructConflictGraph();

                    return false;
                }
            }
        }
       }

    return m_unsatClauses.empty();
}