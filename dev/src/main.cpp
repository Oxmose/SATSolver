// SATSolver class
#include "Core/SATSolver.h"

// Parsers classes
#include "CNFParser/CNFParser.h"
#include "LogExpParser/LOGParser.h"

// Bet heuristics classes
#include "BETHeuristic/IBet.h"          // Bet Heuristic Interface
#include "BETHeuristic/StandardBet.h"   // Standard bet heuristic
#include "BETHeuristic/RandomBet.h"    	// Random bet heuristic
#include "BETHeuristic/DLISBet.h"    	// DLIS bet heuristic
#include "BETHeuristic/MOMSBet.h"    	// MOMS bet heuristic

// STD INCLUDES
#include <ctime>        //std::clock
#include <string>       //std::string
#include <iostream>     //std::cout std::cerr
#include <sys/ioctl.h>  //std::ioctl

// OTHERS INCLUDES FROM PROJECT
#include "RandomSatExpGenerator/randomGen.h"
#include "Core/Clause.h"

// GLOBAL FLAGS/VARS
#include "Global/Global.h"

// HEADER FILE
#include "main.h"

using namespace std;

int main(int argc, char** argv)
{
    // Define parse type and file to parse
    PARSE_TYPE parserType = CNF_PARSE;
    string fileName;

    // Init debug flag
    debugFlag = false;
    bool watchedLitMeth;
    BET_METHOD betMeth;

    // Parse the command line
    int commandError = parseCommand(argc, argv, fileName, parserType, debugFlag, watchedLitMeth, betMeth);


    if(commandError == 1)
    {
        cerr << "Error, wrong arguments." << endl << "Usage : " << argv[0] << " [-tseitin] [-wl] [-rand | -rand0 | -moms | -dlis | -dlis0] [-debug] <file_name>" << endl;
        return 1;
    }
    else if(commandError == -1)
        return 0;

    // Time computation
    clock_t start;
    double duration;
    start = clock();

    OUTDEBUG("DEBUG ENABLED");

    // Create the solver
    SATSolver solver(watchedLitMeth);

    // Set strategy
    switch(betMeth)
    {
        case NORM:
            solver.setStrategy(new StandardBet());
            break;
        case RAND0:
            solver.setStrategy(new RandomBet(false));
            break;
        case RAND1:
            solver.setStrategy(new RandomBet(true));
            break;
        case MOMS:
            solver.setStrategy(new MOMSBet());
            break;
        case DLIS:
            solver.setStrategy(new DLISBet(false));
            break;
        case DLIS1:
            solver.setStrategy(new DLISBet(true));
            break;
        default:
            solver.setStrategy(new StandardBet());
    }
    // Create the LOGParser
    LOGParser logParser(fileName);
    unsigned int maxIndex;
    ClauseSet initClausesSet(compareUnsat);


    if(parserType == CNF_PARSE)
    {
        // Create parser and parse CNF formula
        CNFParser parser(fileName);
        if(!parser.parse(maxIndex, initClausesSet))
            OUTWARNING("Errors while parsing the file.");
        solver.setMaxIndex(maxIndex);
        solver.setOriginFormula(initClausesSet);
    }
    else
    {
        // Parse logExpFormula
        if(!logParser.parse(maxIndex, initClausesSet))
            OUTWARNING("Errors while parsing the file.");
        solver.setMaxIndex(maxIndex);
        solver.setOriginFormula(initClausesSet);
    }

    OUTDEBUG("We check SAT of :" << solver.formulaToStr());

    // Solve SAT formula
    if(solver.solve())
    {
        cout << "s SATIFIABLE" << endl;
        if(parserType == CNF_PARSE)
            solver.showSolution();
        else
            solver.showSolution(logParser);
    }
    else
        cout << "s UNSATISFIABLE" << endl;

    /*
        N.B:There's no "s ???" output as we do not impose any time limit or run
        any bizarre heuristic.
     */

    // Display time
    duration = ( std::clock() - start ) / (double) CLOCKS_PER_SEC;
    OUTDEBUG("Solved in: "<< duration << "s");

    // Only for loop instances (may be added later)
    // solver.reset();

    return 0;
}// main(int, char**)

int parseCommand(int argc, char **argv, string &fileName, PARSE_TYPE &parserType, bool &debugFlag, bool &watchedLitMeth, BET_METHOD &betMeth)
{
    
    watchedLitMeth = false;
    betMeth = NORM;
    int commandError = 0;
    parserType = CNF_PARSE;
    bool argsValid[4] = {false, false, false, false};

    
    if(argc == 1)
    {
        // Display user interface and enable user to choose the action
        displayMenu(argv[0]);
        menuChoice();
        return -1;
    }
    else
    {
        for(unsigned int i = 1; i < argc; ++i)
        {
            string value = string(argv[i]);
            if(i == argc - 1)
            {
                fileName = value;
            }
            else if(value == "-tseitin" && !argsValid[0])
            {
                parserType = LOG_PARSE;
                argsValid[0] = true;
            }
            else if(value == "-wl" && !argsValid[1])
            {
                watchedLitMeth = true;
                argsValid[1] = true;
            }
            else if(value == "-rand" && !argsValid[2])
            {
                betMeth = RAND0;
                argsValid[2] = true;
            }
            else if(value == "-rand0" && !argsValid[2])
            {
                betMeth = RAND1;
                argsValid[2] = true;
            }
            else if(value == "-moms" && !argsValid[2])
            {
                betMeth = MOMS;
                argsValid[2] = true;
            }
            else if(value == "-dlis" && !argsValid[2])
            {
                betMeth = DLIS;
                argsValid[2] = true;
            }
            else if(value == "-dlis0" && !argsValid[2])
            {
                betMeth = DLIS1;
                argsValid[2] = true;
            }
            else if(value == "-debug" && !argsValid[3])
            {
                debugFlag = true;
                argsValid[3] = true;
            }            
            else
            {
                commandError = 1;
                break;
            }
        }
        unsigned int count = 0;
        for(unsigned int i = 0; i < 4; ++i)
        {
            if(argsValid[i])
                ++count;
        }
        if(count != argc - 2)
        {
            commandError = 1;
        }
        return commandError;
    }

    return commandError;
}

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
    cout << "\t\t" << softName << " [-wl] [-rand | -moms | -dlis] [-debug] <file_name>" << endl;
    cout << "\t If you have a logic formula file to solve, please run this software as : " << endl;
    cout << "\t\t" << softName << " [-tseitin] [-wl] [-rand | -moms | -dlis] [-debug] <file_name>" << endl;
    cout << "\t The argument -wl enable the watched literals method." << endl;
    cout << "\t [-rand | -moms | -dlis] define the used heuristic to bet on the next literal :" << endl;
    cout << "\t\t -rand : next bet on a random literal." << endl;
    cout << "\t\t -moms : next bet on the variable which has the maximum occurences count in clauses of minimum size." << endl;
    cout << "\t\t -dlis : next bet on next variable which can satify the maximum count of clauses." << endl;

    cout << "\t To generate DIMAC CNF file, press 1 and then [ENTER]" << endl;
    cout << "\t To read the credits, press 2 and then [ENTER]" << endl;
    cout << "\t To quit, press 3 and then [ENTER]" << endl;

} // displayMenu()

void menuChoice()
{
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
} // menuChoice()
