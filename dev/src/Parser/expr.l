%{
#include <iostream>
#include <cstdlib>
using namespace std;
#define YY_DECL extern "C" int yylex()

#include "expr.tab.hpp"
%}
%%
[ \t]         {}
[1-9][0-9]*     { yylval.ival = atoi(yytext); return TK_VAR; }
"("             { return TK_LPAREN; }
")"             { return TK_RPAREN; }
"\/"            { return TK_DIS; }
"/\\"           { return TK_CON; }
"\n"            { return TK_CON;}
"X"		{ return TK_XOR; }
"=>"            { return TK_IMP; }
"<=>"           { return TK_EQU; }
"~"             { return TK_ANT; }
"-"             { return TK_NEG; }
"="             { return TK_EQUA; }
"!="            { return TK_NEQUA; }
","		{ return TK_COMMA; }
"<"		{ return TK_LTH; }
">"		{ return TK_GTH; }
"<="		{ return TK_LEQ; }
">="		{ return TK_GEQ; }
x[1-9][0-9]*  { yylval.name = yytext; return TK_LOVAR; }
[a-z]+          { yylval.name = yytext; return TK_FUN; }
"0"("\n"*)            { return TK_ZERO;}//End Of Formule, the HEX 0a is a pain in the 
.               ;
%%
