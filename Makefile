CXX = g++
CXXFLAGS = -std=c++17 -Iinclude -Wall -Wextra

SRC_DIR = src
OBJ_DIR = obj
BIN_DIR = bin
INC_DIR = include

SRCS = $(SRC_DIR)/run.cpp \
       $(SRC_DIR)/task_module.cpp \
       $(SRC_DIR)/io.cpp \
       $(SRC_DIR)/memory_manager.cpp

OBJS = $(patsubst $(SRC_DIR)/%.cpp,$(OBJ_DIR)/%.o,$(SRCS))

TARGET = $(BIN_DIR)/main

HEADERS = $(INC_DIR)/config.hpp

all: $(TARGET)

$(TARGET): $(OBJS)
	@mkdir -p $(BIN_DIR)
	$(CXX) $(CXXFLAGS) -o $@ $(OBJS)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp $(HEADERS)
	@mkdir -p $(OBJ_DIR)
	$(CXX) $(CXXFLAGS) -c -o $@ $<

clean:
	rm -rf $(OBJ_DIR) $(BIN_DIR)

.PHONY: all clean
