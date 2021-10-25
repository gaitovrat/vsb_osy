TARGETS = server client
OBJS = $(TARGETS:%=%.o)

CPPFLAGS += -g -std=c++20 -Wall 

all: $(TARGETS)

%: %.o
	$(CXX) -o $@ $^

%.o: %.cpp
	$(CXX) -c $(CPPFLAGS) -o $@ $<

clean:
	rm -fr $(TARGETS) $(OBJS)
