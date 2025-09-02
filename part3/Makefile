# Makefile
CC      := clang
BIN     := clox
SRC     := main.c debug.c chunk.c memory.c value.c vm.c compiler.c scanner.c object.c table.c
OBJ     := $(SRC:%.c=build/%.o)

CFLAGS  := -std=c11 -Wall -Wextra -Wno-unused-parameter -Iinclude
LDFLAGS :=

# Default build: optimized
CFLAGS  += -O2

.PHONY: all run clean debug release

all: $(BIN)

# Link step
$(BIN): $(OBJ)
	$(CC) $(OBJ) $(LDFLAGS) -o $@

# Compile step (+ header dependency generation)
build/%.o: %.c | build
	$(CC) $(CFLAGS) -MMD -MP -c $< -o $@

build:
	mkdir -p $@

# Convenience targets
run: $(BIN)
	./$(BIN)

debug: CFLAGS += -O0 -g
debug: clean $(BIN)

release: CFLAGS += -O3
release: clean $(BIN)

clean:
	rm -rf build $(BIN)

# Auto-include header deps
-include $(OBJ:.o=.d)

