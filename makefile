#
# CAD/Off makefile for EMX (0.9c)
#

CFLAGS=-g -Wall -Zmt -O2 
CC=gcc $(CFLAGS)
I=e:/prog/emx/include/
o=obj

# -------------------------------------------------------------------------

.SUFFIXES: .cpp .$(o)

.c.$(o):
	$(CC) -Zomf -Zsys -c $<

# -------------------------------------------------------------------------

default:	cadoff.exe

cadoff.exe:	cadoff.obj cadoff.def cadoff.res
	$(CC) -o cadoff.exe -Zomf -Zsys -O2 -s *.$(o) cadoff.def cadoff.res

cadoff.res:	cadoff.rc cadoff.ico
	rc -r -i e:\prog\emx\include cadoff.rc

# -------------------------------------------------------------------------
