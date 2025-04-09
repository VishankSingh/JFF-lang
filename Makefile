CC = gcc
CFLAGS = -Wall -Wextra -pedantic -std=c11 -g
LDFLAGS = -lm

# -Werror

SRC_DIR = src
INCLUDE_DIR = src/include
BUILD_DIR = build

TARGET_NAME = main
TARGET = build/$(TARGET_NAME)

SRC = $(wildcard $(SRC_DIR)/*.c)
OBJS = $(SRC:$(SRC_DIR)/%.c=$(BUILD_DIR)/%.o)

YELLOW = \033[1;33m
GREEN = \033[1;32m
RED = \033[1;31m
NC = \033[0m

.PHONY: all clean run debug valgrind

all: $(TARGET)

$(TARGET): $(OBJS)
	@printf "$(YELLOW)[Linking] %s$(NC)\n" "$@"
	@$(CC) $(OBJS) -o $(TARGET) $(LDFLAGS)
	@printf "$(GREEN)[Build Complete]$(NC)\n"


$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(BUILD_DIR)
	@printf "$(YELLOW)[Compiling] %s$(NC)\n" "$<"
	@$(CC) $(CFLAGS) -c $< -o $@

clean:
	@rm -rf $(BUILD_DIR)
	@printf "$(RED)[Cleaned]$(NC)\n"

run: $(TARGET)
	@$(TARGET)

debug: $(TARGET)
	@gdb $(TARGET)

valgrind: $(TARGET)
	@valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes $(TARGET)

