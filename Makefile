CC=gcc
CFLAGS=-Wall -Wextra -I./include $(shell pkg-config --cflags sdl3-ttf)
LDFLAGS=-lSDL3 -lSDL3_ttf $(shell pkg-config --libs sdl3-ttf)
LIB=libspark2d.a

SRC_DIR=src
OBJ_DIR=obj
EXAMPLES_DIR=examples

# List specific source files we want to compile
# This helps avoid duplicate implementations
SOURCES = \
    $(SRC_DIR)/spark_core.c \
    $(SRC_DIR)/spark_graphics.c \
    $(SRC_DIR)/spark_ui.c \
    $(SRC_DIR)/spark_window.c \
    $(SRC_DIR)/spark_mouse.c \
    $(SRC_DIR)/spark_keyboard.c \
    $(SRC_DIR)/spark_font.c \
    $(SRC_DIR)/spark_timer.c \
    $(SRC_DIR)/spark_event.c \
    $(SRC_DIR)/spark_audio.c

OBJ=$(SOURCES:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)

.PHONY: all clean examples

all: $(LIB)

$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c | $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(LIB): $(OBJ)
	ar rcs $@ $^

examples: $(LIB)
	$(MAKE) -C $(EXAMPLES_DIR)

clean:
	rm -rf $(OBJ_DIR) $(LIB)
	$(MAKE) -C $(EXAMPLES_DIR) clean



    