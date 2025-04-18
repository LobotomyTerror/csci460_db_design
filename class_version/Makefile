PROJ=class_version_simple
SRC=${PROJ}.cpp
EXE=${PROJ}
CFLAGS=-g -pg

all: check lint run

${EXE}: class_version_simple.cpp
	clang++ ${CFLAGS} $< -o $@

clean:
	rm ${EXE}

run: ${EXE}
	@./${EXE}

check: ${EXE}
	cppcheck ${SRC}

lint: ${EXE}
	cpplint --filter=-whitespace,-legal,-runtime ${SRC}