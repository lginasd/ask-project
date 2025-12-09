# Compiler
CXX = g++

# Compiler flags
CXXFLAGS = -Wall -Wextra -std=c++17 -O2

# Boost include path (adjust if Boost is in a non-standard location)
BOOST_INCLUDE ?=
# Example: BOOST_INCLUDE = -I/path/to/boost/include

# Raylib include path override (optional)
RAYLIB_INCLUDE ?=

# Detect platform for raylib link flags (override via env if needed)
UNAME_S := $(shell uname -s 2>/dev/null)
ifeq ($(UNAME_S),Linux)
	RAYLIB_LIBS ?= -lraylib -lm -lpthread -ldl -lrt -lX11
endif
ifeq ($(UNAME_S),Darwin)
	RAYLIB_LIBS ?= -lraylib -framework OpenGL -framework Cocoa -framework IOKit -framework CoreVideo
endif
ifeq ($(OS),Windows_NT)
	RAYLIB_LIBS ?= -lraylib -lopengl32 -lgdi32 -lwinmm
endif
RAYLIB_LIBS ?= -lraylib

# Linker flags for quadmath library (required by Boost float128)
LDFLAGS = -lquadmath

# Directories
BUILD_DIR = build
BIN_DIR = bin

# Target executable
TARGET = $(BIN_DIR)/main

# Source files
SRCS = main.cpp utils.cpp conversion.cpp gui.cpp

# Object files (placed in build directory)
OBJS = $(SRCS:%.cpp=$(BUILD_DIR)/%.o)

# Default target
all: $(TARGET)

# Create directories if they don't exist
$(BUILD_DIR) $(BIN_DIR):
	mkdir -p $@

# Link object files to create executable
$(TARGET): $(OBJS) | $(BIN_DIR)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJS) $(LDFLAGS) $(RAYLIB_LIBS)

# Compile source files to object files in build directory
$(BUILD_DIR)/%.o: %.cpp | $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) $(BOOST_INCLUDE) $(RAYLIB_INCLUDE) -c $< -o $@

# Run the program
run: $(TARGET)
	./$(TARGET)

# Clean generated files and directories
clean:
	rm -rf $(BUILD_DIR) $(BIN_DIR)

# Phony targets
.PHONY: all run clean
