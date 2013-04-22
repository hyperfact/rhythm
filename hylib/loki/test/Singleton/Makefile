BIN2 = Dependencies
BIN3 = Phoenix
CXXFLAGS = -Wall -Wold-style-cast -Wundef -Wsign-compare -Wconversion -Wpointer-arith -pedantic -O2
CPPFLAGS = -I../../include -DNDEBUG
LDFLAGS = -L../../lib
LDLIBS = -lloki

.PHONY: build clean
build:  $(BIN2) $(BIN3)
clean:
	rm -f  $(BIN2) $(BIN3) $(BIN2).exe $(BIN2).o $(BIN3).exe $(BIN3).o
