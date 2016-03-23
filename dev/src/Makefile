#expr name
NAME=Parser/expr
#compiler name
CC=g++
#compiler flags
FLAGS=-std=c++11 -O3 -Wno-write-strings -Wall -o 
#lex name+path
LEX=flex
LIBLEX=-lfl
#yacc/bison
YACC=bison
#bin target
TARGET=../bin/resol
#obj folder
OBJ=../obj

all: Global/*.cpp BETHeuristic/*.cpp RandomSatExpGenerator/*.cpp Parser/*.cpp Core/*.cpp main.cpp $(NAME).tab.cpp $(NAME).yy.c $(NAME).cpp
	$(CC) $(FLAGS) $(TARGET) $^ $(LIBLEX)

$(NAME).yy.c :  $(NAME).l
	$(LEX)  -o $@ $^

$(NAME).tab.cpp : $(NAME).ypp
	$(YACC) --report=all -o $@ -d $^

clean:
	rm -f *~ $(NAME).yy.c $(NAME).tab.cpp $(NAME).tab.hpp $(TARGET) $(OBJ)
