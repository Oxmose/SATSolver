/*
**
**  Random SMTE formula generator
**
*/

#ifndef DEF_RANDOMSMTEGENERATOR_H
#define DEF_RANDOMSMTEGENERATOR_H

// STD INCLUDES
#include <iostream>     // std::cin std::cout
#include <algorithm>    // std::transform
#include <string>       // std::string
#include <cstdlib>     	// std::srand std::rand
#include <ctime>       	// std::time
#include <fstream>     	// std::ofstream

void randomGenerator(const std::string &p_fileName);

#endif // DEF_RANDOMSMTEGENERATOR_H
