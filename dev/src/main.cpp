// SATSolver class
#include "Core/SATSolver.h"

// STD INCLUDES
#include <ctime>        //std::clock
#include <iostream>     //std::cout std::cerr
#include <sys/ioctl.h>  //std::ioctl

// OTHERS INCLUDES FROM PROJECT
#include "RandomSatExpGenerator/randomGen.h"

using namespace std;

#define VERSION "0.1"

void displayMenu(char *softName)
{
    struct winsize w;
    ioctl(0, TIOCGWINSZ, &w);

    unsigned int total = 0;

    for(unsigned int i = 0; i < w.ws_col / 2 - 8; ++i)
    {
        cout << "=";
        ++total;
    }
    cout << " SATSolver V" << VERSION << " ";
    total += 16;
    for(unsigned int i = total; i < w.ws_col; ++i)
        cout << "=";
    cout << endl;
    total = 0;
    cout << "#";
    for(unsigned int i = 0; i < w.ws_col / 2 - 19; ++i)
    {
        cout << " ";
        ++total;
    }
    cout << " By Tristan Sterin and Alexy Torres ";
    total += 36;
    for(unsigned int i = total; i < w.ws_col - 2; ++i)
        cout << " ";
    cout << "#" << endl;
    for(unsigned int i = 0; i < w.ws_col; ++i)
        cout << "=";
    cout << endl;

    cout << "How to use :" << endl;
    cout << "\t If you have a DIMACS CNF file to solve, please run this software as : " << endl;
    cout << "\t\t" << softName << " <file_name>" << endl;
    cout << "\t If you have a logic formula file to solve, please run this software as : " << endl;
    cout << "\t\t" << softName << " -tseitin <file_name>" << endl;
    cout << "\t At the moment, this software does not read standard input, but it will be solved in the next version." << endl << endl;

    cout << "\t If you want to generate a DIMAC CNF file, please type 1 and then [ENTER]" << endl;
    cout << "\t If tou want to read the about text of this software, please type 2 and then [ENTER]" << endl;
    cout << "\t To quit, please type 3 and then [ENTER]" << endl;    
    
} // displayMenu()

int main(int argc, char** argv)
{
    PARSE_TYPE parserType = CNF_PARSE;
    string fileName;

    if(argc == 1)
    {
        displayMenu(argv[0]);
        // Get the user choice
        unsigned short int choice; 
        bool ok = false;       
        do
        {        
            cin >> choice;
            if(choice == 1)
            {
                string fileName;
                cout << "Please enter a file name to save the formula: ";
                cin >> fileName;
                randomGenerator(fileName);
                ok = true;
            }
            else if(choice == 2)
            {
                cout << endl << endl << "SATSolver V " << VERSION << endl;
                cout << "Created by Tristan Sterin and Alexy Torres." << endl;
                cout << "Project realized at the ENS (Ecole Normale Superieure) of Lyon." << endl;
                cout << "Year 2015/2016" << endl;
                ok = true;
            }
            else if(choice == 3)
            {
                ok = true;
            }
            else
                cout << "Wrong input, please try again." << endl;

        } while(!ok);

        cout << "Thanks, bye friendly user!" << endl;
        return 0;
    }
    else if(argc == 3)
    {
        if(string(argv[1]) != "-tseitin")
        {
            cerr << "Error, wrong arguments." << endl << "Usage : " << argv[0] << " [-tseitin] <file_name>" << endl;
            return 1;
        }
        else
        {
            parserType = LOG_PARSE;
            fileName = argv[2];
        }
    }
    else if(argc != 2)
    {
        cerr << "Error, wrong arguments." << endl << "Usage : " << argv[0] << " [-tseitin] <file_name>" << endl;
        return 1;
    }
    else
        fileName = argv[1];   

    // Time computation
    std::clock_t start;
    double duration;
    start = std::clock();

    // Create the solver
    SATSolver solver(fileName);

    // Parse test
    if(!solver.parse(parserType))
    {
        cerr << "Error while parsing the file." << endl;
    }
    
    OUTDEBUG("We check SAT of :" << endl << solver.formulaToStr());

    if(solver.solve())
    {
        cout << "s SATIFIABLE" << endl;
        solver.showSolution();
    }
    else
        cout << "s UNSATISFIABLE" << endl;

    // Display time
    duration = ( std::clock() - start ) / (double) CLOCKS_PER_SEC;
    std::cout<<"Solved int: "<< duration << "s" << endl;

    return 0;
}// main(int, char**)
