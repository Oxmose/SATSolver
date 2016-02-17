// SATSolver class
#include "Core/SATSolver.h"

// STD INCLUDES
#include <ctime>        //std::clock
#include <iostream>     //std::cout std::cerr
#include <sys/ioctl.h>  //std::ioctl

// OTHERS INCLUDES FROM PROJECT
#include "RandomSatExpGenerator/randomGen.h"

// GLOBAL FLAGS/VARS
#include "Global/Global.h"

using namespace std;

#define VERSION "0.1"

bool debugFlag;

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
    cout << "\t\t" << softName << " <file_name> [-debug]" << endl;
    cout << "\t If you have a logic formula file to solve, please run this software as : " << endl;
    cout << "\t\t" << softName << " -tseitin <file_name> [-debug]" << endl;

    cout << "\t To generate DIMAC CNF file, press 1 and then [ENTER]" << endl;
    cout << "\t To read the credits, press 2 and then [ENTER]" << endl;
    cout << "\t To quit, press 3 and then [ENTER]" << endl;

} // displayMenu()

int main(int argc, char** argv)
{
    PARSE_TYPE parserType = CNF_PARSE;
    string fileName;
    debugFlag = false;

    if(argc == 1)
    {
        displayMenu(argv[0]);

        // Get the user choice
        unsigned short int choice;
        bool ok = false;
        do
        {
            cout << ">";
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
        if(string(argv[1]) != "-tseitin" && string(argv[2]) != "-debug")
        {
            cerr << "Error, wrong arguments." << endl << "Usage : " << argv[0] << " [-tseitin] <file_name> [-debug]" << endl;
            return 1;
        }
        else
        {
            int index = 1;
            if(string(argv[2]) == "-debug")
            {
                parserType = CNF_PARSE;
		        debugFlag = true;
            }
            else
            {
                index = 2;
                parserType = LOG_PARSE;
            }
            fileName = argv[index];
        }
    }
    else if(argc == 4)
    {
        if(string(argv[1]) != "-tseitin")
        {
            cerr << "Error, wrong arguments." << endl << "Usage : " << argv[0] << " [-tseitin] <file_name> [-debug]" << endl;
            return 1;
        }
	    else if(string(argv[3]) != "-debug")
	    {
		    cerr << "Error, wrong arguments." << endl << "Usage : " << argv[0] << " [-tseitin] <file_name> [-debug]" << endl;
            return 1;
	    }
        else
        {
            debugFlag = true;
            parserType = LOG_PARSE;
            fileName = argv[2];
        }
    }
    else if(argc != 2)
    {
        cerr << "Error, wrong arguments." << endl << "Usage : " << argv[0] << " [-tseitin] <file_name> [-debug]" << endl;
        return 1;
    }
    else
        fileName = argv[1];

    // Time computation
    std::clock_t start;
    double duration;
    start = std::clock();

    OUTDEBUG("DEBUG ENABLED");

    // Create the solver
    SATSolver solver(fileName);

    // Parse test
    if(!solver.parse(parserType))
    {
        cerr << "[WARNING]Errors while parsing the file." << endl;
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
    std::cout<<"Solved in: "<< duration << "s" << endl;

    return 0;
}// main(int, char**)
