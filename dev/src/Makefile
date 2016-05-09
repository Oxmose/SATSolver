#expr name
EXPRNAME=Parser/expr

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
BET=BETHeuristic/DLISBet.cpp BETHeuristic/MOMSBet.cpp BETHeuristic/RandomBet.cpp BETHeuristic/StandardBet.cpp BETHeuristic/VSIDSBet.cpp
RANDOMGEN=RandomSatExpGenerator/randomGen.cpp
MAIN=main.cpp
CORE=NewCore/clause.cpp NewCore/conflict_graph.cpp NewCore/SATSolver.cpp
SMT=NewCore/SMT/SMTSolver_eq.cpp
TOOLS=Tools/Node.cpp Tools/UnionFind.cpp
PARSER=Parser/CNFParser.cpp Parser/LOGParser.cpp $(EXPRNAME).cpp $(EXPRNAME).tab.cpp $(EXPRNAME).yy.c

PRELOG=$(EXPRNAME).tab.cpp $(EXPRNAME).yy.c

SRCS= $(GLOBAL) $(RANDOMGEN) $(TOOLS) $(CORE) $(SMT) $(BET) $(MAIN) $(PARSER)

#OBJS
OBJS=$(SRCS:.cpp=.o)
OBJDIR=../obj/

all: $(TARGET)
		@echo END

$(TARGET): $(PRELOG) $(OBJS)
		$(CC) $(CFLAGS) -o $(TARGET) $(OBJS) $(LIBLEX)

.cpp.o:
		$(CC) $(CFLAGS) -c $<  -o $@

$(EXPRNAME).yy.c :  $(EXPRNAME).l
		$(LEX)  -o $@ $^

$(EXPRNAME).tab.cpp : $(EXPRNAME).ypp
		$(YACC) --report=all -o $@ -d $^

clean:
		rm -f $(EXPRNAME).yy.c $(EXPRNAME).tab.cpp $(EXPRNAME).tab.hpp $(OBJS) $(TARGET)
