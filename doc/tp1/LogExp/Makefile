#example name
NAME=expr
#compiler name
CC=g++
#lex name+path
LEX=flex
LIBLEX=-lfl
#yacc/bison
YACC=bison

all: $(NAME).tab.cpp $(NAME).yy.c $(NAME).cpp main.cpp
	$(CC) -g -Wno-write-strings -o calc $^ $(LIBLEX)

$(NAME).yy.c :  $(NAME).l
	$(LEX)  -o $@ $^

$(NAME).tab.cpp : $(NAME).ypp
	$(YACC) --report=all -o $@ -d $^

clean:
	rm -f *~ $(NAME).yy.c $(NAME).tab.cpp $(NAME).tab.hpp $(NAME).output
