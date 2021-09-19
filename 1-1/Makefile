CXX=gcc
FLAGS=-Wall -g -DDEBUG
OUT=pre
OUT_DIR=out
SOURCE=main.c sol.c

all: preparation
	$(CXX) $(FLAGS) $(SOURCE) -o $(OUT_DIR)/$(OUT)

preparation:
	@rm -rf $(OUT_DIR)
	@mkdir -p $(OUT_DIR)
