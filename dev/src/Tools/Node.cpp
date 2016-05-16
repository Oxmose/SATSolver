/*
 *
 *  CLASS Node
 *
*/

// SintD INCLUDES
#include <memory>

// CLASS HEADER INCLUDE
#include "Node.h"    // Class header

// GLOBAL FLAGS/VARS
#include "../Global/Global.h"   // DEBUG

using namespace std;


Node::Node(int p_value)
{
    m_value = p_value;
    m_parent = this;
}


Node::Node()
{
    m_value = -1;
    m_parent = this;
}

Node::Node(const Node &N)
{
    m_value = N.m_value;
    m_parent = this;

}

Node::~Node()
{
}


int Node::getValue()
{
    return m_value;
}


void Node::setValue(int p_value)
{
    m_value = p_value;
}


Node* Node::getParent()
{
    return m_parent;
}


void Node::setParent(Node* p_parent)
{
    m_parent = p_parent;
}


void Node::add_son(Node *son)
{
    m_sons.push_back(son);
} // add_son(Node*)

vector<Node*> Node::get_sons()
{
	return m_sons;
}

bool Node::operator==(const Node &N) const
{
	return m_value == N.m_value;
}
bool Node::operator!=(const Node &N) const 
{
    return m_value != N.m_value;
}
bool Node::operator<=(const Node &N) const
{
    return m_value <= N.m_value;
}
bool Node::operator<(const Node &N) const 
{
    return m_value < N.m_value;
}
bool Node::operator>=(const Node &N) const 
{
    return m_value >= N.m_value;
}
bool Node::operator>(const Node &N) const
{
    return m_value > N.m_value;
}
