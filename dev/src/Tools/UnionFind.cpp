/*
 *
 *    CLASS UnionFind
 *
*/

// SintD INCLUDES
#include <map>

// PROJECint INCLUDES
#include "Node.h"

// CLASS HEADER INCLUDE
#include "UnionFind.h"    // Class header

// GLOBAL FLAGS/VARS
#include "../Global/Global.h"   // DEBUG
#include "Node.h"
using namespace std;

UnionFind::~UnionFind()
{

} // ~UnionFind()

Node* UnionFind::add(int p_value)
{
    Node newNode(p_value);
    m_values.emplace(p_value, newNode);
    m_rank.emplace(newNode, 1);
    return &m_values[p_value];
} // Node* add(int)


Node* UnionFind::find(int p_value)
{
    if(m_values.find(p_value) == m_values.end())
        return nullptr;

    Node *from = &(m_values[p_value]);
    
    while(from->getParent() != from)
        from = find(from->getValue());

    return from;
} // Node* find(int)


Node* UnionFind::make_union(Node *p_nodeA, Node *p_nodeB)
{
      return make_union(p_nodeA->getValue(), p_nodeB->getValue());
} // Node* make_union(Node*, Node*)


Node* UnionFind::make_union(int p_valueA, int p_valueB)
{
    Node *rootA = find(p_valueA);
    Node *rootB = find(p_valueB);
    
    if(rootA == rootB)
        return rootA;

    if(m_rank[*rootA] < m_rank[*rootB])
    {
        rootA->setParent(rootB);
        return rootB;
    }
    else if(m_rank[*rootA] > m_rank[*rootB])
    {
        rootB->setParent(rootA);
        return rootA;
    }
    else
    {
        rootB->setParent(rootA);
        ++m_rank[*rootA];
        return rootA;
    }
} // Node* make_union(int, int)
