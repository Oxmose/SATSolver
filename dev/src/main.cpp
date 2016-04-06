// SATSolver class
#include "Core/SATSolver.h"
#include "Core/SATSolverSTD.h"
#include "Core/SATSolverWL.h"
#include "Core/SATSolverCL.h"

// Parser interface / classes
#include "Parser/IParser.h"
#include "Parser/CNFParser.h"
#include "Parser/LOGParser.h"

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
    // Init settings
    Settings_s sets;

    // Parse the command line
    int commandError = parseCommand(argc, argv, sets);
    
    if(commandError == 1)
    {
        cerr << "Error, wrong arguments." << endl << "Usage : " << argv[0] << " [-tseitin] [-cl | -cl-interact] [-wl] [-rand | -rand0 | -moms | -dlis | -dlis0] [-debug] <file_name>" << endl;
        return 1;
    }
    else if(commandError == -1)
        return 0;
    
    debugFlag = sets.debug_s;
    // Time computation
    clock_t start;
    double duration;
    start = clock();

    OUTDEBUG("DEBUG ENABLED");

    // Create the solver
    unique_ptr<SATSolver> solver;
    if(!sets.wl_s)
        solver = unique_ptr<SATSolver>(new SATSolverSTD());
    else 
        solver = unique_ptr<SATSolver>(new SATSolverWL());
    solver = unique_ptr<SATSolver>(new SATSolverCL());
    
    // Set strategy
    shared_ptr<IBet> betStrat;
    switch(sets.bet_s)
    {
        case NORM:
            betStrat = shared_ptr<IBet>(new StandardBet());
            break;
        case RAND0:
            betStrat = shared_ptr<IBet>(new RandomBet(false));
            break;
        case RAND1:
            betStrat = shared_ptr<IBet>(new RandomBet(true));
            break;
        case MOMS:
            betStrat = shared_ptr<IBet>(new MOMSBet());
            break;
        case DLIS:
            betStrat = shared_ptr<IBet>(new DLISBet(false));
            break;
        case DLIS1:
            betStrat = shared_ptr<IBet>(new DLISBet(true));
            break;
        default:
            betStrat = shared_ptr<IBet>(new StandardBet());
    }
    solver->setStrategy(betStrat);

    shared_ptr<IParser> parser;
    switch(sets.parser_s)
    {
        case CNF_PARSE:
            parser = shared_ptr<IParser>(new CNFParser(sets.filename_s));
            break;
        case LOG_PARSE:
            parser = shared_ptr<IParser>(new LOGParser(sets.filename_s));
            break;
        default:
            parser = shared_ptr<IParser>(new CNFParser(sets.filename_s));
    }

    unsigned int maxIndex;
    vector<Clause> clauses;

    if(!parser->parse(maxIndex, clauses))
        OUTWARNING("Wranings while parsing the file.");
    solver->setMaxIndex(maxIndex);
    solver->setOriginFormula(clauses);
    solver->setParser(parser);

    OUTDEBUG("We check SAT of :" << solver->formulaToStr());

    // Solve SAT formula
    if(solver->solve())
    {
        cout << "s SATISFIABLE" << endl;
        solver->showSolution();
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
    // solver->reset();

    return 0;
}// main(int, char**)

int parseCommand(int argc, char **argv, Settings_s &sets)
{
    // Init settings values
    sets.parser_s = CNF_PARSE;
    sets.bet_s = NORM;
    sets.debug_s = false;
    sets.wl_s = false;
    sets.cl_s = false;
    sets.clint_s = false;
    
    int commandError = 0;
    bool argsValid[5] = {false, false, false, false, false};
    
    if(argc == 1)
    {
        // Display user interface and enable user to choose the action
        displayMenu(argv[0]);
        menuChoice();
        return -1;
    }
    else
    {
        for(int i = 1; i < argc; ++i)
        {
            string value = string(argv[i]);
            if(i == argc - 1)
            {
                sets.filename_s = value;
            }
            else if(value == "-tseitin" && !argsValid[0])
            {
                sets.parser_s = LOG_PARSE;
                argsValid[0] = true;
            }
            else if(value == "-wl" && !argsValid[1])
            {
                sets.wl_s = true;
                argsValid[1] = true;
            }
            else if(value == "-rand" && !argsValid[2])
            {
                sets.bet_s = RAND0;
                argsValid[2] = true;
            }
            else if(value == "-rand0" && !argsValid[2])
            {
                sets.bet_s = RAND1;
                argsValid[2] = true;
            }
            else if(value == "-moms" && !argsValid[2])
            {
                sets.bet_s = MOMS;
                argsValid[2] = true;
            }
            else if(value == "-dlis" && !argsValid[2])
            {
                sets.bet_s = DLIS;
                argsValid[2] = true;
            }
            else if(value == "-dlis0" && !argsValid[2])
            {
                sets.bet_s = DLIS1;
                argsValid[2] = true;
            }
            else if(value == "-debug" && !argsValid[3])
            {
                sets.debug_s = true;
                argsValid[3] = true;
            }  
            else if(value == "-cl" && !argsValid[4])
            {
                sets.cl_s = true;
                argsValid[4] = true;
            }
            else if(value == "-cl-interact" && !argsValid[4])
            {
                sets.cl_s = true;
                sets.clint_s = true;
                argsValid[4] = true;
            }
            else
            {
                commandError = 1;
                break;
            }
        }
        int count = 0;
        for(unsigned int i = 0; i < 5; ++i)
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
}// int parseComment(int, char**, Settings_s&)

void displayMenu(char *softName)
{
    struct winsize w;
    ioctl(0, TIOCGWINSZ, &w);

    unsigned int total = 0;

    for(int i = 0; i < w.ws_col / 2 - 8; ++i)
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
    for(int i = 0; i < w.ws_col / 2 - 19; ++i)
    {
        cout << " ";
        ++total;
    }
    cout << " By Tristan Sterin and Alexy Torres ";
    total += 36;
    for(int i = total; i < w.ws_col - 2; ++i)
        cout << " ";
    cout << "#" << endl;
    for(int i = 0; i < w.ws_col; ++i)
        cout << "=";
    cout << endl;

    cout << "How to use :" << endl;
    cout << "\t If you have a DIMACS CNF file to solve, please run this software as : " << endl;
    cout << "\t\t" << softName << " [-wl] [-cl | -cl-interact] [-rand | -rand0 | -moms | -dlis | -dlis0] [-debug] <file_name>" << endl;
    cout << "\t If you have a logic formula file to solve, please run this software as : " << endl;
    cout << "\t\t" << softName << " [-tseitin] [-wl] [-cl | -cl-interact] [-rand | -rand0 | -moms | -dlis | -dlis0] [-debug] <file_name>" << endl;
    cout << "\t The argument -wl enable the watched literals method." << endl;
    cout << "\t The argument -cl enable clauses learning. -cl-interact enable interactive clauses learning." << endl;
    cout << "\t [-rand | -moms | -dlis] define the used heuristic to bet on the next literal :" << endl;
    cout << "\t\t -rand : next bet on a random literal." << endl;
    cout << "\t\t -rand0 : next bet on a random literal (the bet is random here)." << endl;
    cout << "\t\t -moms : next bet on the variable which has the maximum occurences count in clauses of minimum size." << endl;
    cout << "\t\t -dlis : next bet on next variable which can satify the maximum count of clauses." << endl;
    cout << "\t\t -dlis0 : next bet on next variable which can satify the maximum count of clauses with score sum(pow(2, -C)) with C the size of the clauses the variable appears in." << endl;

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
