
ARCHIVE=04_xkozak15-xmikla10.tar.gz

#compiler
CC=g++

#optimization
OPT=-O2

#dynamically linked libraries
libs=-lsimlib -lm

#flags
CPPFLAGS= -std=c++11 -Wall -Wextra -pedantic -pthread

#name of the executable
BASIC_MODEL=basic

OBJECT_FILES=basic_model.o
FILES=basic_model.cpp


.PHONY: build
.PHONY: clean
.PHONY: distrib

build: ${BASIC_MODEL}

${BASIC_MODEL}: ${FILES}
	${CC} ${CPPFLAGS} ${FILES} -o ${BASIC_MODEL} ${libs} ${OPT}


distrib:
	tar -cvzf ${ARCHIVE} ${FILES} Makefile dokumentace.pdf

clean:
	rm -f ${OBJECT_FILES} ${BASIC_MODEL}
	rm -f ${ARCHIVE}
