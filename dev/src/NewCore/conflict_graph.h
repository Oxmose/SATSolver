#ifndef DEF_CONFLICT_GRAPH_H
#define DEF_CONFLICT_GRAPH_H

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
#include <unordered_set>

using namespace std;

class SATSolver;

struct conflict_graph
{
     conflict_graph() {}
     conflict_graph(SATSolver const * solver) : conflict_literal(0), uip(0), solver(solver) {};
     void add_node(int l, pair<int,bool> level_bet);
     void add_edge(int l_a, int l_b);
     void remove_node(int l, bool total=true);

     inline bool has(int l)
     {
          return sons_of.find(l) != sons_of.end();
     }

     int conflict_literal;
     int uip;
     unordered_map<int, bool> in_uip_cut;

     void add_deduction_node(int from_clause, bool conflict=false);
     void find_uip_cut();
     void find_uip();
     int first_uip_in_ppch(int curr, unordered_map<int,int>& id_in_ppch);
     unordered_map<int,int> res_dyn_uip;
     
     unordered_map<int, pair<int,bool>> infos_on;//decision level and if is a bet

     unordered_map<int,unordered_set<int>> sons_of;
     unordered_map<int,unordered_set<int>> parents_of;

     string node_to_str(int l);
     void output(bool extra_info=false);

     SATSolver const * solver;
};

#endif // DEF_CONFLICT_GRAPH_H
