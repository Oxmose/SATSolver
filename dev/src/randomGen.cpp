/*
**
**  Random SAT formula generator
**
*/

#include <iostream>     // std::cin std::cout
#include <algorithm>    // std::transform
#include <string>       // std::string
#include <cstdlib>     // std::srand std::rand
#include <ctime>       // std::time

using namespace std;

int main()
{
    srand(time(NULL));

    unsigned int maxIndex;
    cout << "Enter the maximum index for the variables: ";
    cin >> maxIndex;

    cin.ignore();

    unsigned int clausesCount;
    cout << "Enter the number of clauses: ";
    cin >> clausesCount;

    cin.ignore();

    string header("p cnf " + to_string(maxIndex) + ' ' + to_string(clausesCount));
    string answer;
    do
    {
        cout << "Will all the clauses have the same size? (YES/no)" << endl;

        cin >> answer;
        cin.ignore();
        
        std::transform(answer.begin(), answer.end(), answer.begin(), ::tolower);
    }while(answer != "yes" && answer != "no" && answer != "");

    if(answer == "no")
    {
        string formula("");
        for(unsigned int i = 0; i < clausesCount; ++i)
        {
            unsigned int clausesSize;
            cout << "Size of the clause " << i + 1 << ": ";
            cin >> clausesSize;
            cin.ignore();
            for(unsigned int j = 0; j < clausesSize; ++j)
            {
                int random;
                do
                {
                    random = rand() % (maxIndex * 2) - maxIndex;
                }while(random == 0);

                formula += to_string(random) + ' ';
            }
            formula += "0\n";
        }
        cout << header << endl << formula << endl;
    }
    else
    {
        unsigned int clausesSize;
        cout << "Size of the clauses: ";
        cin >> clausesSize;

        string formula("");
        for(unsigned int i = 0; i < clausesCount; ++i)
        {
            for(unsigned int j = 0; j < clausesSize; ++j)
            {
                int random;
                do
                {
                    random = rand() % (maxIndex * 2) - maxIndex;
                }while(random == 0);

                formula += to_string(random) + ' ';
            }
            formula += "0\n";
        }
        cout << header << endl << formula << endl;
    }

    return 0;
} // main()
