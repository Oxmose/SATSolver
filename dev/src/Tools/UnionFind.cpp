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
	for(auto entry : m_values)
		delete entry.second;
} // ~UnionFind()

Node* UnionFind::add(int p_value)
{
    if(m_values.find(p_value) != m_values.end())
	    return m_values[p_value];	

    Node *newNode = new Node(p_value);
    m_values.emplace(p_value, newNode);
    m_rank.emplace(newNode, 1);
    return newNode;
} // Node* add(int)


Node* UnionFind::find(int p_value)
{
    if(m_values.find(p_value) == m_values.end())
        return nullptr;

    Node *from = m_values[p_value];
    if(from->getParent() != from)
        from->setParent(find(from->getParent()->getValue()));
    return from->getParent();
} // Node* find(int)

Node* UnionFind::make_union(int p_valueA, int p_valueB)
{
    Node *rootA = find(p_valueA);
    Node *rootB = find(p_valueB);
    
    if(rootA == rootB)
        return rootA;;

    if(m_rank[rootA] < m_rank[rootB])
    {
        rootA->setParent(rootB);
        return rootB;
    }
    else if(m_rank[rootA] > m_rank[rootB])
    {
        rootB->setParent(rootA);
        return rootA;
    }
    else
    {
        rootB->setParent(rootA);
        ++m_rank[rootA];
        return rootA;
    }
} // Node* make_union(int, int)
