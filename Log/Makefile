SOURCES=$(wildcard *.cpp)
TARGETS=log
OBJECTS=$(SOURCES:%.cpp=%.o)
FLAGS=-I./Include

all: $(TARGETS)

$(TARGETS): $(OBJECTS)
	mkdir -p Lib
	ar -r Lib/lib$@.a $^

%.o: %.cpp
	g++ -c $(FLAGS) -o $@ $^

clean:
	rm -rf Lib
