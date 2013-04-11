CC=g++
SRCDIR=../molevelset/src
INCLUDE=-I${SRCDIR}
LINKFLAGS=-lm

all: misc box findtree

clean: 
	rm -f *.o testMisc testBox testFindTree

box.o: ${SRCDIR}/box.h ${SRCDIR}/box.cpp
	${CC} -c ../molevelset/src/box.cpp ${INCLUDE}

misc.o: ${SRCDIR}/misc.h ${SRCDIR}/misc.cpp
	${CC} -c ../molevelset/src/misc.cpp ${INCLUDE}

findtree.o: ${SRCDIR}/findtree.h ${SRCDIR}/findtree.cpp
	${CC} -c ../molevelset/src/findtree.cpp ${INCLUDE}

testMisc.o: testMisc.cpp
	${CC} -c testMisc.cpp ${INCLUDE}

testMisc: box.o misc.o testMisc.o
	${CC} -o testMisc box.o misc.o testMisc.o ${LINKFLAGS}

testBox.o: testBox.cpp 
	${CC} -c testBox.cpp ${INCLUDE}

testBox: testBox.o box.o misc.o
	${CC} -o testBox box.o misc.o testBox.o ${LINKFLAGS}

testFindTree.o: testFindTree.cpp
	${CC} -c testFindTree.cpp ${INCLUDE}

testFindTree: box.o misc.o findtree.o testFindTree.o
	${CC} -o testFindTree box.o misc.o findtree.o testFindTree.o ${LINKFLAGS}

misc: testMisc
	./testMisc

box: testBox
	./testBox

findtree: testFindTree
	./testFindTree