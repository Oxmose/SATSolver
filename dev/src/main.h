/*
 *
 *  HEADER Main
 *
*/

#ifndef DEF_MAIN_H
#define DEF_MAIN_H

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

bool debugFlag;

int main(int argc, char** argv);

int parseCommand(int argc, char **argv, std::string &fileName, PARSE_TYPE &parserType, bool &debugFlag, bool &watchedLitMeth, BET_METHOD &betMeth);
void displayMenu(char *softName);
void menuChoice();

#endif // DEF_MAIN_H
