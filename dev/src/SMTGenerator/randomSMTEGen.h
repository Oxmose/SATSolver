/*
**
**  Random SMTE formula generator
**
*/

#ifndef DEF_RANDOMSMTEGENERATOR_H
#define DEF_RANDOMSMTEGENERATOR_H

// STD INCLUDES
#include <iostream>     // std::cin std::cout
#include <algorithm>    // std::transform
#include <string>       // std::string
#include <cstdlib>     	// std::srand std::rand
#include <ctime>       	// std::time
#include <fstream>     	// std::ofstream

enum OPERATOR
{
    IMPL = 0,
    EQUI = 1,
    OR   = 2,
    AND  = 3,
    XOR  = 4,
    NOT  = 5,
    EQUA = 6,
    DIFF = 7,
    NONE = 8
};

enum TYPE
{
    OP = 10,
    LIT = 11
};

typedef struct node node;
struct node
{    
    TYPE type;
    node *left;
    node *right;
    int lit;
    OPERATOR op;
    node(TYPE t, node* l, node* r, OPERATOR o = NONE, int lite = 0)
    {
        type = t;
        left = l;
        right = r;
        op = o;
        lit = lite;
    }
};

void randomGenerator(const std::string &p_fileName);
void generateTree(unsigned int maxIndex, unsigned int clausesCount, unsigned int clausesSize, std::ofstream &out);
void dfs(node *root, std::ofstream &out);

#endif // DEF_RANDOMSMTEGENERATOR_H
