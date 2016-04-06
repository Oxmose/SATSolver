#include "ConflictGraph.h"

void ConflictGraph::clear()
{
	m_voisinDe.clear();
	m_levelOf.clear();
}

void ConflictGraph::add_node(pair<node,int> node_level)
{
	if(m_voisinDe.find(node_level.first) != m_voisinDe.end())
		return;

	m_voisinDe[node_level.first] = set<node>();
	m_levelOf[node_level.first] = node_level.second;
	OUTDEBUG("Add Node " << node_to_str(node_level.first));
}

void ConflictGraph::add_edge(node nodeA, node nodeB)
{
	OUTDEBUG("Add edge " << node_to_str(nodeA) << " -> " << node_to_str(nodeB));
	if(m_voisinDe.find(nodeA) == m_voisinDe.end())
	{
		OUTERROR("Fatal error in graph construction, node should exist before edge is constructed " << node_to_str(nodeA) << "-> " << node_to_str(nodeB));
	}
	m_voisinDe[nodeA].insert(nodeB);
}

set<node> ConflictGraph::get_nodes()
{
	set<node> toReturn;
	for(auto n: m_voisinDe)
		toReturn.insert(n.first);
	return toReturn;
}

string ConflictGraph::node_to_str(const node& a)
{
	return '"'+to_string(a.first)+"\n"+to_string(a.second)+"@"+to_string(m_levelOf[a])+'"';
}

void ConflictGraph::output(string file_name)
{
	ofstream myfile;
 	myfile.open(file_name);
    myfile << "digraph {\n";
	for(auto& e: m_voisinDe)
	{
		for(auto& v : e.second)
			myfile << node_to_str(e.first) + "->" + node_to_str(v) << ";\n";
		if(e.second.empty())
			myfile << node_to_str(e.first) << ";\n";
	}

	for(auto& e: m_voisinDe)
		if(m_levelOf[e.first] == levelMax)
			myfile << node_to_str(e.first) << "[shape=circle, style=filled, fillcolor=blue];\n";
	myfile << "}";
    myfile.close();
}