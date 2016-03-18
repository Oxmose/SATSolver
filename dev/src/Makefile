#example name
NAME=expr
#compiler name
CC=g++
#lex name+path
LEX=flex
LIBLEX=-lfl
#yacc/bison
YACC=bison

all: Global/*.cpp BETHeuristic/*.cpp RandomSatExpGenerator/*.cpp CNFParser/*.cpp Core/*.cpp main.cpp LogExpParser/LOGParser.cpp LogExpParser/$(NAME).tab.cpp LogExpParser/$(NAME).yy.c LogExpParser/$(NAME).cpp
	$(CC) -std=c++11 -O3 -Wno-write-strings -fdiagnostics-color=always -o ../bin/resol $^ $(LIBLEX)

LogExpParser/$(NAME).yy.c :  LogExpParser/$(NAME).l
	$(LEX)  -o $@ $^

LogExpParser/$(NAME).tab.cpp : LogExpParser/$(NAME).ypp
	$(YACC) --report=all -o $@ -d $^

clean:
	rm -f *~ LogExpParser/$(NAME).yy.c LogExpParser/$(NAME).tab.cpp LogExpParser/$(NAME).tab.hpp ../bin/resol
