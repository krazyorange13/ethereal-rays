WFLAGS = -Wall -Wextra -Wno-unused-command-line-argument -Wno-enum-conversion
IFLAGS = -Isrc/ -Iinclude/
LFLAGS = -Llib/ -lSDL3 -lSDL3_image
MFLAGS = -O2
FLAGS = $(IFLAGS) $(LFLAGS) $(WFLAGS) $(MFLAGS)

CC = gcc-14
CC_FLAGS = $(FLAGS)

TARGET = out
BUILD_DIR = ./build

C_FILES = $(wildcard src/*.c)
O_FILES = $(C_FILES:%.c=$(BUILD_DIR)/%.o)
D_FILES = $(O_FILES:%.o=%.d)

$(TARGET): $(BUILD_DIR)/$(TARGET)

$(BUILD_DIR)/$(TARGET): $(O_FILES)
	-mkdir -p $(@D)
	$(CC) $(CC_FLAGS) $^ -o $@

-include $(D_FILES)
$(BUILD_DIR)/%.o : %.c
	mkdir -p $(@D)
	$(CC) $(CC_FLAGS) -MMD -c $< -o $@

.PHONY: clean
clean:
	rm -f $(BUILD_DIR)/$(TARGET) $(O_FILES) $(D_FILES)
