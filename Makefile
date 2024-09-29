PROGRAM := main

CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra
LLVM_CFLAGS = $(shell llvm-config --cxxflags)
LLVM_LDFLAGS = $(shell llvm-config --ldflags --libs all)

SRC_DIR = src
OBJ_DIR = obj
BIN_DIR = bin

src_files := $(wildcard $(SRC_DIR)/*.cpp)
obj_files := $(patsubst $(SRC_DIR)/%.cpp, $(OBJ_DIR)/%.o, $(src_files))
target = $(BIN_DIR)/$(PROGRAM)

all: $(target)

$(target): $(obj_files) | $(BIN_DIR)
	$(CXX) $(CXXFLAGS) $(LLVM_CFLAGS) -o $@ $^ $(LLVM_LDFLAGS)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp | $(OBJ_DIR)
	$(CXX) $(CXXFLAGS) $(LLVM_CFLAGS) -c $< -o $@

$(BIN_DIR):
	mkdir -p $(BIN_DIR)

$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

run: $(target)
	./$(target)

clean:
	rm -rf $(OBJ_DIR) $(BIN_DIR)

.PHONY: all run clean
