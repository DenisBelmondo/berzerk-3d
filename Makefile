FLAGS +=  -g\
		  -Wall\
		  -Wextra\
		  -Wpedantic
CC ?= cc
CFLAGS += -std=c99\
		  $(FLAGS)\
		  -Ithird_party/cute_headers
CXX ?= c++
CXXFLAGS += -std=c++17\
			$(FLAGS)\
			-Ithird_party/raylib-cpp/include\
			-Ithird_party/cute_headers
LIBS += -lraylib

all: bin/berzerk
	rm bin/cute_c2.o

bin/cute_c2.o: src/cute_c2.c
	$(CC) $(CFLAGS) -o $@ -c $^

bin/berzerk: bin/cute_c2.o $(wildcard src/*.cpp)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS) $(LIBS)

clean:
	rm -rf bin/*
