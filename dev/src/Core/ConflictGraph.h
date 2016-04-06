#ifndef DEF_CONFLICT_G_H
#define DEF_CONFLICT_G_H

#include <map>
#include <set>
#include <vector>
#include "../Global/Global.h"
#include <iostream>
#include <string>

#include <iostream>
#include <fstream>

using namespace std;

typedef pair<int,bool> node;

class ConflictGraph
{
	public:
		ConflictGraph() {}

		void clear();
		void add_node(pair<node,int> node_level);
		void add_edge(node nodeA, node nodeB);
		set<node> get_nodes();

		string node_to_str(const node& a);
		void output(string file_name);

		int levelMax;


	private:

		map<node,set<node>> m_voisinDe;
		map<node,int> m_levelOf;
};

#endif