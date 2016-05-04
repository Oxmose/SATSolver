/*
 * GLOBAL VARS
*/

#ifndef DEF_GLOBAL_H
#define DEF_GLOBAL_H
#include <string>


#define VERSION "4.0"

/*
#define OUTDEBUG(Out) do {  \
    if(debugFlag) { \
        std::cerr << "[DEBUG] " << Out << std::endl; \
    } \
} while(0)

#define OUTERROR(Out) do {  \
    std::cerr << "[ERROR] " << Out << std::endl; \
    exit(EXIT_FAILURE); \
} while(0)

#define OUTWARNING(Out) do {  \
    std::cerr << "[WARNING] " << Out << std::endl; \
} while(0)
*/


// Settings structure
struct settings_s
{
    std::string filename_s;
    //PARSE_TYPE parser_s;
    //BET_METHOD bet_s;
    bool debug_s;
    bool wl_s;
    bool cl_s;
    bool clinterac_s;
    bool forget_s;
   // bool naiveuip_s;
};

extern struct settings_s settings_s;

#endif // DEF_GLOBAL_H
