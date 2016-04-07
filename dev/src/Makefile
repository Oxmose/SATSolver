#expr name
NAME=Parser/expr

#compiler name
CC=g++

#compiler flags
CFLAGS=-std=c++11 -O3 -Wno-write-strings -Wall

#lex name+path
LEX=flex
LIBLEX=-lfl

#yacc/bison
YACC=bison

#bin target
TARGET=../bin/resol

#SRC
GLOBAL=Global/Global.cpp
BET=BETHeuristic/DLISBet.cpp BETHeuristic/MOMSBet.cpp BETHeuristic/RandomBet.cpp BETHeuristic/StandardBet.cpp
RANDOMGEN=RandomSatExpGenerator/randomGen.cpp
MAIN=main.cpp
CORE=Core/Clause.cpp Core/ConflictGraph.cpp Core/SATSolver.cpp Core/SATSolverCL.cpp Core/SATSolverSTD.cpp Core/SATSolverWL.cpp
PARSER=Parser/CNFParser.cpp Parser/LOGParser.cpp $(NAME).cpp $(NAME).tab.cpp $(NAME).yy.c
PRELOG=$(NAME).tab.cpp $(NAME).yy.c


SRCS= $(GLOBAL) $(BET) $(RANDOMGEN) $(CORE) $(MAIN) $(PARSER) 

#OBJS
OBJS=$(SRCS:.cpp=.o)

all: $(TARGET)
		@echo Compiling

$(TARGET): $(PRELOG) $(OBJS)
		$(CC) $(CFLAGS) -o $(TARGET) $(OBJS) $(LIBLEX)

.cpp.o:
		$(CC) $(CFLAGS) -c $<  -o $@

$(NAME).yy.c :  $(NAME).l
		$(LEX)  -o $@ $^

$(NAME).tab.cpp : $(NAME).ypp
		$(YACC) --report=all -o $@ -d $^

clean:
		rm -f $(NAME).yy.c $(NAME).tab.cpp $(NAME).tab.hpp $(OBJS) $(TARGET)
