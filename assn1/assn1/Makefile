## CS130 Fall 2014: Computer Graphics
## assn1 Makefile
##
## 
#########################################

LIBS = -lglut -lGLU -lGL -lXmu -lXext -lXi -lX11 -lm
CC = g++

## Global header files
INCLUDE = point3d.h point2d.h face.h vertex.h

## Object files and executables
MAIN_OUT = a.out

## Requirements for each command
MAIN_REQS = main.cpp

## Targets to compile for each command
MAIN_TARGETS = main.cpp

all: main

## Main 
main: $(MAIN_REQS)
	$(CC) $(MAIN_TARGETS) $(LIBS) -o $(MAIN_OUT)

clean:
	rm -f *~ *.o *.out
