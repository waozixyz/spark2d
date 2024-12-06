# Dependency paths
DEPS_DIR=deps
SDL_PATH=$(DEPS_DIR)/SDL
SDL_TTF_PATH=$(DEPS_DIR)/SDL_ttf
SDL_IMAGE_PATH=$(DEPS_DIR)/SDL_image

CC=gcc
EMCC=emcc

CFLAGS=-Wall -Wextra -fPIC \
-I./include \
-I$(SDL_PATH)/include \

LDFLAGS=-L$(SDL_PATH)/build \
-L$(SDL_TTF_PATH)/build \
-L$(SDL_IMAGE_PATH)/build \
-lSDL3 -lSDL3_ttf -lSDL3_image -lm

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
$(SRC_DIR)/spark_theme.c \
$(SRC_DIR)/spark_window.c \
$(SRC_DIR)/spark_mouse.c \
$(SRC_DIR)/spark_keyboard.c \
$(SRC_DIR)/spark_font.c \
$(SRC_DIR)/spark_timer.c \
$(SRC_DIR)/spark_event.c \
$(SRC_DIR)/spark_filesystem.c \
$(SRC_DIR)/spark_audio.c

GRAPHICS_SOURCES = \
$(SRC_DIR)/graphics/core.c \
$(SRC_DIR)/graphics/primitives.c \
$(SRC_DIR)/graphics/text.c \
$(SRC_DIR)/graphics/image.c \
$(SRC_DIR)/graphics/shadows.c \
$(SRC_DIR)/graphics/font.c \
$(SRC_DIR)/graphics/color.c

UI_SOURCES = \
$(SRC_DIR)/ui/core/ui_system.c \
$(SRC_DIR)/ui/components/grid.c \
$(SRC_DIR)/ui/components/button.c \
$(SRC_DIR)/ui/components/dropdown.c \
$(SRC_DIR)/ui/components/tabbar.c \
$(SRC_DIR)/ui/components/slider.c


ALL_SOURCES = $(SOURCES) $(GRAPHICS_SOURCES) $(UI_SOURCES) $(BACKEND_SOURCES)

OBJ=$(patsubst $(SRC_DIR)/%.c,$(OBJ_DIR)/%.o,$(SOURCES) $(GRAPHICS_SOURCES) $(UI_SOURCES))

WEB_OBJ=$(patsubst $(SRC_DIR)/%.c,$(WEB_OBJ_DIR)/%.o,$(ALL_SOURCES))

.PHONY: all clean web_lib deps

all: deps $(LIB)

makedirs:
	@mkdir -p $(OBJ_DIR)/ui/core
	@mkdir -p $(OBJ_DIR)/ui/components
	@mkdir -p $(WEB_OBJ_DIR)/ui/core
	@mkdir -p $(WEB_OBJ_DIR)/ui/components

deps:
	cd $(SDL_PATH) && cmake -B build && cmake --build build
	cd $(SDL_TTF_PATH) && cmake -B build && cmake --build build
	cd $(SDL_IMAGE_PATH) && cmake -B build && cmake --build build

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

$(WEB_OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(dir $@)
	$(EMCC) $(CFLAGS) $(EM_FLAGS) -c $< -o $@

$(LIB): makedirs $(OBJ)
	ar rcs $@ $(OBJ)

web_lib: makedirs $(WEB_OBJ)

clean:
	rm -rf $(OBJ_DIR) $(LIB)
