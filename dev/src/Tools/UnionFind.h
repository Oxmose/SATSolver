/*
 *
 *  CLASS UnionFind
 *
*/

#ifndef DEF_UNIONFIND_H
#define DEF_UNIONFIND_H

// STD INCLUDES

// GLOBAL FLAGS/VARS
#include "../Global/Global.h"   // DEBUG

template<class T>
class UnionFind
{
    public:
        ~UnionFind();

        Node<T>* add(T p_value);

        Node<T>* find(T p_value);

        Node<T>* make_union(Node<T> *p_nodeA, Node<T> *p_nodeB);
        Node<T>* make_union(T p_valueA, T p_valueB);
       
    private:
        map<T, Node<T>> m_values;
        map<Node<T>, size_t> m_rank;
        bool m_useCongruence;
        
}; // UnionFind

#endif // DEF_UNIONFIND_H
