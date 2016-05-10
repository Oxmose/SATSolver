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

string tab_char[7] = { "=>", "<=>", "\\/", "/\\", "X", "=", "!=" };

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

        while(clausesSizet && !to_fill.empty())
        {

            node* next_fill = to_fill.front();
            if(next_fill->left)
            {
                to_fill.pop();
            }

            op = (OPERATOR) (rand() % 7);

            node *op_n = new node(OP, nullptr, nullptr, op, 0);

            int left, right;

            while((left = rand() % (maxIndex * 2 + 2) - (maxIndex + 1)) == 0);
            while((right = rand() % (maxIndex * 2 + 2) - (maxIndex + 1)) == left || right == 0);

            if(op == EQUA || op == DIFF)
            {
                
                node *left_n = new node(LIT, nullptr, nullptr, NONE, 0);
                node *right_n = new node(LIT, nullptr, nullptr, NONE, 0);

                left_n->lit = left;
                right_n->lit = right;
                
                op_n->left = left_n;
                op_n->right = right_n;                
            }
            else
            {
                node *left_n = new node(LIT, nullptr, nullptr, NONE, 0);

                left_n->lit = left;                
                op_n->left = left_n;

                int rn = rand() % 3;
                if(rn == 0)
                {
                    to_fill.push(op_n); 
                }
                else
                {
                    node *right_n = new node(LIT, nullptr, nullptr, NONE, 0);

                    right_n->lit = right;                
                    op_n->right = right_n;
                }

                
            }

            if(!next_fill->left)
                next_fill->left = op_n;
            else
                next_fill->right = op_n;

            --clausesSizet;
        }
        while(!to_fill.empty())
        {
            node* next_fill = to_fill.front();
            if(!next_fill->left)
            {
                to_fill.pop();
            }

            int left, right;

            while((left = rand() % (maxIndex * 2 + 2) - (maxIndex + 1)) == 0);
            while((right = rand() % (maxIndex * 2 + 2) - (maxIndex + 1)) == left || right == 0);

            node *left_n = new node(LIT, nullptr, nullptr, NONE, 0);
            node *right_n = new node(LIT, nullptr, nullptr, NONE, 0);

            left_n->lit = left;
            right_n->lit = right;
            

            if(!next_fill->left)
            {
                next_fill->left = left_n;
            }
            else
                next_fill->right = right_n;
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
        out << "(";
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
