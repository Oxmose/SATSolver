/*
 *
 *  HEADER Main
 *
*/

#ifndef DEF_MAIN_H
#define DEF_MAIN_H

// PROJECT INCLUDES
#include "Global/Global.h"
#include "RandomSatExpGenerator/randomGen.h"

// SAT CORE
#include "NewCore/SATSolver.h"

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

int main(int argc, char const *argv[]);

int parseCommand(int argc, const char **argv);
void displayMenu(const char *softName);
void menuChoice();

#endif // DEF_MAIN_H
