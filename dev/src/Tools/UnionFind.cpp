/*
 *
 *    CLASS UnionFind
 *
*/

// STD INCLUDES

// CLASS HEADER INCLUDE
#include "UnionFind.h"    // Class header

// GLOBAL FLAGS/VARS
#include "../Global/Global.h"   // DEBUG

using namespace std;

template<class T>
UnionFind<T>::~UnionFind()
{

} // ~UnionFind()

template<class T>
Node<T>* UnionFind<T>::add(T p_value)
{
    Node<T> newNode(p_value);
    m_values.emplace(p_value, newNode);
    m_rank.emplace(newNode, 1);
    return &newNode;
} // Node<T>* add(T)

template<class T>
Node<T>* UnionFind<T>::find(T p_value)
{
    if(m_values.find(p_value) == m_values.end())
        return nullptr;
    Node<T> *from = &(m_values[p_value]);
    while(from.getParent() != from)
        from = find(from->getValue());
    return from;
} // Node<T>* find(T)

template<class T>
Node<T>* UnionFind<T>::make_union(Node<T> *p_nodeA, Node<T> *p_nodeB)
{
      return make_union(p_nodeA->getValue(), p_nodeB->getValue());
} // Node<T>* make_union(Node<T>*, Node<T>*)

template<class T>
Node<T>* UnionFind<T>::make_union(T p_valueA, T p_valueB)
{
    Node<T> rootA = find(p_valueA);
    Node<T> rootB = find(p_valueB);
    
    if(rootA == rootB)
        return rootA;

    if(m_rank[*rootA] < m_rank[*rootB])
    {
        rootA.setParent(rootB);
        return rootB;
    }
    else if(m_rank[*rootA] > ranf[*rootB])
    {
        rootB.setParent(rootA);
        return rootA;
    }
    else
    {
        rootB.setParent(rootA);
        ++m_rank[*rootA];
        return rootA;
    }
} // Node<T>* make_union(T, T)
