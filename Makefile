FLAGS +=  -g\
		  -Wall\
		  -Wextra\
		  -Wpedantic
CC ?= cc
CFLAGS += -std=c99\
		  $(FLAGS)
CXX ?= c++
CXXFLAGS += -std=c++17\
			$(FLAGS)\
			-Ithird_party/raylib-cpp/include
LIBS += -lraylib

all: bin/berzerk

bin/berzerk: $(wildcard src/*.cpp)
	mkdir -p bin/
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS) $(LIBS)

clean:
	rm -rf bin/*
