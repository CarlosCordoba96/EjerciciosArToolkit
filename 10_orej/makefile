# Ruta absoluta o relativa (cuidado con espacios en blanco al final del path!)
ARTOOLKITDIR = ../../ARToolKit
INC_DIR      = $(ARTOOLKITDIR)/include
LIB_DIR      = $(ARTOOLKITDIR)/lib

LIBS   = -lARgsub_lite -lARgsub -lARvideo -lAR -lglut -lGLU -lGL -lm

NAMEEXEC     = ejemploorej

all: $(NAMEEXEC)

$(NAMEEXEC): $(NAMEEXEC).c orej.c orej.h
	cc -I $(INC_DIR) -o $(NAMEEXEC) $(NAMEEXEC).c orej.c -L$(LIB_DIR) $(LIBS) 
clean:
	rm -f *.o $(NAMEEXEC) *~ *.*~

