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

box.o: ${SRCDIR}/box.hh ${SRCDIR}/box.cc
	${CC} -c ../molevelset/src/box.cc ${INCLUDE}

misc.o: ${SRCDIR}/misc.hh ${SRCDIR}/misc.cc
	${CC} -c ../molevelset/src/misc.cc ${INCLUDE}

testMiscBinary.o: testMiscBinary.cc
	${CC} -c testMiscBinary.cc ${INCLUDE}

testMiscBinary: box.o misc.o testMiscBinary.o
	${CC} -o testMiscBinary box.o misc.o testMiscBinary.o ${LINKFLAGS}

testBoxBinary.o: testBoxBinary.cc 
	${CC} -c testBoxBinary.cc ${INCLUDE}

testBoxBinary: testBoxBinary.o
	${CC} -o testBoxBinary box.o misc.o testBoxBinary.o ${LINKFLAGS}

