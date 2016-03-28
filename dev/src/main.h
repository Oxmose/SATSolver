/*
 *
 *  HEADER Main
 *
*/

#ifndef DEF_MAIN_H
#define DEF_MAIN_H

// SATSolver class
#include "Core/SATSolver.h"
#include "Core/SATSolverSTD.h"
#include "Core/SATSolverWL.h" 

// Parser interface / classes
#include "Parser/IParser.h"
#include "Parser/CNFParser.h"
#include "Parser/LOGParser.h"

// Bet heuristics classes
#include "BETHeuristic/IBet.h"          // Bet Heuristic Interface
#include "BETHeuristic/StandardBet.h"   // Standard bet heuristic
#include "BETHeuristic/RandomBet.h"        // Random bet heuristic
#include "BETHeuristic/DLISBet.h"        // DLIS bet heuristic
#include "BETHeuristic/MOMSBet.h"        // MOMS bet heuristic

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

// Settings structure
typedef struct Settings_s Settings_s;
struct Settings_s
{
    std::string filename_s;
    PARSE_TYPE parser_s;
    BET_METHOD bet_s;
    bool debug_s;
    bool wl_s;
    bool cl_s;
    bool clint_s;

};

bool debugFlag;

int main(int argc, char** argv);

int parseCommand(int argc, char **argv, Settings_s &sets);
void displayMenu(char *softName);
void menuChoice();

#endif // DEF_MAIN_H
