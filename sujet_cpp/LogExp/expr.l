%{
#include <iostream>
#include <cstdlib>
using namespace std;
#define YY_DECL extern "C" int yylex()

#include "expr.tab.hpp"
%}
%%
[ \n\t]         ;
[1-9][0-9]*     { yylval.ival = atoi(yytext); return TK_VAR; } //don't want 0
"("             { return TK_LPAREN; }
")"             { return TK_RPAREN; }
"\/"            { return TK_DIS; }
"/\\"           { return TK_CON; }
"X"		{ return TK_XOR; }
"=>"            { return TK_IMP; }
"<=>"           { return TK_EQU; }
"~"             { return TK_ANT; } //Antithese
"-"             { return TK_NEG;}
"0"             { return TK_ZERO;}//End Of Formule
.               ;
%%
