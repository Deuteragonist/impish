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
CFLAGS=-std=c99 -D_XOPEN_SOURCE=700 -Wall -Wextra -pedantic
LDFLAGS=-lreadline
DFLAGS=-g
RFLAGS=-DNDEBUG -O2
OUT=impish
INDENT=indent
IFLAGS=-i3 -nut -linux
SIZE=size

.PHONY: all help indent clean

all: $(OUT) 

$(OUT): $(SRC) $(INC)
	@$(CC) $(CFLAGS) $(RFLAGS) $(LDFLAGS) $(SRC) -o $(OUT)
	@$(SIZE) $(OUT)

$(OUT)-dbg: $(SRC) $(INC)
	@$(CC) $(CFLAGS) $(DFLAGS) $(LDFLAGS) $(SRC) -o $(OUT)-dbg
	@$(SIZE) $(OUT)-dbg

help:
	@echo available targets: $(OUT) $(OUT)-dbg indent clean

indent:
	@$(INDENT) $(IFLAGS) $(SRC) $(INC)

clean:
	@$(RM) -f $(OUT) $(OUT)-dbg *.o *~

