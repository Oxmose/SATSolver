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

SATSolver::SATSolver() : conflict_graph(this)
{
    curr_level = -1;
    iter = 0; 
    smt_solver = NULL;

    if(settings_s.smte_s)
        smt_solver = new SMTSolver_eq(this);
}


SATSolver::~SATSolver()
{
    for(auto e : dpll_to_smt)
        delete e.second;
}

/*
    Returns true if clause was unitary.
*/
bool SATSolver::add_clause(clause c, bool input/*=false*/)
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

    for(auto l : c.literal)
    {
        c.assoc_lit[l] = true;
        //m_varScores[abs(l)] = 0.0; SURTOUT PAS, annule la MAJ et pas un soucis au debut
        clauses_with_var[abs(l)].insert(c.id);
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
    decision_stack.push_back(decision(ded, curr_level, false));
    
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
    int last_decision = decision_stack.back().dec;
    OUTDEBUG(fprintf(stderr, "\nHandling last decision %d%s.\n", last_decision, (decision_stack.back().bet) ? "b" : "d"));
    print_current_state();
    OUTDEBUG(fprintf(stderr, "\n"));

    return (settings_s.wl_s) ? apply_last_decisionWL() : apply_last_decisionSTD();
}

int SATSolver::apply_last_decisionWL()
{
    int last_decision = decision_stack.back().dec;
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
    int last_decision = decision_stack.back().dec;
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
    int bt_level = 0;//Where bt should stop inclusively
    if(!settings_s.cl_s)
    {
        bt_level = curr_level;
        OUTDEBUG(fprintf(stderr, "\tShould backtrack until level %d (last bet) inclusively.\n", bt_level));
        return make_pair(clause(-1), bt_level);
    }


    assert(settings_s.cl_s);
    OUTDEBUG(fprintf(stderr, "Diagnosising conflict.\n"));
    conflict_graph.add_deduction_node(conflict_clause, true);

    clause to_learn(formula.size());
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
                    if(n.first != conflict_graph.uip)
                        bt_level = max(bt_level, conflict_graph.infos_on[n.first].first);
                    break;
                }
        }
    }

    OUTDEBUG(fprintf(stderr, "\tWill learn clause %s.\n", to_learn.to_str().c_str()));

    assert(to_learn.assoc_lit.find(-conflict_graph.uip) != to_learn.assoc_lit.end());


    int good_trig = 0;
    if(to_learn.literal.size() != 1)
    {
        good_trig = 0;
        for(int i = decision_stack.size()-1 ; i >= 0 ; i--)
        {
            int dec = decision_stack[i].dec;
            if(dec != conflict_graph.uip && to_learn.assoc_lit.find(-dec) != to_learn.assoc_lit.end())
            {
                    good_trig = decision_stack[i].dec;
                    break;
            }
        }
        assert(good_trig != 0);
    }

    OUTDEBUG(fprintf(stderr, "\tShould backtrack until level %d inclusively.\n", bt_level));

    if(settings_s.bet_s == VSIDS)
    {
        set<int> toAvoid;
        for(auto l : to_learn.literal)
        {
            m_varScores[abs(l)] = VSIDSScoreFunction(getVarScores(abs(l)),true);
            toAvoid.insert(abs(l));
        }
        for(unsigned int iClause = 0 ; iClause < formula.size() ; iClause++)
            for(auto l :  formula[iClause].literal)
                if(toAvoid.find(abs(l)) == toAvoid.end())
                {
                    toAvoid.insert(abs(l));
                    m_varScores[abs(l)] = VSIDSScoreFunction(getVarScores(abs(l)),false);
                }
        OUTDEBUG(fprintf(stderr,"VSIDS scores updated.\n"));
        for(auto v : m_varScores)
            if(v.second != 0.0)
                OUTDEBUG(fprintf(stderr, "\tVar %d: %lf\n", v.first, v.second));
    }


    if(settings_s.clinterac_s)
    {
        bool ok = false;
        while(!ok)
        {
            cout << endl << "Conflict spotted!" << endl;
            cout << "Will learn clause: " << to_learn.to_str().c_str() << endl;
            cout << "Backtrack to level: " << bt_level << endl;
            cout << endl;
            cout << "g: output conflict graph" << endl << "c: continue to next conflict" << endl << "t: disable interaction" << endl;
            char choice;
            cin >> choice;

            if(choice == 'g')
            {
                //cout << "Outputing graph" << endl; done by output
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

    if(settings_s.wl_s && to_learn.literal.size() != 1)
    {

        //The good triggers to have
        to_learn.triggers.insert(-good_trig);
        to_learn.triggers.insert(-conflict_graph.uip);
        /*for(auto d : decision_stack)
            fprintf(stderr, "%d%s ", d.first, (d.second) ? "b" : "d");*/
        OUTDEBUG(fprintf(stderr, "\tLearnt clause %s watched by %d and %d (uip).\n", to_learn.to_str().c_str(), -good_trig, -conflict_graph.uip));
    }
    return make_pair(to_learn,bt_level);
}

/*
    Retourne la dernière decision écrasée.
    Dans le cas CL:
        Si full_bt n'est pas à vrai on laisse le noeud associé dans le graphe, on enleve juste ses fils.

*/
decision SATSolver::backtrack(int bt_level, bool full_bt)
{
    assert(!decision_stack.empty());

    OUTDEBUG(fprintf((stderr), "Backtracking until level %d inclusively.\n", bt_level));
    print_current_state();

    OUTDEBUG(fprintf(stderr, "\tClearing pending deductions.\n\t"));
    while(!deduction_queue.empty())
        deduction_queue.pop();

    if(settings_s.cl_s)
        conflict_graph.remove_node(conflict_graph.conflict_literal);

    pair<int,pair<int,bool>> last_node;

    while(!decision_stack.empty())
    {
        int toCancel = decision_stack.back().dec;
        bool bet = decision_stack.back().bet;
        int level = decision_stack.back().level;
        decision last_dec = decision_stack.back();
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

        if(level == bt_level && bet)
        {
            if(settings_s.cl_s)
                conflict_graph.remove_node(toCancel, full_bt);
            
            print_current_state();
            return last_dec;
        }

        OUTDEBUG(fprintf(stderr, "\t"));
        if(settings_s.cl_s)
            conflict_graph.remove_node(toCancel);
    }
    return decision(-1, -2, false);
}

void SATSolver::take_a_bet()
{
    int the_bet = m_bet->takeABet(this);
    ++curr_level;
    decision_stack.push_back(decision(the_bet,curr_level,true));
    OUTDEBUG(fprintf(stderr,"Taking bet %d.\n", the_bet));
    OUTDEBUG(fprintf(stderr, "Current level is now %d.\n", curr_level));
    if(settings_s.cl_s)
        conflict_graph.add_node(the_bet, make_pair(curr_level, true));
}

void SATSolver::reset_valuation()
{
    for(auto& a : valuation)
        a.second = -1;
}

bool SATSolver::solve()
{
    for(auto c : formula)
        printf("%s\n", c.to_str().c_str());
    for(auto l : dpll_to_smt)
        printf("%d %s\n", l.first, l.second->to_str().c_str());
    for(auto l : valuation)
        printf("%d\n", l.first);
    exit(0);

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

        /* SMT */
        
        if(!smt_solver->apply_last_decision())
        {
            pair<clause,int> diagnosis = smt_solver->diagnose_conflict();
            assert(diagnosis.first.literal.size() != 1);
            decision last_dec = backtrack(diagnosis.second, false);
            add_clause(diagnosis.first);
            curr_level++;
            decision_stack.push_back(last_dec);
            jump = true;
            continue;
        }

        int conflict_clause = apply_last_decision();
        if(conflict_clause != -1)
        {
            if(curr_level == -1)
            {
                is_unsat = true;
                continue;
            }

            pair<clause,int> diagnosis = diagnose_conflict(conflict_clause);//Learnt clause, bt level
            decision last_dec = backtrack(diagnosis.second, diagnosis.first.literal.size() == 1);
            if(!settings_s.cl_s || !add_clause(diagnosis.first))//Added clause wasn't unitary, no deduction added
            {
                assert(last_dec.bet);
                if(settings_s.cl_s)
                    curr_level++;
                else
                {
                    last_dec.dec *= -1;
                    last_dec.bet = false;
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
        OUTDEBUG(fprintf(stderr, "%d%s ", d.dec, (d.bet) ? "b" : "d"));
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

void SATSolver::emplace_eq(unsigned int var, struct smt_literal *eq)
{
    struct smt_literal_eq *eqi = (struct smt_literal_eq *)eq;
	if(eqi->equal)
        OUTDEBUG(fprintf(stderr, "Adding equality %d = %d with var %d\n", eqi->left, eqi->right, var));
    else
        OUTDEBUG(fprintf(stderr, "Adding non equality %d = %d with var %d\n", eqi->left, eqi->right, eqi->index));
    dpll_to_smt.emplace(var, eq);
}
