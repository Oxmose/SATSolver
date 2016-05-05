/*
 *
 *  CLASS Node
 *
*/

// STD INCLUDES
#include <memory>

// CLASS HEADER INCLUDE
#include "Node.h"    // Class header

// GLOBAL FLAGS/VARS
#include "../Global/Global.h"   // DEBUG

using namespace std;

template<class T>
Node<T>::Node(T p_value)
{
    m_value = p_value;
    m_parent = this;

}

template<class T>
Node<T>::~Node()
{
}

template<class T>
T Node<T>::getValue()
{
    return m_value;
}

template<class T>
void Node<T>::setValue(T p_value)
{
    m_value = p_value;
}

template<class T>
Node<T>* Node<T>::getParent()
{
    return m_parent;
}

template<class T>
void Node<T>::setParent(Node<T>* p_parent)
{
    m_parent = p_parent;
}
