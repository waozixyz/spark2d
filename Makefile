# Dependency paths
DEPS_DIR=deps
CIMGUI_PATH=$(DEPS_DIR)/cimgui
SDL_PATH=$(DEPS_DIR)/SDL
SDL_TTF_PATH=$(DEPS_DIR)/SDL_ttf
SDL_IMAGE_PATH=$(DEPS_DIR)/SDL_image

CC=gcc
CXX=g++
EMCC=emcc
CXXFLAGS=$(CFLAGS) -DIMGUI_IMPL_API="extern \"C\""

CFLAGS=-Wall -Wextra -fPIC \
-I./include \
-I$(CIMGUI_PATH) \
-I$(CIMGUI_PATH)/imgui \
-I$(CIMGUI_PATH)/imgui/backends \
-I$(SDL_PATH)/include \
-DCIMGUI_DEFINE_ENUMS_AND_STRUCTS

LDFLAGS=-L$(SDL_PATH)/build \
-L$(SDL_TTF_PATH)/build \
-L$(SDL_IMAGE_PATH)/build \
-L$(CIMGUI_PATH) \
-lSDL3 -lSDL3_ttf -lSDL3_image -lm -lcimgui

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

IMGUI_SOURCES = \
$(CIMGUI_PATH)/imgui/imgui.cpp \
$(CIMGUI_PATH)/imgui/imgui_draw.cpp \
$(CIMGUI_PATH)/imgui/imgui_tables.cpp \
$(CIMGUI_PATH)/imgui/imgui_widgets.cpp

BACKEND_SOURCES = \
$(CIMGUI_PATH)/imgui/backends/imgui_impl_sdl3.cpp \
$(CIMGUI_PATH)/imgui/backends/imgui_impl_sdlrenderer3.cpp

ALL_SOURCES = $(SOURCES) $(GRAPHICS_SOURCES) $(UI_SOURCES) $(IMGUI_SOURCES) $(BACKEND_SOURCES)

OBJ=$(patsubst $(SRC_DIR)/%.c,$(OBJ_DIR)/%.o,$(SOURCES) $(GRAPHICS_SOURCES) $(UI_SOURCES))
OBJ+=$(patsubst $(CIMGUI_PATH)/imgui/%.cpp,$(OBJ_DIR)/%.o,$(IMGUI_SOURCES))
OBJ+=$(patsubst $(CIMGUI_PATH)/imgui/backends/%.cpp,$(OBJ_DIR)/%.o,$(BACKEND_SOURCES))

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
	cd $(CIMGUI_PATH) && \
	CMAKE_C_FLAGS="-DIMGUI_IMPL_API='extern \"C\"'" \
	cmake -B build \
	-DIMGUI_IMPL_SDL=ON \
	-DIMGUI_IMPL_SDL_VERSION=3 \
	-DIMGUI_IMPL_OPENGL=ON \
	&& cmake --build build

$(OBJ_DIR)/%.o: $(CIMGUI_PATH)/imgui/backends/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -I$(CIMGUI_PATH)/imgui -c $< -o $@

$(OBJ_DIR)/%.o: $(CIMGUI_PATH)/imgui/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

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
