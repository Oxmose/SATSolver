/*
 * GLOBAL VARS
*/

#ifndef DEF_GLOBAL_H
#define DEF_GLOBAL_H

#define OUTDEBUG(Out) do {  \
    if(debugFlag) { \
        std::cerr << "[DEBUG] " << Out << std::endl; \
    } \
} while(0)

// Global debug flag
extern bool debugFlag;

#endif // DEF_GLOBAL_H
