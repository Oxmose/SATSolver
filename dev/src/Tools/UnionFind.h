/*
 *
 *  CLASS UnionFind
 *
*/

#ifndef DEF_UNIONFIND_H
#define DEF_UNIONFIND_H

// STD INCLUDES
#include <map>

// PROJECT INCLUDES
#include "Node.h"

// GLOBAL FLAGS/VARS
#include "../Global/Global.h"   // DEBUG

class Node;

class UnionFind
{
    public:
        ~UnionFind();

        Node* add(int p_value);

        Node* find(int p_value);
        Node* make_union(int p_valueA, int p_valueB);
       
    private:
        map<int, Node*> m_values;
        map<Node*, size_t> m_rank;
        bool m_useCongruence;
        
}; // UnionFind

#endif // DEF_UNIONFIND_H
