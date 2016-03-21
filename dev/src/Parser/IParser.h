/*
 *
 *  INTERFACE PARSER
 *
*/

#ifndef DEF_IPARSER_H
#define DEF_IPARSER_H


enum PARSE_TYPE
{
    CNF_PARSE,
    LOG_PARSE
};

class IParser
{
    public:
        virtual ~IParser(){}
        virtual bool parse(unsigned int &p_maxIndex, std::vector<Clause>& p_clauses) = 0;
        virtual bool tseitinResolution(std::map<int,int> &p_valuation, unsigned int &p_maxIndex) = 0;

    private:
}; // IParser

#endif // DEF_IPARSER_H
