#include "ConflictGraph.h"

void ConflictGraph::clear()
{
    m_voisinDe.clear();
    m_levelOf.clear();
    levelMax = -1;
}

void ConflictGraph::add_node(pair<node,int> node_level)
{

    if(!m_voisinDe.empty() && m_voisinDe.find(node_level.first) != m_voisinDe.end())
        return;

    //OUTERROR("ok " << node_level.first.first << " " << node_level.first.second);
    m_voisinDe[node_level.first] = set<node>();
    //OUTERROR("ok");
    m_levelOf[node_level.first] = node_level.second;
    //printf("%d %d %s\n", node_level.second, m_levelOf[make_pair(65,false)], node_to_str(node_level.first).c_str());
    levelMax = max(levelMax, node_level.second);

    if(m_verbose)
        OUTDEBUG("Add Node " << node_to_str(node_level.first));
}

void ConflictGraph::remove_node(node n)
{

    if(m_voisinDe.empty() || m_voisinDe.find(n) == m_voisinDe.end())
    {
        OUTERROR("Cannot remove unexisting node " << node_to_str(n));
        return;
    }

    if(m_verbose)
        OUTDEBUG("Remove node " << node_to_str(n));
    m_voisinDe.erase(n);
}

void ConflictGraph::add_edge(node nodeA, node nodeB)
{
    if(m_verbose)
        OUTDEBUG("Add edge " << node_to_str(nodeA) << " -> " << node_to_str(nodeB));
    if(!m_voisinDe.empty() && (m_voisinDe.find(nodeA) == m_voisinDe.end()))
    {
        OUTERROR("Fatal error in graph construction, node should exist before edge is constructed " << node_to_str(nodeA) << "-> " << node_to_str(nodeB));
    }
    m_voisinDe[nodeA].insert(nodeB);
}

map<node,set<node>> ConflictGraph::get_graph()
{
    return m_voisinDe;
}

string ConflictGraph::node_to_str(const node& a)
{
    return '"'+to_string(a.first)+"\n"+to_string(a.second)+"@"+to_string(m_levelOf[a])+'"';
}

node ConflictGraph::findIUP(int the_bet, int the_conflict)
{
    node ret;

    set<node> nexts;
    queue<node> neight;

    node conflict;
    node start;
    for(auto a : m_voisinDe)
    {
        if(a.first.first == the_bet)
            start = a.first;
        if(a.first.first == the_conflict)
            conflict = a.first;
    }


    node conflictBar = conflict;
    conflictBar.second = !conflict.second;

    ConflictGraph inverse(false);

    neight.push(start);

    map<node, bool> visited;
    while(!neight.empty())
    {
        node toVisit = neight.front();
        inverse.add_node(make_pair(toVisit, m_levelOf[toVisit]));
        neight.pop();

        for(auto v : m_voisinDe[toVisit])
        {
            inverse.add_node(make_pair(v, m_levelOf[v]));
            inverse.add_edge(v, toVisit);
            neight.push(v);
        }
    }

    visited.clear();
    
    map<node, bool> uips;
    bool found = false;
    for(auto v : m_voisinDe)
    {
        if(m_levelOf[v.first] == levelMax)
        {
            visited.clear();
            
            while(!neight.empty())
                neight.pop();

            found = false;
            neight.push(conflict);
            neight.push(conflictBar);
            while(!neight.empty())
            {
                node toVisit = neight.front();
                visited.emplace(toVisit, true);
                neight.pop();
                nexts.erase(toVisit);

                for(auto u : inverse.m_voisinDe[toVisit])
                {
                    if(u == start)
                        found = true;
                    else if(visited.find(u) == visited.end() && u != v.first)
                    { 
                        neight.push(u);
                    }
                }
                if(found)
                    break;
            }
            if(!found)
                uips.emplace(v.first, true);
        }
    }
    
    while(!neight.empty())
        neight.pop();

    neight.push(conflict);
    neight.push(conflictBar);

    for(auto v : uips)
        cout << "UIP : " << v.first.first << endl;
    while(!neight.empty())
    {
        node toVisit = neight.front();
        neight.pop();

        for(auto v : inverse.m_voisinDe[toVisit])
        {
            if(uips.find(v) != uips.end())
            {
                cout << "FOUDN " << v.first << endl;
                return v;
            }

            neight.push(v);
        }
    }

    cout << "SENT IS : " << start.first << endl;
    return start;
}

void ConflictGraph::output(string file_name, int the_bet, int the_conflict)
{
    //set<int> theCut = findIUPCut();

    ofstream myfile;
    myfile.open(file_name);
    myfile << "digraph {\n";

    map<node,bool> active;
    
    std::vector<int> v;
    map<int,bool> vu;

    for(auto& e: m_voisinDe)
    {
        if(vu.find(e.first.first) != vu.end())
            v.push_back(e.first.first);
        //else if(vu.find(e.first.first) != vu.end())
          //  OUTERROR("More than one conflict...");
        vu[e.first.first] = true;
        //printf("Node: %d %d %s\n", e.first.first, e.first.second, node_to_str(e.first).c_str());
        active[e.first] = m_levelOf[e.first] == levelMax;
        for(auto& v : e.second)
            active[e.first] = active[e.first] || m_levelOf[v] == levelMax;
        if(true)
        {
            for(auto& v : e.second)
                if(true)
                    myfile << node_to_str(e.first) + "->" + node_to_str(v) << ";\n";
            if(e.second.empty())
                myfile << node_to_str(e.first) << ";\n";
        }
    }

    if(m_verbose)
        for(auto a: v)
            OUTDEBUG("Conflict index : "<< a << "\n");
    if(v.size() != 1)
        OUTERROR("Should be one conflict.");
    if(v[0] != the_conflict)
        OUTERROR("Conflictual conflict");

    node iup = findIUP(the_bet,the_conflict);
    for(auto& e: m_voisinDe)
        if(true)
        {
            if(e.first.first == the_bet)
                myfile << node_to_str(e.first) << "[shape=circle, style=filled, fillcolor=red];\n";
            else if(e.first.first == iup.first)
                myfile << node_to_str(e.first) << "[shape=circle, style=filled, fillcolor=yellow];\n";
            else if(e.first.first == v[0])
                myfile << node_to_str(e.first) << "[shape=circle, style=filled, fillcolor=green];\n";
            else if(m_levelOf[e.first] == levelMax)
                myfile << node_to_str(e.first) << "[shape=circle, style=filled, fillcolor=blue];\n";
        }
    myfile << "}";
    myfile.close();
}

size_t ConflictGraph::size()
{
    return m_voisinDe.size();
}
