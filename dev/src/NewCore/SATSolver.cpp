// STD INCLUDES
#include <iostream>
#include <cassert>
#include <fstream>
#include <sstream>
#include <cstdio>
#include <string>
#include <vector>
#include <queue>
#include <set>
#include <map>
#include <unordered_map>

// PROJECT INCLUDES
#include "../Global/Global.h"
#include "../BETHeuristic/IBet.h"
#include "clause.h"
#include "conflict_graph.h"

// HEADER INCLUDE
#include "SATSolver.h"


/*
    Returns true if clause was unitary.
*/
bool SATSolver::add_clause(clause c, bool input)
{
    assert(c.literal.size() != 0);

    OUTDEBUG(fprintf(stderr, "Adding %s.\n", c.to_str().c_str()));

    if(input)
        for(auto l : c.literal)
            if(valuation.find(abs(l)) == valuation.end())
                    valuation[abs(l)] = -1;

    if(c.literal.size() == 1)
    {
        if(valuation[abs(c.literal[0])] != -1 && valuation[abs(c.literal[0])] != (c.literal[0] > 0))
        {
            OUTDEBUG(fprintf(stderr, "Unitary clash, %d is %d.\n", abs(c.literal[0]), valuation[abs(c.literal[0])]));
            printf("s UNSATISFIABLE\n");
            exit(0);
        }

        if(input)
            valuation[abs(c.literal[0])] = c.literal[0] > 0;//detect dummy unsat

        deduction_queue.push(make_pair(c.literal[0],-1));
        OUTDEBUG(fprintf(stderr, "\tIs unitary, becomes a deduction.\n"));
        return true;
    }



    //On s'assure l'exclusivité
    c.assoc_lit.clear();
    c.alive_lit.clear();
    c.triggers.clear();

    for(auto l : c.literal)
    {
        c.assoc_lit[l] = true;
        m_varScores[abs(l)] = 0.0;
    }

    if(settings_s.wl_s)
    {
        if(c.triggers.empty())
            for(int i = 0 ; i < 2 ; i++)
            {
                c.triggers.insert(c.literal[i]);
                OUTDEBUG(fprintf(stderr, "\t%d triggered.\n", c.literal[i]));
            }

        for(auto t : c.triggers)
            clauses_watched_by[abs(t)].insert(c.id);
    }
    else
    {
        /*
            Subtil : lorsqu'on lit l'entrée on met tout le monde dans alive_lit
            pour que les deductions unitaires de l'entrées soient traitées.
            Alors que quand on apprend une clause il faut mettre que celles vraiment en vie.
        */
        for(auto l : c.literal)
            if(input || valuation[abs(l)] == -1)
                c.alive_lit.insert(l);

        for(auto l : c.literal)
            clauses_with_var[abs(l)].insert(c.id);

    }

    OUTDEBUG(fprintf(stderr, "Clause added with id %d.\n", c.id));
    formula.push_back(c);
    unsat_clauses.insert(c.id);

    return false;
}

/*
    Returns true if deduction already done.
*/
bool SATSolver::add_pending_deduction()
{
    assert(!deduction_queue.empty());

    pair<int,int> ded_clause = deduction_queue.front();
    int ded = ded_clause.first, from_clause = ded_clause.second;
    deduction_queue.pop();

    if(valuation[abs(ded)] != -1)
    {
        OUTDEBUG(fprintf(stderr, "Deduction %d already done.", ded));
        return true;
    }

    OUTDEBUG(fprintf(stderr, "Add pending deduction %d to decision stack.\n", ded));
    decision_stack.push_back(make_pair(ded, false));
    
    if(settings_s.cl_s)
        conflict_graph.add_deduction_node(from_clause);

    return false;
}

/* 
    Returns the conflict clause's index.
*/
int SATSolver::apply_last_decision()
{
    assert(!decision_stack.empty());
    int last_decision = decision_stack.back().first;
    OUTDEBUG(fprintf(stderr, "\nHandling last decision %d%s.\n", last_decision, (decision_stack.back().second) ? "b" : "d"));
    print_current_state();
    OUTDEBUG(fprintf(stderr, "\n"));
    return (settings_s.wl_s) ? apply_last_decisionWL() : apply_last_decisionSTD();
}

int SATSolver::apply_last_decisionWL()
{
    int last_decision = decision_stack.back().first;
    valuation[abs(last_decision)] = last_decision > 0;

    int conflict_clause = -1;

    vector<pair<int,pair<int,int>>> change;
    for(auto iClause : clauses_watched_by[abs(last_decision)])
        if(unsat_clauses.find(iClause) != unsat_clauses.end())
        {
            OUTDEBUG(fprintf(stderr, "Decision affects clause %d: %s.\n", iClause, formula[iClause].to_str().c_str()));
            OUTDEBUG(fprintf(stderr, "Watched by %d and %d.\n", *formula[iClause].triggers.begin(), formula[iClause].other_trigger(*formula[iClause].triggers.begin())));

            if(formula[iClause].has(last_decision))
            {
                OUTDEBUG(fprintf(stderr, "\tClause is now sat.\n"));

                unsat_clauses.erase(iClause);
                clauses_sat_by[last_decision].push_back(iClause);
            }
            else
            {
                assert(formula[iClause].has(-last_decision));
                assert(formula[iClause].triggers.find(-last_decision) != formula[iClause].triggers.end());

                bool trigger_moved = false;

                //Clause already sat
                for(auto l : formula[iClause].literal)
                    if(valuation[abs(l)] != -1 && valuation[abs(l)] == (l > 0))
                    {
                        assert(!formula[iClause].has_trigger(l));
                        OUTDEBUG(fprintf(stderr, "\tClause was already sat by: %d.\n", l));

                        unsat_clauses.erase(iClause);
                        clauses_sat_by[l].push_back(iClause);
                        formula[iClause].triggers.erase(-last_decision);
                        formula[iClause].triggers.insert(l);
                        change.push_back(make_pair(iClause,make_pair(-last_decision,l)));//Clause, Old, New
                        trigger_moved = true;
                        OUTDEBUG(fprintf(stderr, "Now watched by %d and %d.\n",  *formula[iClause].triggers.begin(), formula[iClause].other_trigger(*formula[iClause].triggers.begin())));
                        break;
                    }
                
                //Free spot for trigger
                if(!trigger_moved)
                {
                    for(auto l : formula[iClause].literal)
                        if(!formula[iClause].has_trigger(l) && valuation[abs(l)] == -1)
                        {
                            OUTDEBUG(fprintf(stderr, "\tTrigger %d moved to %d.\n", -last_decision, l));
                            formula[iClause].triggers.erase(-last_decision);
                            formula[iClause].triggers.insert(l);
                            change.push_back(make_pair(iClause,make_pair(-last_decision,l)));//Clause, Old, New
                            trigger_moved = true;
                            OUTDEBUG(fprintf(stderr, "Now watched by %d and %d.\n",  *formula[iClause].triggers.begin(), formula[iClause].other_trigger(*formula[iClause].triggers.begin())));
                            break;
                        }
                }

                //Unit prop or bt
                if(!trigger_moved)
                {
                    int other_trigger = formula[iClause].other_trigger(-last_decision);

                    if(valuation[abs(other_trigger)] == -1)
                    {
                        OUTDEBUG(fprintf(stderr, "\tUnit prop %d spotted.\n", other_trigger));
                        deduction_queue.push(make_pair(other_trigger,iClause));
                        continue;
                    }
                    else
                    {
                        OUTDEBUG(fprintf(stderr, "\tContradiction %d spotted.\n", -last_decision));
                        conflict_clause = iClause;
                        break;
                    }

                }

            }

            OUTDEBUG(fprintf(stderr, "\n"));
        }

    OUTDEBUG(fprintf(stderr, "Make trigger changes effective.\n"));

    for(auto c: change)
    {
        int iClause = c.first;
        int old_watcher = c.second.first;
        int new_watcher = c.second.second;

        OUTDEBUG(fprintf(stderr, "\tClause %d: %s was watched by %d and now %d\n", iClause, formula[iClause].to_str().c_str(), old_watcher, new_watcher));

        old_watcher = abs(old_watcher);
        new_watcher = abs(new_watcher);
        assert(clauses_watched_by[old_watcher].find(iClause) != clauses_watched_by[old_watcher].end());//Costly assert
        clauses_watched_by[old_watcher].erase(iClause);
        assert(clauses_watched_by[new_watcher].find(iClause) == clauses_watched_by[new_watcher].end());//Costly assert
        clauses_watched_by[new_watcher].insert(iClause);
    }

    OUTDEBUG(fprintf(stderr, "\n"));
    return conflict_clause;
}

int SATSolver::apply_last_decisionSTD()
{
    int last_decision = decision_stack.back().first;
    valuation[abs(last_decision)] = last_decision > 0;

    int conflict_clause = -1;
    for(auto iClause : clauses_with_var[abs(last_decision)])
    {
        if(unsat_clauses.find(iClause) != unsat_clauses.end())
        {
            OUTDEBUG(fprintf(stderr, "Decision affects clause %d: %s.\n", iClause, formula[iClause].to_str().c_str()));
            if(formula[iClause].has(last_decision))
            {
                OUTDEBUG(fprintf(stderr, "\tClause is now sat.\n"));

                unsat_clauses.erase(iClause);
                clauses_sat_by[last_decision].push_back(iClause);
            }
            else
            {
                assert(formula[iClause].has(-last_decision));
                if(formula[iClause].alive_lit.erase(-last_decision))
                {
                    if(formula[iClause].alive_lit.size() == 1)
                    {
                        OUTDEBUG(fprintf(stderr, "\tUnit prop %d spotted.\n", *formula[iClause].alive_lit.begin()));
                        deduction_queue.push(make_pair(*formula[iClause].alive_lit.begin(),iClause));
                        continue;
                    }
                    
                    if(formula[iClause].alive_lit.size() == 0)
                    {
                        OUTDEBUG(fprintf(stderr, "\tContradiction %d spotted.\n", -last_decision));
                        conflict_clause = iClause;
                        break;
                    }
                }
                else
                    assert(false);
            }

            OUTDEBUG(fprintf(stderr, "\n"));
        }
    }

    OUTDEBUG(fprintf(stderr, "\n"));
    return conflict_clause;
}

/*
    Les litéraux de niveaux -1 sont flushés des clauses apprises.
*/
pair<clause,int> SATSolver::diagnose_conflict(int conflict_clause)
{
    int bt_to = -1;//Where bt should stop inclusively
    if(!settings_s.cl_s)
    {
        for(int i = decision_stack.size()-1 ; i >= 0 ; i--)
        {
            if(decision_stack[i].second)
            {
                bt_to = decision_stack[i].first;
                OUTDEBUG(fprintf(stderr, "\tShould backtrack until %d (last bet) inclusively.\n", bt_to));
                return make_pair(clause(-1), bt_to);
            }
        }
    }

    printf("%d\n", curr_level);
    assert(settings_s.cl_s);
    OUTDEBUG(fprintf(stderr, "Diagnosising conflict.\n"));

    conflict_graph.add_deduction_node(conflict_clause, true);

    clause to_learn(formula.size());
    vector<int> level;

    //conflict_graph.output();
    conflict_graph.find_uip_cut();

    OUTDEBUG(fprintf(stderr, "\tUIP found to be %d.\n", conflict_graph.uip));
    OUTDEBUG(fprintf(stderr, "\tUIP cut found to be: \n"));
    for(auto n : conflict_graph.in_uip_cut)
        OUTDEBUG(fprintf(stderr, "\t\t%d\n", n.first));

    for(auto n : conflict_graph.sons_of)
    {
        if(conflict_graph.infos_on[n.first].first != -1 && conflict_graph.in_uip_cut.find(n.first) == conflict_graph.in_uip_cut.end())
        {
            for(auto v : conflict_graph.sons_of[n.first])
                if(conflict_graph.in_uip_cut.find(v) != conflict_graph.in_uip_cut.end())
                {                    
                    to_learn.literal.push_back(-n.first);
                    to_learn.assoc_lit[-n.first] = true;
                    level.push_back(conflict_graph.infos_on[n.first].first);
                    break;
                }
        }
    }

    OUTDEBUG(fprintf(stderr, "\tWill learn clause %s.\n", to_learn.to_str().c_str()));
    
    if(settings_s.clinterac_s)
    {
        bool ok = false;
        while(!ok)
        {
            cout << "Conflict spotted!" << endl << "g: output conflict graph" << endl << "c: continue to next conflict" << endl << "t: disable interaction" << endl;
            char choice;
            cin >> choice;

            if(choice == 'g')
            {
                cout << "Outputing graph" << endl;
                conflict_graph.output(true);
                ok = true;
            }
            else if(choice == 't')
            {
                cout << "Interaction disables" << endl;
                settings_s.clinterac_s = false;
                ok = true;
            }
            else
            {
                if(choice != 'c')
                    cout << "Wrong input, pleace select again" << endl;
                else
                {
                    cout << "Jump to next conflict" << endl;
                    ok = true;
                }
            }
        }
    }

    assert(to_learn.assoc_lit.find(-conflict_graph.uip) != to_learn.assoc_lit.end());


    int good_trig = 0;
    if(to_learn.literal.size() == 1)
    {
        for(unsigned int i = 0 ; i < decision_stack.size() ; i++)
        {
            bt_to = decision_stack[i].first;
            if(decision_stack[i].second)
                break;
        }
    }
    else
    {
        bt_to = 0;
        good_trig = 0;
        for(int i = decision_stack.size()-1 ; i >= 0 ; i--)
        {
            int dec = decision_stack[i].first;
            if(dec != conflict_graph.uip && to_learn.assoc_lit.find(-dec) != to_learn.assoc_lit.end())
            {
                for(int j = i ; j >= 0 ; j--)
                {
                    if(to_learn.assoc_lit.find(-decision_stack[j].first) != to_learn.assoc_lit.end())
                        good_trig = decision_stack[j].first;
                    if(decision_stack[j].second)
                    {
                        bt_to = decision_stack[j].first;
                        break;
                    }
                }
            }
        }
    }

    assert(bt_to != 0);

    OUTDEBUG(fprintf(stderr, "\tShould backtrack until %d inclusively.\n", bt_to));

    if(settings_s.wl_s && to_learn.literal.size() != 1)
    {
        //The good triggers to have
        to_learn.triggers.insert(-good_trig);
        to_learn.triggers.insert(-conflict_graph.uip);

        /*for(auto d : decision_stack)
            fprintf(stderr, "%d%s ", d.first, (d.second) ? "b" : "d");*/
        OUTDEBUG(fprintf(stderr, "\tLearnt clause %s watched by %d (bt to) and %d (uip).\n", to_learn.to_str().c_str(), -bt_to, -conflict_graph.uip));
    }
    return make_pair(to_learn,bt_to);
}

/*
    Retourne la dernière decision écrasée.
    Dans le cas CL:
        Si full_bt n'est pas à vrai on laisse le noeud associé dans le graphe, on enleve juste ses fils.

*/
pair<int,bool> SATSolver::backtrack(int bt_to, bool full_bt)
{
    assert(!decision_stack.empty());

    OUTDEBUG(fprintf((stderr), "Backtracking until %d inclusively.\n", bt_to));
    print_current_state();

    OUTDEBUG(fprintf(stderr, "\tClearing pending deductions.\n\t"));
    while(!deduction_queue.empty())
        deduction_queue.pop();

    if(settings_s.cl_s)
        conflict_graph.remove_node(conflict_graph.conflict_literal);

    pair<int,pair<int,bool>> last_node;

    while(!decision_stack.empty())
    {
        int toCancel = decision_stack.back().first;
        bool bet = decision_stack.back().second;
        decision_stack.pop_back();

        OUTDEBUG(fprintf(stderr, "Cancelling %d%s.\n", toCancel, (bet) ? "b" : "d"));
        valuation[abs(toCancel)] = -1;

        if(!settings_s.wl_s)
        {
            OUTDEBUG(fprintf(stderr, "\tReviving var in appropriate clauses.\n"));

            for(auto iClause : clauses_with_var[abs(toCancel)])
            {
                int lit_in_clause = (formula[iClause].has(toCancel)) ? toCancel : -toCancel;
                formula[iClause].alive_lit.insert(lit_in_clause);
            }
        }

        if(bet)
        {
            curr_level--;
            OUTDEBUG(fprintf(stderr, "\tCurrent level is now %d.\n", curr_level));
        }

        OUTDEBUG(fprintf(stderr, "\tReviving clauses satisfied by %d.\n", toCancel));
        for(auto iClause : clauses_sat_by[toCancel])
        {
            OUTDEBUG(fprintf(stderr, "\t\t%s revived.\n", formula[iClause].to_str().c_str()));
            unsat_clauses.insert(iClause);
        }
        clauses_sat_by[toCancel].clear();

        if(toCancel == bt_to)
        {
            if(settings_s.cl_s)
                conflict_graph.remove_node(toCancel, full_bt);
            
            print_current_state();
            return make_pair(toCancel,bet);
        }

        OUTDEBUG(fprintf(stderr, "\t"));
        if(settings_s.cl_s)
            conflict_graph.remove_node(toCancel);
    }
    return make_pair(-1, false);
}

void SATSolver::take_a_bet()
{
    ++curr_level;
    m_bet->takeABet(this);
}

void SATSolver::reset_valuation()
{
    for(auto& a : valuation)
        a.second = -1;
}

bool SATSolver::solve()
{
    reset_valuation();//No unitary clash found in input

    bool is_unsat = false;
    bool jump = false;

    while(!unsat_clauses.empty() && !is_unsat)
    {
        OUTDEBUG(fprintf(stderr, "\nIteration %d.\n", iter));
        print_current_state();
        iter++;

        /*
            After a backtrack we might want to jump over this step
            to re-apply last backtrack cancelled decision.
            But, if we learned an unitary clause it is unecessary as it goes into pending deductions.
        */
        if(!jump)
        {
            if(deduction_queue.empty())
                take_a_bet();
            else if(add_pending_deduction())
                continue;
        }

        jump = false;
        int conflict_clause = apply_last_decision();
        if(conflict_clause != -1)
        {
            if(curr_level == -1)
            {
                is_unsat = true;
                continue;
            }

            pair<clause,int> diagnosis = diagnose_conflict(conflict_clause);//Learnt clause, bt level
            pair<int,bool> last_dec = backtrack(diagnosis.second, diagnosis.first.literal.size() == 1);
            if(!settings_s.cl_s || !add_clause(diagnosis.first))//Added clause wasn't unitary, no deduction added
            {
                assert(last_dec.second);
                if(settings_s.cl_s)
                    curr_level++;
                else
                {
                    last_dec.first *= -1;
                    last_dec.second = false;
                }
                decision_stack.push_back(last_dec);
                //conflict_graph.output();
                jump = true;
            }
        }
    }

    return is_unsat;
}

void SATSolver::print_current_state()
{
    OUTDEBUG(fprintf(stderr, "Current state: "));
    for(auto d : decision_stack)
        OUTDEBUG(fprintf(stderr, "%d%s ", d.first, (d.second) ? "b" : "d"));
    OUTDEBUG(fprintf(stderr, "\n"));
}

void SATSolver::getStrFormula()
{
    for(auto c : formula)
    {
        for(int l : c.literal)
            cout << l << " ";
        cout << endl;
    }
}

void SATSolver::setBet(IBet *p_bet)
{
    m_bet = p_bet;
}

double SATSolver::getVarScores(int p_var)
{
    if(m_varScores.find(abs(p_var)) != m_varScores.end())
        return m_varScores[abs(p_var)];
    else 
        return 0;
}

double SATSolver::VSIDSScoreFunction(double oldScore, bool inLearnedClause)
{
    double incrementConstant = 1;
    double pondConstant =  0.95;//What is in minisat according to http://www.cs.tau.ac.il/research/alexander.nadel/SAT-05_CBH_2.pdf
    if(inLearnedClause)
    {
        return (oldScore + incrementConstant);
    }
    else
    {
        return oldScore*pondConstant;
    }
}
