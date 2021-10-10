#! /bin/zsh

if [[ $# -lt 1 ]]; then
    echo "Enter project name."
    exit 1
fi

mkdir $1 2> /dev/null
if [[ $? -ne 0 ]]; then
    echo "Cannot create directory."
    exit 1
fi

read -r -d '' makefile << EOF
SOURCES=\$(wildcard *.cpp)
TARGETS=$1
OBJECTS=\$(SOURCES:%.cpp=%.o)
FLAGS=-I../Log/Include  
LFLAGS=-L../Log/Lib -llog

.PHONY: all clean

all: \$(TARGETS)

%.o: %.cpp
\tg++ -c \$(FLAGS) -o \$@ \$^

\$(TARGETS): \$(OBJECTS)
\tg++ \$(FLAGS) \$(LFLAGS) -o \$@.bin \$^

clean:
\trm -rf \$(TARGETS).bin \$(OBJECTS)
EOF

echo $makefile 1> $1/Makefile

read -r -d '' main << EOF
#include <log.h>

int main(int argc, char **argv)
{
    log(info, "Hello world");
    return 0;
}
EOF

echo $main 1> $1/main.cpp
cd $1 && make 1> /dev/null

if [[ $? -eq 0 ]]; then
    ./$1.bin
    echo "Project \"$1\" Successfully created."
else
    echo "Project created with error status - $?"
fi

exit $?
