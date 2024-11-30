CC=gcc
EMCC=emcc
CFLAGS=-Wall -Wextra -I./include $(shell pkg-config --cflags sdl3-ttf)
LDFLAGS=-lSDL3 -lSDL3_ttf -lm $(shell pkg-config --libs sdl3-ttf)

# Emscripten flags
EM_FLAGS=-s USE_SDL=3 -s USE_SDL_TTF=3 -s WASM=1 \
        -s EXPORTED_RUNTIME_METHODS='["ccall","cwrap"]' \
        -s EXPORTED_FUNCTIONS='["_main"]' \
        -s ALLOW_MEMORY_GROWTH=1

LIB=libspark2d.a
SRC_DIR=src
OBJ_DIR=obj
WEB_OBJ_DIR=$(OBJ_DIR)/web

SOURCES = \
    $(SRC_DIR)/spark_core.c \
    $(SRC_DIR)/spark_graphics.c \
    $(SRC_DIR)/spark_theme.c \
    $(SRC_DIR)/spark_ui.c \
    $(SRC_DIR)/spark_window.c \
    $(SRC_DIR)/spark_mouse.c \
    $(SRC_DIR)/spark_keyboard.c \
    $(SRC_DIR)/spark_font.c \
    $(SRC_DIR)/spark_timer.c \
    $(SRC_DIR)/spark_event.c \
    $(SRC_DIR)/spark_audio.c

OBJ=$(SOURCES:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)
WEB_OBJ=$(SOURCES:$(SRC_DIR)/%.c=$(WEB_OBJ_DIR)/%.o)

.PHONY: all clean web_lib

all: $(LIB)

$(OBJ_DIR) $(WEB_OBJ_DIR):
	mkdir -p $@

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c | $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(WEB_OBJ_DIR)/%.o: $(SRC_DIR)/%.c | $(WEB_OBJ_DIR)
	$(EMCC) $(CFLAGS) $(EM_FLAGS) -c $< -o $@

$(LIB): $(OBJ)
	ar rcs $@ $^

web_lib: $(WEB_OBJ)

clean:
	rm -rf $(OBJ_DIR) $(LIB)