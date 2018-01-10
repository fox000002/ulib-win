#
include ../../../make.inc

#
CFLAGS =
CXXFLAGS = 
INCS = -I"../../../inc" -I"."
LIBS = -L"../../../lib" 

#
SRCDIR = ../../../src
INCDIR = ../../../inc
OBJDIR = ../../../obj
RESDIR = ../../../res
ICODIR = ../../../ico
#
OBJ = safearray.o
BIN = safearray.exe
RC  = 
RES = ${RC:.rc=.res}
#
HEADER =
#
all: $(BIN) 

#
safearray.exe : safearray.o
	$(CC) -o $@ $< -loleaut32
	
#
%.o : %.c
	$(CC) -c $< $(INCS)
    
#
clean :
	$(RM) ${OBJ} ${RES} ${BIN}
