/*
 *
 *  HEADER Main
 *
*/

#ifndef DEF_MAIN_H
#define DEF_MAIN_H

// SATSolver class
#include "Core/SATSolver.h"

// STD INCLUDES
#include <ctime>        //std::clock
#include <string>       //std::string
#include <iostream>     //std::cout std::cerr
#include <sys/ioctl.h>  //std::ioctl

// OTHERS INCLUDES FROM PROJECT
#include "RandomSatExpGenerator/randomGen.h"

// GLOBAL FLAGS/VARS
#include "Global/Global.h"

// HEADER FILE
#include "main.h"

#define VERSION "0.1"

bool debugFlag;

int main(int argc, char** argv);

int parseCommand(int argc, char **argv, std::string &fileName, PARSE_TYPE &parserType, bool &debugFlag);
void displayMenu(char *softName);
void menuChoice();

#endif // DEF_MAIN_H
