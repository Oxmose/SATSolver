#example name
NAME=expr
#compiler name
CC=g++
#lex name+path
LEX=flex
LIBLEX=-lfl
#yacc/bison
YACC=bison

all: Global/*.cpp BETHeuristic/*.cpp RandomSatExpGenerator/*.cpp Parser/*.cpp Core/*.cpp main.cpp Parser/$(NAME).tab.cpp Parser/$(NAME).yy.c Parser/$(NAME).cpp
	$(CC) -std=c++11 -O3 -Wno-write-strings -fdiagnostics-color=always -o ../bin/resol $^ $(LIBLEX)

Parser/$(NAME).yy.c :  Parser/$(NAME).l
	$(LEX)  -o $@ $^

Parser/$(NAME).tab.cpp : Parser/$(NAME).ypp
	$(YACC) --report=all -o $@ -d $^

clean:
	rm -f *~ Parser/$(NAME).yy.c Parser/$(NAME).tab.cpp Parser/$(NAME).tab.hpp ../bin/resol
