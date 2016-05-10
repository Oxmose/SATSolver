/*
**
**  Random SMTE formula generator
**
*/

// STD INCLUDES
#include <iostream>    // std::cin std::cout
#include <algorithm>   // std::transform
#include <string>      // std::string
#include <cstdlib>     // std::srand std::rand
#include <ctime>       // std::time
#include <fstream>     // std::ofstream
#include <queue>

// FILE HEADER
#include "randomSMTEGen.h"

using namespace std;

string tab_char[8] = { "=>", "<=>", "\\/", "/\\", "X", "~", "=", "!=" };

int main()
{
    srand(time(NULL));
    string filename;
    cout << "Entre file name: ";
    cin >> filename;
    randomGenerator(filename);
    return 0;
}

void randomGenerator(const string &p_fileName)
{
    

    unsigned int maxIndex;
    cout << "Enter the maximum index for the variables: ";
    cin >> maxIndex;
    cin.ignore();

    unsigned int clausesCount;
    cout << "Enter the number of clauses: ";
    cin >> clausesCount;
    cin.ignore();

    unsigned int clausesSize;
    cout << "Enter the size of clauses (0 for random): ";
    cin >> clausesSize;
    cin.ignore();
    
    

    ofstream out(p_fileName, ofstream::out | ofstream::trunc);
    if(!out)
    {
        cerr << "Can't open output file." << endl;
        return;
    }
        
    generateTree(maxIndex, clausesCount, clausesSize, out);
    out.flush();
    out.close();
    cout << "File saved!" << endl;
} // randomGenerator(const string&)

void generateTree(unsigned int maxIndex, unsigned int clausesCount, unsigned int clausesSize, ofstream &out)
{
    for(unsigned int i = 0; i < clausesCount; ++i)
    {
        node *root = new node(OP, nullptr, nullptr, NONE, 0);

        OPERATOR op = (OPERATOR) (rand() % 5);
        root->op = op;

        unsigned int clausesSizet;
        if(clausesSize == 0)
            clausesSizet = rand() % 10 + 3;
        else
            clausesSizet = clausesSize;

        queue<node*> to_fill;
        to_fill.push(root);

        while(clausesSizet)
        {
            node* next_fill = to_fill.front();

            op = (OPERATOR) (rand() % 5);

            node *op_n = new node(OP, nullptr, nullptr, op, 0);

            to_fill.push(op_n);

            if(rand() % 2)
                next_fill->left = op_n;
            else
                next_fill->right = op_n;            

            --clausesSizet;
        }
        while(!to_fill.empty())
        {
            //cout << "HERE" << endl;
            node* next_fill = to_fill.front();
            if(next_fill->left != nullptr || next_fill->right != nullptr)
            {
                to_fill.pop();
            }

            int left, right;

            while((left = rand() % (maxIndex + 1)) == 0);
            while((right = rand() % (maxIndex + 1)) == left || right == 0);

            node *left_n = new node(LIT, nullptr, nullptr, NONE, 0);
            node *right_n = new node(LIT, nullptr, nullptr, NONE, 0);

            left_n->lit = left;
            right_n->lit = right;

            node *equal;
            int eq = rand() % 2;
            if(eq)
                equal = new node(OP, nullptr, nullptr, EQUA, 0);
            else
                equal = new node(OP, nullptr, nullptr, DIFF, 0);

            equal->left = left_n;
            equal->right = right_n;


            if(!next_fill->left)
            {
                next_fill->left = equal;
            }
            else
            {
                next_fill->right = equal;
            }
        }
        dfs(root, out);
        if(i == clausesCount - 1)
        {
            out << " 0" << endl;
        }
        else
        {
            out << endl;
        }
    }
}

void dfs(node *root, ofstream &out)
{
    if(root->type != LIT)
    {
        if(root->op == DIFF || root->op == EQUA)
        {
            if(!(rand() % 4))
                out << "~(";
            else
                out << "(";
        }
        else
            out << "(";
    }

    if(root->left)
        dfs(root->left, out);

    if(root->type == LIT)
        out << root->lit;
    else
        out << " " << tab_char[root->op] << " ";

    if(root->right)
        dfs(root->right, out);

    if(root->type != LIT)
        out << ")";
}
