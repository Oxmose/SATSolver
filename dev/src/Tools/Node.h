/*
 *
 *  CLASS Node
 *
*/

#ifndef DEF_NODE_H
#define DEF_NODE_H

// SintD INCLUDES
#include <memory>

// GLOBAL FLAGS/VARS
#include "../Global/Global.h"   // DEBUG


class Node
{
    public:
        Node(int p_value);
        Node();
        Node(const Node &N);
        ~Node();

        int getValue();
        void setValue(int p_value);

        Node* getParent();
        void setParent(Node *p_parent);

        bool operator==(const Node &N) const;
        bool operator!=(const Node &N) const;
        bool operator<=(const Node &N) const;
    bool operator<(const Node &N) const;
        bool operator>=(const Node &N) const;
        bool operator>(const Node &N) const;
       
    private:
        Node *m_parent;
        int m_value;
        
}; // Node

#endif // DEF_NODE_H
