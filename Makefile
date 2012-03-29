# CMPSC 311, Spring 2012, Project 7
#
# Author:   Max DeLiso
# Email:    jmd5473@psu.edu
# File:		Makefile
#

CC=gcc
RM=rm
SRC=impish.c 
INC=
CFLAGS=-std=c99 -Wall -Wextra -pedantic  
DFLAGS=-g
RFLAGS=-DNDEBUG -O2
OUT=impish
INDENT=indent
IFLAGS=-i3 -nut -linux

.PHONY: all help indent clean

all: $(OUT) $(OUT)-dbg

$(OUT): $(SRC) $(INC)
	$(CC) $(CFLAGS) $(RFLAGS) $(SRC) -o $(OUT)

$(OUT)-dbg: $(SRC) $(INC)
	$(CC) $(CFLAGS) $(DFLAGS) $(SRC) -o $(OUT)-dbg

help:
	@echo available targets: $(OUT) $(OUT)-dbg indent clean

indent:
	$(INDENT) $(IFLAGS) $(SRC) $(INC)

clean:
	$(RM) -f $(OUT) $(OUT)-dbg *.o *~

