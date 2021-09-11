CXX=gcc
OUT=pre
OUT_DIR=out
SOURCE=main.c sol.c

all: preparation
	$(CXX) $(SOURCE) -o $(OUT)

preparation:
	@rm -rf $(OUT_DIR)
	@mkdir -p $(OUT_DIR)
