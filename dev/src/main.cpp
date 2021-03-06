// PROJECT INCLUDES
#include "Global/Global.h"
#include "RandomSatExpGenerator/randomGen.h"

// SAT CORE
#include "Core/SATSolver.h"

// PARSERS
#include "Parser/IParser.h"
#include "Parser/LOGParser.h"
#include "Parser/CNFParser.h"

// BET HEURISTICS
#include "BETHeuristic/IBet.h"
#include "BETHeuristic/DLISBet.h"
#include "BETHeuristic/MOMSBet.h"
#include "BETHeuristic/RandomBet.h"
#include "BETHeuristic/StandardBet.h"
#include "BETHeuristic/VSIDSBet.h"

// STD INCLUDES
#include <sys/ioctl.h>  //std::ioctl
#include <iostream>

// HEADER INCLUDE
#include "main.h"

using namespace std;

int main(int argc, char const *argv[])
{
    int commandError = parseCommand(argc, argv);
    SATSolver solver;

    if(settings_s.smtc_s)
        settings_s.disable_smt_cl_s = true;

    if(commandError == 1)
    {
        cerr << "Error, wrong arguments." << endl << "Usage : " << argv[0] << " [-tseitin] [-smte | -smtc | -smtd] [-disable_smt_cl] [-cl | -cl-interac] [-wl] [-vsids | -rand | -rand0 | -moms | -dlis | -dlis0] [-debug] <file_name>" << endl;
        return 1;
    }
    else if(commandError == 2)
    {
        cout << "If you have a DIMACS CNF file to solve, please run this software as:" << endl;
        cout << "\t" << argv[0] << " [arguments] <file_name>" << endl;
        cout << "If you have a logic SAT formula file to solve, please run this software as:" << endl;
        cout << "\t" << argv[0] << " [-tseitin] [arguments] <file_name>" << endl;
        cout << "If you have a SMTE/SMTC/SMTD formula file to solver, please run this software as:" << endl;
        cout << "\t" << argv[0] << " [-smte | -smtc | -smtd] [arguments] <file_name>" << endl;
        cout << endl << endl;

        cout << "Arguments order does not matter, the file name should always be given as last argument" << endl;
        cout << endl << endl;

        cout << "Parsers: " << endl;
        cout << "No arguments" << endl;
        cout << "\t Enable CNF DIMACS file parser" << endl;
        cout << "Argument [-tseitin]" << endl;
        cout << "\t Enable logical SAT formula parser" << endl;
        cout << "Argument [-smte | -smtc | -smtd] [-disable_smt_cl] " << endl;
        cout << "\t -smte : Enable SMT for equality" << endl;
        cout << "\t -smtc : Enable SMT for congruence" << endl;
        cout << "\t -smtd : Enable SMT for difference logic" << endl;
        cout << "\t -disable_smt_cl : Disable CL for smte" << endl;
        cout << endl << endl;


        cout << "Arguments [-wl] [-cl | -cl-interac] (can be combined)" << endl;
        cout << "\t -wl : enable watched litterals method" << endl;
        cout << "\t -cl : enable clause learning method" << endl;
        cout << "\t -cl-interac : enable clause learning with menu interaction" << endl;
        cout << endl << endl;

        cout << "Arguments [-vsids | -rand | -rand0 | -moms | -dlis | -dlis0]" << endl;
        cout << "\t -vsids : next bet on the most active literal in learned clauses" << endl;
        cout << "\t -rand : next bet on a random literal (bet is not random)" << endl;
        cout << "\t -rand0 :  next bet on a random literal (bet is random)" << endl;
        cout << "\t -moms : next bet on the literal which has the maximum occurences count in clauses of minimum size" << endl;
        cout << "\t -dlis : next bet on next literal which can satify the maximum count of clauses" << endl;
        cout << "\t -dlis0 : next bet on next literal which can satify the maximum count of clauses with score sum(pow(2, -C)) with C the size of the clauses the literal appears in" << endl;
        cout << endl << endl;

        cout << "Argument [-debug]" << endl;
        cout << "\t Enable debug output" << endl;
        return 0;
    }
    else if(commandError == -1)
        return 0;

    IParser *parser;
    if(settings_s.parser_s == CNF_PARSE)
        parser = new CNFParser(settings_s.filename_s);
    else
        parser = new LOGParser(settings_s.filename_s);
    
    unsigned int maxIndex = 0;
    parser->parse(solver, maxIndex);

    IBet *betStrat;
    switch(settings_s.bet_s)
    {
        case NORM:
            betStrat = new StandardBet();
            break;
        case RAND0:
            betStrat = new RandomBet(false);
            break;
        case RAND1:
            betStrat = new RandomBet(true);
            break;
        case MOMS:
            betStrat = new MOMSBet();
            break;
        case DLIS:
            betStrat = new DLISBet(false);
            break;
        case DLIS1:
            betStrat = new DLISBet(true);
            break;
        case VSIDS:
            betStrat = new VSIDSBet();
            break;
        default:
            betStrat = new StandardBet();            
    }

    solver.setBet(betStrat);

    if(solver.solve())
    {
        delete parser;
        delete betStrat;
        cout << "s UNSATISFIABLE" << endl;
        return 0;
    }

    cout << "s SATISFIABLE" << endl;
    if(settings_s.smte_s || settings_s.smtc_s)
    {
        solver.get_smt_solution();
        return 0;
    }

    if(settings_s.parser_s != CNF_PARSE)
    {
        parser->tseitinResolution(solver.valuation, maxIndex);
    }
    
    for(auto v : solver.valuation)
    {
        int index = v.first;
        bool value = (v.second == -1) ? true : v.second;
        string modifier = "";
        if(!value)
            modifier = "-";
        cout << modifier << index << " ";
    }
    cout << endl;

    delete parser;
    delete betStrat;

    return 0;
}

int parseCommand(int argc, const char **argv)
{
    // Init settings values
    settings_s.parser_s = CNF_PARSE;
    settings_s.bet_s = NORM;
    settings_s.debug_s = false;
    settings_s.wl_s = false;
    settings_s.cl_s = false;
    settings_s.clinterac_s = false;
    settings_s.forget_s = false;
    settings_s.smte_s = false;
    settings_s.smtc_s = false;
    settings_s.smtd_s = false;
    settings_s.disable_smt_cl_s = false;

    int commandError = 0;
    bool argsValid[8] = {false, false, false, false, false, false, false, false};
    
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

            if(value == "-help")
                return 2;

            if(i == argc - 1)
            {
                settings_s.filename_s = value;
            }
            else if(value == "-tseitin" && !argsValid[0])
            {
                settings_s.parser_s = LOG_PARSE;
                argsValid[0] = true;
            }
            else if(value == "-wl" && !argsValid[1])
            {
                settings_s.wl_s = true;
                argsValid[1] = true;
            }            
            else if(value == "-rand" && !argsValid[2])
            {
                settings_s.bet_s = RAND0;
                argsValid[2] = true;
            }
            else if(value == "-rand0" && !argsValid[2])
            {
                settings_s.bet_s = RAND1;
                argsValid[2] = true;
            }
            else if(value == "-moms" && !argsValid[2])
            {
                settings_s.bet_s = MOMS;
                argsValid[2] = true;
            }
            else if(value == "-dlis" && !argsValid[2])
            {
                settings_s.bet_s = DLIS;
                argsValid[2] = true;
            }
            else if(value == "-dlis0" && !argsValid[2])
            {
                settings_s.bet_s = DLIS1;
                argsValid[2] = true;
            }
            else if(value == "-vsids" && !argsValid[2])
            {
                settings_s.bet_s = VSIDS;
                argsValid[2] = true;
            }    
            else if(value == "-debug" && !argsValid[3])
            {
                settings_s.debug_s = true;
                argsValid[3] = true;
            }  
            else if(value == "-cl" && !argsValid[4])
            {
                settings_s.cl_s = true;
                argsValid[4] = true;
            }
            else if(value == "-cl-interac" && !argsValid[4])
            {
                settings_s.cl_s = true;
                settings_s.clinterac_s = true;
                argsValid[4] = true;
            }
            else if(value == "-forget" && !argsValid[5])
            {
                settings_s.forget_s = true;
                argsValid[5] = true;
            }
            else if(value == "-smte" && !argsValid[6])
            {
                settings_s.smte_s = true;
                settings_s.parser_s = LOG_PARSE;
                argsValid[6] = true;
            }
            else if(value == "-smtc" && !argsValid[6])
            {
                settings_s.smtc_s = true;
                settings_s.parser_s = LOG_PARSE;
                argsValid[6] = true;
            }
            else if(value == "-smtd" && !argsValid[6])
            {
                settings_s.smtd_s = true;
                settings_s.parser_s = LOG_PARSE;
                argsValid[6] = true;
            }
            else if(value == "-disable_smt_cl" && !argsValid[7])
            {
                settings_s.disable_smt_cl_s = true;
                argsValid[7] = true;
            }
            else
            {
                commandError = 1;
                break;
            }
        }
        int count = 0;
        for(unsigned int i = 0; i < 8; ++i)
        {
            if(argsValid[i])
                ++count;
        }
        if(settings_s.disable_smt_cl_s && (!settings_s.smtc_s && !settings_s.smte_s))
        {
            OUTWARNING("Can't use disable_smt_cl without smt");
            exit(0);
        }
        if((settings_s.bet_s == VSIDS || settings_s.forget_s) && !settings_s.cl_s)
        {
            OUTWARNING("Imcompatible heuristic, you must use -cl to enable this heuristic, set heuristic to standard.");
            settings_s.bet_s = NORM;
        }
        if(count != argc - 2)
        {
            commandError = 1;
        }
        return commandError;
    }

    return commandError;
}// int parseComment(int, char**, Settings_s&)

void displayMenu(const char *softName)
{
    struct winsize w;
    ioctl(0, TIOCGWINSZ, &w);

    unsigned int total = 0;

    for(int i = 0; i < w.ws_col / 2 - 8; ++i)
    {
        cout << "=";
        ++total;
    }
    
    cout << " TSAT V" << VERSION << " ";
    total += 11;
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

    cout << "---- TSAT MENU ";
    for(int i = 0; i < w.ws_col - 15; ++i)
    {
        cout << "-";
    }
    cout << endl;
    cout << "| 1. Generate DIMAC CNF file" << endl;
    cout << "| 2. Generate SMTE logical file" << endl;
    cout << "| 3. Read the credits" << endl;
    cout << "| 4. List available arguments" << endl;
    cout << "| 5. Quit" << endl;

    cout << endl << "---- TSAT PROMPT ";
    for(int i = 0; i < w.ws_col - 17; ++i)
    {
        cout << "-";
    }

} // displayMenu()

void menuChoice()
{
    // Get the user choice
    unsigned short int choice;
    bool ok = false;
    do
    {
        cout << "> ";
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
            randomSMTEGen();
        }
        else if(choice == 3)
        {
            cout << endl << endl << "TSAT V " << VERSION << endl;
            cout << "Created by Tristan Sterin and Alexy Torres." << endl;
            cout << "Project realized at the ENS (Ecole Normale Superieure) of Lyon." << endl;
            cout << "Year 2015/2016" << endl;
            ok = true;
        }
        else if(choice == 4)
        {
            cout << "Parsers: " << endl;
            cout << "No arguments" << endl;
            cout << "\t Enable CNF DIMACS file parser" << endl;
            cout << "Argument [-tseitin]" << endl;
            cout << "\t Enable logical SAT formula parser" << endl;
            cout << "Argument [-smte | -smtc | -smtd] [-disable_smt_cl]" << endl;
            cout << "\t -smte : Enable SMT for equality" << endl;
            cout << "\t -smtc : Enable SMT for congruence" << endl;
            cout << "\t -smtd : Enable SMT for difference logic" << endl;
            cout << "\t -disable_smt_cl : Disable CL for smte" << endl;
            cout << endl << endl;


            cout << "Arguments [-wl] [-cl | -cl-interac] (can be combined)" << endl;
            cout << "\t -wl : enable watched litterals method" << endl;
            cout << "\t -cl : enable clause learning method" << endl;
            cout << "\t -cl-interac : enable clause learning with menu interaction" << endl;
            cout << endl << endl;

            cout << "Arguments [-vsids | -rand | -rand0 | -moms | -dlis | -dlis0]" << endl;
            cout << "\t -vsids : next bet on the most active literal in learned clauses" << endl;
            cout << "\t -rand : next bet on a random literal (bet is not random)" << endl;
            cout << "\t -rand0 :  next bet on a random literal (bet is random)" << endl;
            cout << "\t -moms : next bet on the literal which has the maximum occurences count in clauses of minimum size" << endl;
            cout << "\t -dlis : next bet on next literal which can satify the maximum count of clauses" << endl;
            cout << "\t -dlis0 : next bet on next literal which can satify the maximum count of clauses with score sum(pow(2, -C)) with C the size of the clauses the literal appears in" << endl;
            cout << endl << endl;

            cout << "Argument [-debug]" << endl;
            cout << "\t Enable debug output" << endl;
        }
        else if(choice == 5)
        {
            ok = true;
        }
        else
            cout << "Wrong input, please try again." << endl;

    } while(!ok);

    cout << "Thanks, bye friendly user!" << endl;
} // menuChoice()
