# Names of tools to use when building (make sure /c/MinGW/bin/ is in the PATH)
CXX = g++ # mingw32-g++
LD  = g++ # mingw32-g++
RM  = rm -rf

# Warnings to be raised by the CPP compiler
WARNS = -Wall

# Compiler and linker flags
CPPFLAGS = -g -std=c++17 $(WARNS)
LDFLAGS = -s

# Source files
SRC_DIR = ./src
SRCS = \
	$(wildcard $(SRC_DIR)/*.cpp)

# Output directory
OUT_DIR = ./bin
OBJ_DIR = ./obj
OBJS = $(addprefix $(OBJ_DIR)/, $(foreach SRC, $(subst .cpp,.o,$(SRCS)), $(SRC:$(SRC_DIR)/%=%)))

TARGET = main.exe
TARGET := $(OUT_DIR)/$(TARGET)

.PHONY: all clean

all: $(OUT_DIR) $(OBJ_DIR) $(TARGET)

run: $(TARGET)
	$(TARGET)

$(OUT_DIR):
	mkdir -p $(OUT_DIR)

$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

$(TARGET): $(OBJS) | $(OUT_DIR)
	$(CXX) -o $(TARGET) $(OBJS) $(CPPFLAGS) $(LDFLAGS)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp | $(OBJ_DIR)
	$(CXX) $(CPPFLAGS) -c $< -o $@

clean:
	$(RM) $(TARGET) $(OUT_DIR) $(OBJ_DIR)
