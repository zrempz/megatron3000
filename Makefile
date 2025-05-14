TARGET := megatron3000
SRC_DIR := src
BUILD_DIR := build
CXX := g++
CXXFLAGS := -Wall -Wextra -std=c++17

DEBUG_FLAGS := -g -O0 -DDEBUG
RELEASE_FLAGS := -O2 -DNDEBUG

SRCS := $(wildcard $(SRC_DIR)/*.cpp)
OBJS := $(patsubst $(SRC_DIR)/%.cpp,$(BUILD_DIR)/%.o,$(SRCS))

all: release

debug: CXXFLAGS += $(DEBUG_FLAGS)
debug: $(BUILD_DIR)/$(TARGET)

release: CXXFLAGS += $(RELEASE_FLAGS)
release: $(BUILD_DIR)/$(TARGET)

run: release
	@./$(BUILD_DIR)/$(TARGET)

$(BUILD_DIR)/$(TARGET): $(OBJS)
	@mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) $^ -o $@

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) -MMD -MP -c $< -o $@

clean:
	rm -rf $(BUILD_DIR)
	
bear: clean
	bear -- make all

help:
	@echo "Available targets:"
	@echo "		all/release - Build optimized version (default)"
	@echo "		debug 			- Build with debug symbols"
	@echo "		run 				- Build and run the program"
	@echo "		clean 			- Remove all build files"
	@echo "		bear				- Generate compile_comands.json for tooling"
