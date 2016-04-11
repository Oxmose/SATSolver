#ifndef DEF_CONFLICT_G_H
#define DEF_CONFLICT_G_H

#include <map>
#include <set>
#include <vector>
#include "../Global/Global.h"
#include <iostream>
#include <string>

#include <queue>
#include <fstream>

using namespace std;

typedef pair<int,bool> node;

class ConflictGraph
{
	public:
		ConflictGraph(bool verbose=true) { m_verbose = verbose; m_voisinDe = map<node,set<node>>(); m_levelOf = map<node,int>();}

		void clear();
		void add_node(pair<node,int> node_level);
		void remove_node(node n);
		void add_edge(node nodeA, node nodeB);
		map<node,set<node>> get_graph();

		string node_to_str(const node& a);

		void output(string file_name, int the_bet, int the_conflict);
		size_t size();

		int levelMax;

		node findIUP(int the_bet, int the_conflict);

	private:

		map<node,set<node>> m_voisinDe;
		map<node,int> m_levelOf;
		bool m_verbose;
};

#endif
