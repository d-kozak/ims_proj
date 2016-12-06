
ARCHIVE=04_xkozak15-xmikla10.tar.gz

#compiler
CC=g++

#optimization
OPT=-O2

#dynamically linked libraries
libs=-lsimlib -lm

#flags
CPPFLAGS= -std=c++11 -pthread

#name of the executable
BASIC_MODEL=basic
ADVANCED_MODEL=advanced

OBJECT_FILES= ClosableFacility.o MaintainableClosableFacility.o TimeoutableProcess.o advanced_model.o basic_model.o
FILES= ClosableFacility.cpp MaintainableClosableFacility.cpp TimeoutableProcess.cpp


.PHONY: build
.PHONY: clean
.PHONY: distrib

build: ${BASIC_MODEL} ${ADVANCED_MODEL}


${ADVANCED_MODEL}: ${FILES}
	${CC} ${CPPFLAGS} ${FILES} advanced_model.cpp  -o ${ADVANCED_MODEL} ${libs} ${OPT}

${BASIC_MODEL}: ${FILES}
	${CC} ${CPPFLAGS} ${FILES} basic_model.cpp  -o ${BASIC_MODEL} ${libs} ${OPT}

distrib:
	tar -cvzf ${ARCHIVE} ${FILES} basic_model.cpp advanced_model.cpp Makefile dokumentace.pdf

clean:
	rm -f ${OBJECT_FILES} ${BASIC_MODEL} ${ADVANCED_MODEL}
	rm -f ${ARCHIVE}


run:
	echo "================================================================================="
	echo "Running basic model"
	./${BASIC_MODEL}
	echo "================================================================================="
	echo "Running advanced model - model with self-service cash registers"
	./${ADVANCED_MODEL}
	echo "================================================================================="
