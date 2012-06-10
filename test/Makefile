CC=g++
SRCDIR=../molevelset/src
INCLUDE=-I${SRCDIR}
LINKFLAGS=-lm

box.o: ${SRCDIR}/box.hh ${SRCDIR}/box.cc
	${CC} -c ../molevelset/src/box.cc ${INCLUDE}

misc.o: ${SRCDIR}/misc.hh ${SRCDIR}/misc.cc
	${CC} -c ../molevelset/src/misc.cc ${INCLUDE}

testMiscBinary.o: testMiscBinary.cc
	${CC} -c testMiscBinary.cc ${INCLUDE}

testMiscBinary: box.o misc.o testMiscBinary.o
	${CC} -o testMiscBinary box.o misc.o testMiscBinary.o ${LINKFLAGS}

testMisc: testMiscBinary
	./testMiscBinary

clean: 
	rm -f *.o testMiscBinary
