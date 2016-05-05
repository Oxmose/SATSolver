/*
 *
 *  INTERFACE PARSER
 *
*/

#ifndef DEF_IPARSER_H
#define DEF_IPARSER_H

#include "../NewCore/SATSolver.h"

class IParser
{
    public:
        virtual ~IParser(){}
        virtual bool parse(SATSolver &p_solver, unsigned int &p_maxIndex) = 0;
        virtual bool tseitinResolution(std::map<unsigned int,int> &p_valuation, unsigned int &p_maxIndex) = 0;

    private:
}; // IParser

#endif // DEF_IPARSER_H
