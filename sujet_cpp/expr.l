%{
#include <iostream>
#include <cstdlib>
using namespace std;
#define YY_DECL extern "C" int yylex()

#include "expr.tab.hpp"
%}
%%
[ \t\n]         ;
[0-9]+          { yylval.ival = atoi(yytext); return TK_INT; }
"("             { return TK_LPAREN; }
")"             { return TK_RPAREN; }
"+"             { return TK_PLUS; }
"*"             { return TK_MULT; }
.               ;
%%
