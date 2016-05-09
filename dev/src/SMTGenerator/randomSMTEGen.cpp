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

// FILE HEADER
#include "randomSMTEGen.h"

using namespace std;

int main()
{
    string filename;
    cout << "Entre file name: ";
    cin >> filename;
    randomGenerator(filename);
    return 0;
}

void randomGenerator(const string &p_fileName)
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

    
	string formula("");
	for(unsigned int i = 0; i < clausesCount; ++i)
	{
	    
		int random1 = rand() % maxIndex + 1;
        int random2;
        while((random2 = rand() % maxIndex + 1) == random1);

        int eq = rand() % 2;

		formula += to_string(random1);
	    if(eq)
            formula += " = ";
        else
            formula += " != ";
        formula += to_string(random2);	
        if(i == clausesCount - 1)
            formula += " 0\n";   
        else
            formula += "\n"; 
	}
    
	ofstream out(p_fileName, ofstream::out | ofstream::trunc);
	if(!out)
	{
	    cerr << "Can't open output file." << endl;
	    return;
	}
        
    out << formula;
    out.close();
    cout << "File saved!" << endl;
} // randomGenerator(const string&)
