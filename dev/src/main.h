/*
 *
 *  HEADER Main
 *
*/

#ifndef DEF_MAIN_H
#define DEF_MAIN_H

#include "Global/Global.h"
#include "NewCore/SATSolver.h"

#include <sys/ioctl.h>  //std::ioctl
#include <iostream>

int main(int argc, char const *argv[]);

void read_input(string file, SATSolver& solver);

int parseCommand(int argc, const char **argv);
void displayMenu(const char *softName);
void menuChoice();

#endif // DEF_MAIN_H
