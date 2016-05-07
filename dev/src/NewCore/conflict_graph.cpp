// STD INCLUDES
#include <cassert>
#include <fstream>
#include <sstream>
#include <unistd.h>
#include <cstdio>
#include <string>
#include <vector>
#include <queue>
#include <set>
#include <unordered_map>

// HEADER INCLUDE
#include "conflict_graph.h"

// PROJECT INCLUDES
#include "SATSolver.h"

void conflict_graph::remove_node(int l, bool total /*=true*/)
{
    assert(settings_s.cl_s);
    assert(has(l));

    OUTDEBUG(fprintf(stderr, "%s removing of node %d.\n", (total) ? "Total" : "Partial", l));

    if(total)
    {
        for(auto& n: sons_of)
            n.second.erase(l);
        for(auto& n: parents_of)
            n.second.erase(l);
        sons_of.erase(l);
        parents_of.erase(l);
        infos_on.erase(l);
        return;
    }

    sons_of[l].clear();
}

void conflict_graph::add_node(int l, pair<int,bool> level_bet)
{
    assert(settings_s.cl_s);
    OUTDEBUG(fprintf(stderr, "Adding node (lit: %d, level: %d, bet: %d).\n", l, level_bet.first, level_bet.second));
    assert(infos_on.find(l) == infos_on.end());
    infos_on[l] = level_bet;
    sons_of[l] = set<int>();
    parents_of[l] = set<int>();
}

void conflict_graph::add_edge(int l_a, int l_b)
{
    assert(settings_s.cl_s);
    OUTDEBUG(fprintf(stderr, "Adding edge %d -> %d.\n", l_a, l_b));
    assert(infos_on.find(l_a) != infos_on.end() && infos_on.find(l_b) != infos_on.end());
    sons_of[l_a].insert(l_b);
    parents_of[l_b].insert(l_a);
}

/*
    The uip is to be found in the sub graph of nodes at curr_level.
    The algorithm to find the uip proceeds in 2 times,
        -First find the closest conflict node from the latest bet
        -Then DFS from the other one to see how far it can go into
        the shortest path without taking its edges.
    We have found the UIP.
*/

void conflict_graph::find_uip()
{
    assert(settings_s.cl_s);
    OUTDEBUG(fprintf(stderr, "\tComputing 1-UIP.\n"));
    queue<int> to_handle;
    unordered_map<int,bool> reached;
    unordered_map<int,int> father;

    vector<int> ppch;
    unordered_map<int,int> id_in_ppch;

    int the_bet = 0;
    for(int i = solver->decision_stack.size()-1 ; i >= 0 ; i--)
        if(solver->decision_stack[i].second)
        {
            the_bet = solver->decision_stack[i].first;
            i--;
            break;
        }

    assert(the_bet != 0);//unclear

    int conflict_index = abs(conflict_literal);

    to_handle.push(the_bet);
    reached[the_bet] = true;
    father[the_bet] = 0;

    int conflict_reached = 0;

    while(!to_handle.empty())
    {
        int curr = to_handle.front();
        to_handle.pop();
        
        if(abs(curr) == conflict_index)
        {
            conflict_reached = curr;
            break;
        }

        for(auto v : sons_of[curr])
            if(reached.find(v) == reached.end())
            {
                reached[v] = true;

                to_handle.push(v);
                father[v] = curr;
            }
    }

    assert(conflict_reached != 0);
    
    int curr = conflict_reached;
    int i = 0;
    while(curr)
    {
        //fprintf(stderr, "OOO In ppch: %d\n", curr);
        ppch.push_back(curr);
        id_in_ppch[curr] = i;
        curr = father[curr];
        i++;
    }

    res_dyn_uip.clear();
    int max_id_reached = first_uip_in_ppch(-conflict_reached, id_in_ppch);
    res_dyn_uip.clear();
    id_in_ppch.erase(conflict_reached);
    res_dyn_uip.clear();
    max_id_reached = max(max_id_reached, first_uip_in_ppch(conflict_reached, id_in_ppch));
    res_dyn_uip.clear();
    uip = ppch[max_id_reached];
}

void conflict_graph::add_deduction_node(int from_clause, bool conflict/*=false*/)
{
    assert(settings_s.cl_s);
    assert(!solver->decision_stack.empty());
    int last_decision = solver->decision_stack.back().first;
    conflict_literal = last_decision;
    if(conflict)
        conflict_literal *= -1;

    add_node(conflict_literal, make_pair(solver->curr_level,false));
    if(from_clause != -1)
        for(auto l : solver->formula[from_clause].literal)
            if(l != conflict_literal)
                add_edge(-l,conflict_literal);
}

int conflict_graph::first_uip_in_ppch(int curr, unordered_map<int,int>& id_in_ppch)
{
    if(res_dyn_uip.find(curr) != res_dyn_uip.end())
        return res_dyn_uip[curr];
    assert(settings_s.cl_s);
    //Fprintf("In: %d\n", curr);
    if(infos_on[curr].first != solver->curr_level)
    {
        //printf("Out: %d %d\n", curr, -1);
        res_dyn_uip[curr] = -1;
        return -1;
    }

    int M = -1;
    if(id_in_ppch.find(curr) != id_in_ppch.end())
    {
        //printf("%d in ppch : %d\n", curr, id_in_ppch[curr]);
        res_dyn_uip[curr] = id_in_ppch[curr];
        return id_in_ppch[curr];
    }

    for(auto v : parents_of[curr])
        M = max(M, first_uip_in_ppch(v, id_in_ppch));
    //printf("Out: %d %d\n", curr, M);
    res_dyn_uip[curr] = M;
    return M;
}

void conflict_graph::find_uip_cut()
{
    assert(settings_s.cl_s);
    OUTDEBUG(fprintf(stderr, "\tComputing UIP cut.\n"));

    find_uip();
    assert(uip != 0);

    in_uip_cut.clear();
    queue<int> to_handle;
    to_handle.push(uip);

    while(!to_handle.empty())
    {
        int curr = to_handle.front();
        to_handle.pop();

        for(auto v : sons_of[curr])
            if(in_uip_cut.find(v) == in_uip_cut.end())
            {
                in_uip_cut[v] = true;
                to_handle.push(v);
            }
    }
}

string conflict_graph::node_to_str(int l)
{
    assert(settings_s.cl_s);
    int level = infos_on[l].first;
    return string("\"")+to_string(l)+"@"+to_string(level)+"\"";
}

void conflict_graph::output(bool extra_info/* = false */)
{
    assert(settings_s.cl_s);

    static int i = 0;
    //if(i > 2)
    //    exit(0);
    ofstream myfile;
    myfile.open("conflictGraph"+to_string(i)+".dot");

    OUTDEBUG(fprintf(stderr, "Outputing graph on d%s.dot.\n", to_string(i).c_str()));
    if(settings_s.clinterac_s)
        cout << "Outputing graph on conflictGraph" << to_string(i).c_str() << ".dot" << endl;
    i++;
    
    myfile << "digraph {\n";

    for(auto& e: sons_of)
    {
        for(auto& v : e.second)
            myfile << node_to_str(e.first) + "->" + node_to_str(v) << ";\n";
        if(e.second.empty())
            myfile << node_to_str(e.first) << ";\n";
    }

    for(auto& e: sons_of)
    {
        int level = infos_on[e.first].first;
        bool bet = infos_on[e.first].second;
        if(extra_info && e.first == uip)
            myfile << node_to_str(e.first) << "[shape=circle, style=filled, fillcolor=yellow];\n";
        else if(bet && level == solver->curr_level)
            myfile << node_to_str(e.first) << "[shape=circle, style=filled, fillcolor=green];\n";
        else if(abs(e.first) == abs(conflict_literal))
            myfile << node_to_str(e.first) << "[shape=circle, style=filled, fillcolor=red];\n";
        else if(level == solver->curr_level)
            myfile << node_to_str(e.first) << "[shape=circle, style=filled, fillcolor=blue];\n";
    }
    myfile << "}";
    myfile.close();
}
