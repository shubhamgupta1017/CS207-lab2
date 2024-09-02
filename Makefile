CXX = g++
CXXFLAGS = -std=c++17 -Iinclude -Wall -Wextra -lpthread

SRC_DIR = src
OBJ_DIR = obj
BIN_DIR = bin
INC_DIR = include

SRCS = $(SRC_DIR)/run.cpp \
       $(SRC_DIR)/task_module.cpp \
       $(SRC_DIR)/io.cpp \
       $(SRC_DIR)/memory_manager.cpp \
       $(SRC_DIR)/test.cpp

OBJS = $(patsubst $(SRC_DIR)/%.cpp,$(OBJ_DIR)/%.o,$(SRCS))

TARGET = $(BIN_DIR)/main
TEST_TARGET = $(BIN_DIR)/test

HEADERS = $(INC_DIR)/config.hpp

all: $(TARGET) $(TEST_TARGET)

$(TARGET): $(filter-out $(OBJ_DIR)/test.o, $(OBJS))
	@mkdir -p $(BIN_DIR)
	$(CXX) $(CXXFLAGS) -o $@ $^

$(TEST_TARGET): $(OBJ_DIR)/test.o
	@mkdir -p $(BIN_DIR)
	$(CXX) $(CXXFLAGS) -o $@ $<

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp $(HEADERS)
	@mkdir -p $(OBJ_DIR)
	$(CXX) $(CXXFLAGS) -c -o $@ $<

clean:
	rm -rf $(OBJ_DIR) $(BIN_DIR)

.PHONY: all clean
