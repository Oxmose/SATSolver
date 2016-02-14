/*
**
**  Random SAT formula generator
**
*/

#ifndef DEF_RANDOMGENERATOR_H
#define DEF_RANDOMGENERATOR_H

// STD INCLUDES
#include <iostream>     // std::cin std::cout
#include <algorithm>    // std::transform
#include <string>       // std::string
#include <cstdlib>     	// std::srand std::rand
#include <ctime>       	// std::time
#include <fstream>     	// std::ofstream

void randomGenerator(const std::string &p_fileName);

#endif // DEF_RANDOMGENERATOR_H
