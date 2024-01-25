CC = g++
C_FLAGS = -MMD -MP -O3
L_FLAGS = -lsfml-graphics -lsfml-window -lsfml-system -lGL

BIN = raycaster
BUILD_DIR = ./build
ARGS = test.map

SRCS = $(wildcard *.cpp)
OBJS = $(SRCS:%.cpp=$(BUILD_DIR)/%.o)
DEPS = $(OBJS:%.o=%.d)

all: run

run: $(BUILD_DIR)/$(BIN)
	$(BUILD_DIR)/$(BIN) $(ARGS)

$(BUILD_DIR)/$(BIN): $(OBJS)
	$(CC) $^ -o $@ $(L_FLAGS)

-include $(DEPS)

$(BUILD_DIR)/%.o: %.cpp
	mkdir -p $(BUILD_DIR)
	$(CC) $(C_FLAGS) -c $< -o $@

clean:
	rm -rf $(BUILD_DIR)

.PHONY : all run clean
