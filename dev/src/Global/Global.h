/*
 * GLOBAL VARS
*/

#ifndef DEF_GLOBAL_H
#define DEF_GLOBAL_H

#include <string>
#include "../BETHeuristic/IBet.h"

#define VERSION "4.0"

#define OUTWARNING(Out) do {  \
    std::cerr << "[WARNING] " << Out << std::endl; \
} while(0)

// Parser TYPE enum
enum PARSE_TYPE
{
    CNF_PARSE,
    LOG_PARSE
};

// Settings structure
struct settings_s
{
    std::string filename_s;
    PARSE_TYPE parser_s;
    BET_METHOD bet_s;
    bool debug_s;
    bool wl_s;
    bool cl_s;
    bool clinterac_s;
    bool forget_s;
    bool smte_s;
    bool smtc_s;
    bool disable_smt_cl_s;
};

extern struct settings_s settings_s;

#endif // DEF_GLOBAL_H
