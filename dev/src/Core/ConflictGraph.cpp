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

void ConflictGraph::output(string file_name, const node &UIP, const map<node, bool> &inCut, int conflictNode)
{
    ofstream myfile;
    myfile.open(file_name);
    myfile << "digraph {\n";
    map<node,bool> active;
    for(auto& e: m_voisinDe)
    {
        active[e.first] = m_levelOf[e.first] == levelMax;
        for(auto& v : e.second)
            active[e.first] = active[e.first] || m_levelOf[v] == levelMax;
        if(active[e.first])
        {
            for(auto& v : e.second)
                if(m_levelOf[v] == levelMax)
                    myfile << node_to_str(e.first) + "->" + node_to_str(v) << ";\n";
            if(e.second.empty())
                myfile << node_to_str(e.first) << ";\n";
        }
    }

    for(auto& e: m_voisinDe)
        if(m_levelOf[e.first] == levelMax)
        {
            if(e.first == UIP)
                myfile << node_to_str(e.first) << "[shape=circle, style=filled, fillcolor=yellow];\n";
            else if(inCut.find(e.first) != inCut.end())
                myfile << node_to_str(e.first) << "[shape=circle, style=filled, fillcolor=purple];\n";
            else
                myfile << node_to_str(e.first) << "[shape=circle, style=filled, fillcolor=blue];\n";
        }
    myfile << "}";
    myfile.close();
}

size_t ConflictGraph::size()
{
    return m_voisinDe.size();
}


void ConflictGraph::getUIP(node &ret, int &conflictIndex)
{
    set<node> nexts;
    queue<node> neight;

    node conflict = getConflictNode();
    node start = getStartNode();

    conflictIndex = conflict.first;

    node conflictBar = conflict;
    conflictBar.second = !conflict.second;

    ConflictGraph inverse;

    neight.push(start);

    map<node, bool> visited;
    while(!neight.empty())
    {
        node toVisit = neight.front();
        visited.emplace(toVisit, true);
        inverse.add_node(make_pair(toVisit, m_levelOf[toVisit]));
        neight.pop();

        for(auto v : m_voisinDe[toVisit])
        {
            if(visited.find(v) == visited.end())
            {
                inverse.add_node(make_pair(v, m_levelOf[v]));
                inverse.add_edge(v, toVisit);
                neight.push(v);
            }
        }
    }

    visited.clear();
    
    neight.push(conflict);
    nexts.insert(conflict);
    neight.push(conflictBar);
    nexts.insert(conflictBar);

    ret = make_pair(-1, false);
    while(!neight.empty())
    {
        node toVisit = neight.front();
        visited.emplace(toVisit, true);
        neight.pop();
        nexts.erase(toVisit);

        for(auto v : inverse.m_voisinDe[toVisit])
        {
            if(true /*visited.find(v) == visited.end()*/)
            {
                nexts.insert(v);
                neight.push(v);
            }
        }

        if(nexts.size() == 1)
        {
            ret = *(nexts.begin());
            return;
        }
    }
}

node ConflictGraph::getStartNode()
{
    map<node, bool> hasFather;
    for(auto v : m_voisinDe)
    {
        if(m_levelOf[v.first] == levelMax)
        {
            if(hasFather.find(v.first) == hasFather.end())
                hasFather.emplace(v.first, false);

            for(auto n : v.second)
            {
                if(hasFather.find(n) == hasFather.end())
                    hasFather.emplace(n, true);
                else
                    hasFather[n] = true;
            }
        }
    }

    for(auto val : hasFather)
        if(!val.second)
            return val.first;

    node ret = make_pair(-1, false);
    return ret;
}

node ConflictGraph::getConflictNode()
{
    map<int, bool> valuation;
    for(auto v : m_voisinDe)
    {
        if(m_levelOf[v.first] == levelMax)
        {
            if(valuation.find(v.first.first) == valuation.end())
                valuation.emplace(v.first.first, v.first.second);

            for(auto n : v.second)
            {
                if(valuation.find(n.first) == valuation.end())
                    valuation.emplace(n.first, n.second);
                else
                {
                    if(valuation[n.first] != n.second)
                    {
                        node ret = make_pair(n.first, false);
                        return ret;
                    }
                }
            }
        }
    }
    node ret = make_pair(-1, false);
    return ret;
}

void ConflictGraph::getUIPCut(std::map<node, bool> &inCut, node &uip)
{
    queue<node> neight;
    neight.push(uip);
    while(!neight.empty())
    {
        node toVisit = neight.front();
        neight.pop();
        for(auto v : m_voisinDe[toVisit])
        {
            if(inCut.find(v) == inCut.end())
            {
                neight.push(v);
                inCut.emplace(v, true);
            }
        }
    }
}
