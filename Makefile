# Dependency paths
DEPS_DIR=deps
SDL_PATH=$(DEPS_DIR)/SDL
LVGL_DIR=$(DEPS_DIR)
LVGL_DIR_NAME=lvgl
LVGL_PATH=$(LVGL_DIR)/$(LVGL_DIR_NAME)

# Include LVGL's make configuration
include $(LVGL_DIR)/$(LVGL_DIR_NAME)/lvgl.mk

CC=gcc
EMCC=emcc

CFLAGS=-Wall -Wextra -fPIC \
-I./include \
-I$(SDL_PATH)/include \
-I$(LVGL_DIR) \
-I$(LVGL_PATH) \
-I$(LVGL_PATH)/src \
-I. \

LDFLAGS=-L$(SDL_PATH)/build \
$(LVGL_CFLAGS) \
-lSDL2 -lm

EM_FLAGS=-s USE_SDL=2 -s WASM=1 \
-s EXPORTED_RUNTIME_METHODS='["ccall","cwrap"]' \
-s EXPORTED_FUNCTIONS='["_main"]' \
-s ALLOW_MEMORY_GROWTH=1

LIB=libspark2d.a
SRC_DIR=src
OBJ_DIR=obj
WEB_OBJ_DIR=$(OBJ_DIR)/web

SOURCES = \
$(SRC_DIR)/spark_core.c \
$(SRC_DIR)/spark_theme.c \
$(SRC_DIR)/spark_window.c \
$(SRC_DIR)/spark_mouse.c \
$(SRC_DIR)/spark_keyboard.c \
$(SRC_DIR)/spark_timer.c \
$(SRC_DIR)/spark_event.c \
$(SRC_DIR)/spark_filesystem.c \
$(SRC_DIR)/spark_lvgl.c \
$(SRC_DIR)/spark_audio.c

GRAPHICS_SOURCES = \
$(SRC_DIR)/graphics/core.c \
$(SRC_DIR)/graphics/primitives.c \
$(SRC_DIR)/graphics/text.c \
$(SRC_DIR)/graphics/image.c \
$(SRC_DIR)/graphics/color.c

UI_SOURCES = \
$(SRC_DIR)/ui/label.c \
$(SRC_DIR)/ui/button.c \
$(SRC_DIR)/ui/dropdown.c \
$(SRC_DIR)/ui/tabbar.c \
$(SRC_DIR)/ui/slider.c

ALL_SOURCES = $(SOURCES) $(GRAPHICS_SOURCES) $(UI_SOURCES) $(BACKEND_SOURCES)
OBJ=$(patsubst $(SRC_DIR)/%.c,$(OBJ_DIR)/%.o,$(SOURCES) $(GRAPHICS_SOURCES) $(UI_SOURCES))
LVGL_OBJ=$(LVGL_CSRCS:.c=.o)
WEB_OBJ=$(patsubst $(SRC_DIR)/%.c,$(WEB_OBJ_DIR)/%.o,$(ALL_SOURCES))

.PHONY: all clean web_lib deps

all: deps $(LIB)

makedirs:
	@mkdir -p $(OBJ_DIR)/ui
	@mkdir -p $(WEB_OBJ_DIR)/ui

deps:
	cd $(SDL_PATH) && cmake -B build && cmake --build build

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $(LVGL_CFLAGS) -c $< -o $@

$(WEB_OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(dir $@)
	$(EMCC) $(CFLAGS) $(EM_FLAGS) -c $< -o $@

%.o: %.c
	$(CC) $(CFLAGS) $(LVGL_CFLAGS) -c $< -o $@

$(LIB): makedirs $(OBJ) $(LVGL_OBJ)
	ar rcs $@ $(OBJ) $(LVGL_OBJ)

web_lib: makedirs $(WEB_OBJ)

clean:
	rm -rf $(OBJ_DIR) $(LIB)
	rm -f $(LVGL_OBJ)