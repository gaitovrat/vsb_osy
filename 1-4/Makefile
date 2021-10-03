SOURCES=$(wildcard *.cpp)
TARGETS=process
OBJECTS=$(SOURCES:%.cpp=%.o)

.PHONY: all clean

all: $(TARGETS)

%.o: %.cpp
	g++ -c -o $@ $^

$(TARGETS): $(OBJECTS)
	g++ -o $@.bin $^

clean:
	rm -rf $(TARGETS).bin $(OBJECTS)