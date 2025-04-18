PROJ=indexFile
SRC=${PROJ}.cpp
EXE=${PROJ}
CFLAGS=-g -pg

${EXE}: ${SRC}
	clang++ ${CFLAGS} $< -o $@

clean:
	rm ${EXE}

run: ${EXE}
	@./${EXE}

check: ${EXE}
	cppcheck ${SRC}

lint: ${EXE}
	cpplint --filter=-whitespace,-legal,-runtime ${SRC}