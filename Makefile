#
# CMPSC 311, Spring 2012, Project 7
#
# Author: Max DeLiso
# Email: jmd5473@psu.edu, maxdeliso@gmail.com
# File: Makefile
# License:
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING 
# BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND 
# NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, 
# DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, 
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
#

CC=gcc
RM=rm
SRC=impish.c util.c
INC=impish.h util.h
CFLAGS=-std=c99 -D_XOPEN_SOURCE=700 -D_ISOC99_SOURCE -Wall -Wextra -pedantic
LDFLAGS=-lreadline
DFLAGS=-g
RFLAGS=-DNDEBUG -O2
OUT=impish
INDENT=indent
IFLAGS=-i3 -nut -linux
SIZE=size
INSTALL=install
PREFIX=/usr/local/bin

.PHONY: all help indent clean

all: $(OUT) 

$(OUT): $(SRC) $(INC)
	@$(CC) $(CFLAGS) $(RFLAGS) $(LDFLAGS) $(SRC) -o $(OUT)
	@$(SIZE) $(OUT)

$(OUT)-dbg: $(SRC) $(INC)
	@$(CC) $(CFLAGS) $(DFLAGS) $(LDFLAGS) $(SRC) -o $(OUT)-dbg
	@$(SIZE) $(OUT)-dbg

help:
	@echo available targets: $(OUT) $(OUT)-dbg indent clean install

indent:
	@$(INDENT) $(IFLAGS) $(SRC) $(INC)

clean:
	@$(RM) -f $(OUT) $(OUT)-dbg *.o *~

install: $(OUT)
	@$(INSTALL) $(OUT) $(PREFIX)

