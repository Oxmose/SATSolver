#include "SATSolver.h"
#include "SATSolverCL.h"
#include "ConflictGraph.h"

using namespace std;

SATSolverCL::SATSolverCL(const bool &p_naiveuip, const bool &p_interact, const bool &p_forget, const bool &p_vsids, function<double(double, bool)> p_scoreFunction)
{
    OUTDEBUG("Using Clause Learning Solver, " << string((p_interact ? "with interaction, " : "without interact, ")) << string((p_forget ? "with cutoff threshold to forget clauses." : "without cutoff threshold to forget clauses.")));
    m_isCL = true;
    m_vsids = p_vsids;
    m_currLevel = -1;
    m_btLevel = -1;
    m_conflictGraph.clear();

    m_interact = p_interact;
    m_forget = p_forget;
    m_naiveuip = p_naiveuip;

    m_scoreFunction = p_scoreFunction;
}

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
            if(m_vsids)
            	m_varScores[lit.first] = 0.0;
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
        	OUTDEBUG("Free " << dec.index);
        	m_parentsOf.erase(dec.index);
        	m_deductionQueue.pop();
        }

        m_conflictGraph.remove_node(make_pair(m_currentAssignement.back().index,!m_currentAssignement.back().value));
        bool hasFoundBet = false;
        while(!m_currentAssignement.empty() && !lastBetFound)
        {
            decision toCancel = m_currentAssignement.back();
            lastBetFound = toCancel.bet && (m_currLevel == m_btLevel);
            if(m_currLevel == -1 && hasFoundBet)
            	break;
            if(toCancel.bet)
            {
            	hasFoundBet = true;
            	m_bets.pop_back();
            	m_currLevel--;
            }
            else
            	m_parentsOf.erase(toCancel.index);
            
            	

            reviveClauseWithSatisfier(toCancel.index);
            m_conflictGraph.remove_node(make_pair(toCancel.index,toCancel.value));
            for(int iClause : m_clausesWithVar[toCancel.index])
                m_aliveVarsIn[iClause].insert(toCancel.index);

            m_valuation[toCancel.index] = -1;
            if(!lastBetFound)
                m_currentAssignement.pop_back();
            else
            {
                //If we are on a bet we turn it into a deduction and change it
                //m_currentAssignement.back().value = !m_currentAssignement.back().value;
                //m_currentAssignement.back().bet = false;
                //m_currentAssignement.back().ancien_bet = true; 
        	m_currLevel++;
        	m_bets.push_back(m_currentAssignement.back().index);
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

        OUTDEBUG(currentStateToStr());
    	return addResolutionClause();
    	
    }
    return false;
} // bool backtrack(bool&)


decision SATSolverCL::takeABet()
{
    decision bet = m_betHeuristic->takeABet(m_clauses, m_unsatClauses, m_valuation);
    m_currentAssignement.push_back(bet);

    m_currLevel++;
    m_conflictGraph.add_node(make_pair(make_pair(bet.index,bet.value),m_currLevel));

    m_bets.push_back(bet.index);
    return bet;
} // decision takeABet()

bool SATSolverCL::addResolutionClause()
{
	if(m_resolutionClause.getLiterals().size() == 1)
    {
    	int index = m_resolutionClause.getLiterals().begin()->first;
    	bool value = !m_resolutionClause.getLiterals().begin()->second;
		m_preprocessQueue.push(decision(index,value,false));
    	m_parentsOf[index] = -1;
    	OUTDEBUG("Resolution clause " << m_resolutionClause.toStr() << " added in preprocess");
    	return true;
    }
   

	m_clauses.push_back(m_resolutionClause);
	m_unsatClauses.insert(m_resolutionClause.getId());

	int iClause = m_resolutionClause.getId();

    for(auto lit : m_clauses[iClause].getLiterals())
    {

        m_clausesWithVar[lit.first].push_back(iClause);
        if(m_valuation[lit.first] == -1)
        	m_aliveVarsIn[iClause].insert(lit.first);
        //cout << "HERE " << lit.first << " " << m_varScores[lit.first] << " " << m_scoreFunction(m_varScores[lit.first],true) << endl;
        if(m_vsids && m_resolutionClause.getLiterals().size() != 1)
        	m_varScores[lit.first] = m_scoreFunction(m_varScores[lit.first],true);
    }

    if(m_vsids && m_resolutionClause.getLiterals().size() != 1)
    {
    	for(auto l : m_varScores)
    		if(m_clauses[iClause].getLiterals().find(l.first) == m_clauses[iClause].getLiterals().end())
    			m_varScores[l.first] = m_scoreFunction(m_varScores[l.first],false);
    }

    if(m_currLevel == -1)
    {
    	for(auto a : m_currentAssignement)
    	{
    		m_preprocessQueue.push(a);
    		m_valuation[a.index] = -1;
    	}
    	m_currentAssignement.clear();
    }
    OUTDEBUG("Resolution clause " << m_resolutionClause.toStr() << " added");
    return !(m_currLevel == -1);
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
    OUTDEBUG("GraphSize " << m_conflictGraph.get_graph().size());
    //OUTDEBUG(m_deductionQueue.size());
    iter++;
    OUTDEBUG(currentStateToStr());
    OUTDEBUG("Handling " << ((p_dec.bet) ? string("bet") : string("deduction")) << ": "
            << p_dec.index << " to " << ((p_dec.value) ? string("True") : string("False")));
    
    if(!p_dec.bet)
    {
    	m_conflictGraph.add_node(make_pair(make_pair(p_dec.index,p_dec.value),m_currLevel));

    	if(m_parentsOf[p_dec.index] != -1)
    	{
    		OUTDEBUG("Coming from " << m_clauses[m_parentsOf[p_dec.index]].toStr() << " " << m_parentsOf[p_dec.index]);
        
        	//printf("Clause for %d %d %s\n",p_dec.index, m_parentsOf[p_dec.index], m_clauses[m_parentsOf[p_dec.index]].toStr().c_str());
        	if(m_parentsOf.find(p_dec.index) != m_parentsOf.end())
        		for(auto l : m_clauses[m_parentsOf[p_dec.index]].getLiterals())
            		if(l.first != p_dec.index)
                		m_conflictGraph.add_edge(make_pair(l.first,m_valuation[l.first]),make_pair(p_dec.index,p_dec.value));
        	//m_parentsOf.erase(p_dec.index);
        }
    }
    //OUTDEBUG("\t" << currentStateToStr());
    
    m_valuation[p_dec.index] = p_dec.value;

    m_isContradictory = false;


    for(int iClause : m_clausesWithVar[p_dec.index])
    {
        if(m_unsatClauses.find(iClause) != m_unsatClauses.end())
        {
        	/*printf("%s %d\n", m_clauses[iClause].toDIMACS().c_str(), m_aliveVarsIn[iClause].size());
        	for(auto a : m_aliveVarsIn[iClause])
        		printf("\t%d\n",a);*/
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
                    int the_bet = m_bets.back();
                    /*for(int i = m_currentAssignement.size()-1  ; i >= 0 ; i--)
                    {
                    	cout << "\t" << m_currentAssignement[i].index << ((m_currentAssignement[i].bet) ? " b" : "") << endl;
                        if(m_currentAssignement[i].bet)
                        {
                            the_bet = m_currentAssignement[i].index;
                            break;
                        }
                    }*/

                	pair<Clause,int> whatINeed = m_conflictGraph.resolution(m_naiveuip, make_pair(the_bet,m_valuation[the_bet]),make_pair(p_dec.index, m_valuation[p_dec.index]), m_clauses.size());
                    //printf("%s\n", whatINeed.first.toDIMACS().c_str());
                    //printf("%d\n", whatINeed.second);
                    m_btLevel = whatINeed.second;
                    m_resolutionClause = whatINeed.first;
                    if(m_learnedClauses.find(whatINeed.first.toDIMACS().c_str()) == m_learnedClauses.end())
                    	m_learnedClauses[whatINeed.first.toDIMACS().c_str()] = 1;
                    else
                    {
                    	m_learnedClauses[whatINeed.first.toDIMACS().c_str()]++;
                    }


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
	                        cout << "Output : " << "conflictGraph"+to_string(iFile)+".dot" << endl;
	                        //OUTDEBUG("Output : " << "conflictGraph"+to_string(iFile)+".dot");
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

	                if(whatINeed.first.getLiterals().size() == 1)
	                {
	                	
	                	for(int i = 0 ; i < m_currentAssignement.size() ; i++)
	                	{
	                		if(m_currentAssignement[i].bet)
	                			break;
	                	
	                		if(m_currentAssignement[i].index == whatINeed.first.getLiterals().begin()->first && m_currentAssignement[i].value == whatINeed.first.getLiterals().begin()->second)
	                		{
	                			printf("s UNSATISFIABLE\n");
	                			exit(0);
	                		}
	                	}
	                	m_conflictGraph.remove_node(make_pair(m_currentAssignement.back().index,!m_currentAssignement.back().value));

	                	while(m_currLevel != -1)
	                	{
	                		if(m_currentAssignement.back().bet)
	                			m_currLevel--;
	                		else
	                			m_parentsOf.erase(m_currentAssignement.back().index);
	                		
	                		auto toCancel = m_currentAssignement.back();
	                		reviveClauseWithSatisfier(m_currentAssignement.back().index);
            				m_conflictGraph.remove_node(make_pair(toCancel.index,toCancel.value));

            				for(int iClause : m_clausesWithVar[m_currentAssignement.back().index])
                				m_aliveVarsIn[iClause].insert(m_currentAssignement.back().index);
	                		m_valuation[m_currentAssignement.back().index] = -1;
	                		m_currentAssignement.pop_back();
	                	}
	                	
	                	while(!m_deductionQueue.empty())
	                	{
	                		m_parentsOf.erase(m_deductionQueue.front().index);
	                		m_deductionQueue.pop();
	                	}
	                	
	                	
	                	addResolutionClause();
	                	m_isContradictory = false;
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
    if(m_varScores.find(p_var) != m_varScores.end())
    	return m_varScores[p_var];
    else 
        return 0;
}
