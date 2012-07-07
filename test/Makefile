CC=g++
SRCDIR=../molevelset/src
INCLUDE=-I${SRCDIR}
LINKFLAGS=-lm

all: testMisc testBox

clean: 
	rm -f *.o testMiscBinary testBoxBinary

testBox: testBoxBinary
	./testBoxBinary

testMisc: testMiscBinary
	./testMiscBinary

box.o: ${SRCDIR}/box.h ${SRCDIR}/box.cpp
	${CC} -c ../molevelset/src/box.cpp ${INCLUDE}

misc.o: ${SRCDIR}/misc.h ${SRCDIR}/misc.cpp
	${CC} -c ../molevelset/src/misc.cpp ${INCLUDE}

testMiscBinary.o: testMiscBinary.cpp
	${CC} -c testMiscBinary.cpp ${INCLUDE}

testMiscBinary: box.o misc.o testMiscBinary.o
	${CC} -o testMiscBinary box.o misc.o testMiscBinary.o ${LINKFLAGS}

testBoxBinary.o: testBoxBinary.cpp 
	${CC} -c testBoxBinary.cpp ${INCLUDE}

testBoxBinary: testBoxBinary.o box.o misc.o
	${CC} -o testBoxBinary box.o misc.o testBoxBinary.o ${LINKFLAGS}

