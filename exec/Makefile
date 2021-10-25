SOURCES=$(wildcard *.cpp)
TARGETS=exec
OBJECTS=$(SOURCES:%.cpp=%.o)
FLAGS=-I../Log/Include  
LFLAGS=-L../Log/Lib -llog

.PHONY: all clean

all: $(TARGETS)

%.o: %.cpp
	g++ -c $(FLAGS) -o $@ $^

$(TARGETS): $(OBJECTS)
	g++ $(FLAGS) $(LFLAGS) -o $@.bin $^

clean:
	rm -rf $(TARGETS).bin $(OBJECTS)
