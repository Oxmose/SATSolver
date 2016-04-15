#include "ConflictGraph.h"
#include "Clause.h"

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


    levelMax = -1;
    m_voisinDe.erase(n);
    for(auto& e : m_voisinDe)
    {
        levelMax = max(levelMax, m_levelOf[e.first]);
        e.second.erase(n);
    }
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

pair<Clause,int> ConflictGraph::resolution(node the_bet, node the_conflict, int id_clause)
{
    findUIP(the_bet,the_conflict);
    findUIPCut();
    std::map<int,bool> lit;
    int backtrackLevel = -1000;
    for(auto& e : m_voisinDe)
        for(auto& v : e.second)
            if(m_inCut.find(e.first) == m_inCut.end() && m_inCut.find(v) != m_inCut.end())
            {
                if(m_levelOf[e.first] != m_levelOf[the_conflict])
                    backtrackLevel = max(backtrackLevel,m_levelOf[e.first]);
                lit[e.first.first] = e.first.second;
            }
    if(backtrackLevel == -1000)
        backtrackLevel = m_levelOf[the_conflict]-1;
    if(lit.size() == 1)
        backtrackLevel = 0;
    return make_pair(Clause(lit,false,id_clause),backtrackLevel);
}

void ConflictGraph::findUIP(node the_bet, node the_conflict)
{
    queue<node> neight;

    node conflict = the_conflict;
    node start = the_bet;

    node conflictBar = conflict;
    conflictBar.second = !conflict.second;

    ConflictGraph inverse(false);

    for(auto u : m_voisinDe)
    {
        inverse.add_node(make_pair(u.first, m_levelOf[u.first]));
        for(auto v : u.second)
        {
            inverse.add_node(make_pair(v, m_levelOf[v]));
            inverse.add_edge(v, u.first);
        }
    }

    map<node, bool> visited;    
/*    map<node, bool> access;
    neight.push(conflict);
    neight.push(conflictBar);

    while(!neight.empty())
    {
        node toVisit = neight.front();
        neight.pop();

        for(auto u : inverse.m_voisinDe[toVisit])
        {
            if(m_levelOf[u] == levelMax && visited.find(u) == visited.end())
            {
                visited.emplace(u, true);
                neight.push(u);
                access.emplace(u, true);
            }
        }
    }

    visited.clear();
    neight.push(start);
    while(!neight.empty())
    {
        node toVisit = neight.front();
        neight.pop();

        if(m_voisinDe[toVisit].size() == 1)
        {
            if(*(m_voisinDe[toVisit].begin()) == conflict || *(m_voisinDe[toVisit].begin()) == conflictBar)
            {
                m_uip = start;
                return;
            }
        }
        for(auto u : m_voisinDe[toVisit])
        {            
            if(access.find(u) != access.end() && visited.find(u) == visited.end())
            {
                visited.emplace(u, true);
                neight.push(u);
            }
        }
        if(neight.size() == 1 && neight.front() != conflict && neight.front() != conflictBar)
        {
            node toGo = neight.front();
            if(m_voisinDe[toGo].size() == 1)
            {
                if(*(m_voisinDe[toGo].begin()) != conflict && *(m_voisinDe[toGo].begin()) != conflictBar)
                {                
                    while(m_voisinDe[toGo].size() == 1 && *(m_voisinDe[toGo].begin()) != conflict && *(m_voisinDe[toGo].begin()) != conflictBar)
                    {
                        toGo = *(m_voisinDe[toGo].begin());
                    }
                    m_uip = toGo;
                    return;
                }
            }
            else
            {
                m_uip = toGo;
                return;
            }
        }
    }
*/
    map<node, bool> uips;
    
    bool found = false;

    for(auto v : inverse.m_voisinDe)
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
                neight.pop();

                for(auto u : inverse.m_voisinDe[toVisit])
                {
                    if(u == start)
                    {
                        found = true;
                        break;
                    }
                    else if(visited.find(u) == visited.end() && u != v.first)
                    {
                        visited.emplace(u, true);
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
    visited.clear();
    visited[neight.front()] = true;
    while(!neight.empty())
    {
        node toVisit = neight.front();
        neight.pop();

        for(auto v : inverse.m_voisinDe[toVisit])
        {
            if(visited.find(v) == visited.end())
            {
                visited[v] = true;
                if(uips.find(v) != uips.end())
                { 
                    m_uip = v;
                    return;
                }
                neight.push(v);
            }
        }
    }

    m_uip = start;
}

void ConflictGraph::findUIPCut()
{
    queue<node> neight;
    m_inCut.clear();
    neight.push(m_uip);
    while(!neight.empty())
    {
        node toVisit = neight.front();
        /*OUTDEBUG("???" << toVisit.first);
        for(auto a : m_inCut)
            OUTDEBUG("\t???" << a.first.first);*/
        neight.pop();
        for(auto v : m_voisinDe[toVisit])
        {
            if(m_inCut.find(v) == m_inCut.end())
            {
                neight.push(v);
                m_inCut.emplace(v, true);
            }
        }
    }
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
        active[e.first] = (m_inCut.find(e.first) == m_inCut.end());
        bool b = false;
        for(auto& v : e.second)
            b = b || (m_inCut.find(v) != m_inCut.end());
        active[e.first] = active[e.first] && b;
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
    if(v.size() != 1 && the_conflict != -1)
        OUTERROR("Should be one conflict.");
    if(the_conflict != -1 && v[0] != the_conflict)
        OUTERROR("Conflictual conflict");


    for(auto& e: m_voisinDe)
        if(true)
        {
            if(e.first.first == m_uip.first)
                myfile << node_to_str(e.first) << "[shape=circle, style=filled, fillcolor=yellow];\n";
            else if(the_bet != -1 && e.first.first == the_bet)
                myfile << node_to_str(e.first) << "[shape=circle, style=filled, fillcolor=red];\n";
            else if(the_conflict != -1 && e.first.first == v[0])
                myfile << node_to_str(e.first) << "[shape=circle, style=filled, fillcolor=green];\n";
            else if(m_inCut.find(e.first) != m_inCut.end())
                myfile << node_to_str(e.first) << "[shape=circle, style=filled, fillcolor=purple];\n";
            else if(m_levelOf[e.first] == levelMax)
                myfile << node_to_str(e.first) << "[shape=circle, style=filled, fillcolor=blue];\n";
            else if(active[e.first])
                myfile << node_to_str(e.first) << "[shape=circle, style=filled, fillcolor=pink];\n";
        }
    myfile << "}";
    myfile.close();
}

size_t ConflictGraph::size()
{
    return m_voisinDe.size();
}
