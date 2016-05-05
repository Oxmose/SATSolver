/*
 *
 *  CLASS Node
 *
*/

#ifndef DEF_NODE_H
#define DEF_NODE_H

// STD INCLUDES
#include <memory>

// GLOBAL FLAGS/VARS
#include "../Global/Global.h"   // DEBUG

template<class T>
class Node
{
    public:
        Node(T p_value);
        ~Node();

        T getValue();
        void setValue(T p_value);

        Node<T>* getParent();
        void setParent(Node<T> *p_parent);
       
    private:
	    Node<T> *m_parent;
        T m_value;
        
}; // Node

#endif // DEF_NODE_H
