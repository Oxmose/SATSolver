#ifndef DEF_CONFLICT_G_H
#define DEF_CONFLICT_G_H

#include <map>
#include <set>
#include <vector>
#include "../Global/Global.h"
#include <iostream>
#include <string>

#include "Clause.h"

#include <queue>
#include <fstream>

using namespace std;

typedef pair<int,bool> node;

class ConflictGraph
{
	public:
		ConflictGraph(bool verbose=true) { m_uip = make_pair(-1,0); m_verbose = verbose; m_voisinDe = map<node,set<node>>(); m_levelOf = map<node,int>(); 
    NIL = make_pair(-1, false);}

		void clear();
		void add_node(pair<node,int> node_level);
		void remove_node(node n);
		void add_edge(node nodeA, node nodeB);
		map<node,set<node>> get_graph();

		string node_to_str(const node& a);

		void output(string file_name, int the_bet, int the_conflict);
		size_t size();

		int levelMax;

		pair<Clause,int> resolution(bool naiveuip, node the_bet, node the_conflict, int id_clause);
		void findUIP(bool m_naiveuip, node the_bet, node the_conflict);
        void browseAP(ConflictGraph &inverse, node u, std::map<node, bool> &visited, std::map<node, int> &disc, std::map<node, int> &low, std::map<node, node> &parent, std::map<node, bool> &ap);
		void findUIPCut();

	private:

		map<node,set<node>> m_voisinDe;
		map<node,int> m_levelOf;
		bool m_verbose;

		node m_uip;
		std::map<node, bool> m_inCut;
                node NIL;
};

#endif
