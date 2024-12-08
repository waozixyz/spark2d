CC ?= gcc
EMCC = emcc
LVGL_DIR_NAME ?= lvgl
DEPS_DIR = deps
LVGL_DIR = $(DEPS_DIR)/$(LVGL_DIR_NAME)
SDL_PATH = $(DEPS_DIR)/SDL

# Warning flags from LVGL example
WARNINGS := -Wall -Wshadow -Wundef -Wmissing-prototypes -Wno-discarded-qualifiers -Wextra \
    -Wno-unused-function -Wno-error=strict-prototypes -Wpointer-arith -fno-strict-aliasing \
    -Wno-error=cpp -Wuninitialized -Wmaybe-uninitialized -Wno-unused-parameter \
    -Wno-missing-field-initializers -Wtype-limits -Wsizeof-pointer-memaccess \
    -Wno-format-nonliteral -Wno-cast-qual -Wunreachable-code -Wno-switch-default \
    -Wreturn-type -Wmultichar -Wformat-security -Wno-ignored-qualifiers \
    -Wno-error=pedantic -Wno-sign-compare -Wno-error=missing-prototypes \
    -Wdouble-promotion -Wclobbered -Wdeprecated -Wempty-body -Wshift-negative-value \
    -Wstack-usage=2048 -Wno-unused-value -std=gnu99

# Include paths
INCLUDES := -I./include \
    -I$(SDL_PATH)/include \
    -I$(LVGL_DIR) \
    -I$(LVGL_DIR)/src \
    -I.

# Basic CFLAGS
CFLAGS ?= -O3 -g0 $(INCLUDES) $(WARNINGS)

# Emscripten flags
EM_FLAGS = -s USE_SDL=2 -s WASM=1 \
    -s EXPORTED_RUNTIME_METHODS='["ccall","cwrap"]' \
    -s EXPORTED_FUNCTIONS='["_main"]' \
    -s ALLOW_MEMORY_GROWTH=1

# Include LVGL's make configuration
include $(LVGL_DIR)/lvgl.mk

# Library name
LIB = libspark2d.a
WEB_LIB = libspark2d_web.a
BUILD_DIR = build
OBJ_DIR = $(BUILD_DIR)/obj
WEB_OBJ_DIR = $(BUILD_DIR)/web_obj

# Source directories
SRC_DIR = src
GRAPHICS_DIR = $(SRC_DIR)/graphics
UI_DIR = $(SRC_DIR)/ui
BACKENDS_DIR = backends

# Collect source files
SOURCES = $(wildcard $(SRC_DIR)/*.c)
GRAPHICS_SOURCES = $(wildcard $(GRAPHICS_DIR)/*.c)
UI_SOURCES = $(wildcard $(UI_DIR)/*.c)
BACKENDS_SOURCES = $(wildcard $(BACKENDS_DIR)/*.c)

# Generate object files list
OBJS = $(patsubst %.c,$(OBJ_DIR)/%.o,$(notdir $(SOURCES)))
GRAPHICS_OBJS = $(patsubst %.c,$(OBJ_DIR)/graphics/%.o,$(notdir $(GRAPHICS_SOURCES)))
UI_OBJS = $(patsubst %.c,$(OBJ_DIR)/ui/%.o,$(notdir $(UI_SOURCES)))
BACKENDS_OBJS = $(patsubst %.c,$(OBJ_DIR)/backends/%.o,$(notdir $(BACKENDS_SOURCES)))

# Web object files
WEB_OBJS = $(patsubst %.c,$(WEB_OBJ_DIR)/%.o,$(notdir $(SOURCES)))
WEB_GRAPHICS_OBJS = $(patsubst %.c,$(WEB_OBJ_DIR)/graphics/%.o,$(notdir $(GRAPHICS_SOURCES)))
WEB_UI_OBJS = $(patsubst %.c,$(WEB_OBJ_DIR)/ui/%.o,$(notdir $(UI_SOURCES)))
WEB_BACKENDS_OBJS = $(patsubst %.c,$(WEB_OBJ_DIR)/backends/%.o,$(notdir $(BACKENDS_SOURCES)))

# Combine all objects
ALL_OBJS = $(OBJS) $(GRAPHICS_OBJS) $(UI_OBJS) $(BACKENDS_OBJS) $(LVGL_OBJS)
ALL_WEB_OBJS = $(WEB_OBJS) $(WEB_GRAPHICS_OBJS) $(WEB_UI_OBJS) $(WEB_BACKENDS_OBJS)

# Targets
.PHONY: all clean dirs web

all: dirs $(LIB)

web: dirs $(WEB_LIB)

dirs:
	@mkdir -p $(OBJ_DIR)/graphics
	@mkdir -p $(OBJ_DIR)/ui
	@mkdir -p $(OBJ_DIR)/backends
	@mkdir -p $(WEB_OBJ_DIR)/graphics
	@mkdir -p $(WEB_OBJ_DIR)/ui
	@mkdir -p $(WEB_OBJ_DIR)/backends

# Native build rules
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@echo "CC $<"
	@$(CC) $(CFLAGS) -c $< -o $@

$(OBJ_DIR)/graphics/%.o: $(GRAPHICS_DIR)/%.c
	@echo "CC $<"
	@$(CC) $(CFLAGS) -c $< -o $@

$(OBJ_DIR)/ui/%.o: $(UI_DIR)/%.c
	@echo "CC $<"
	@$(CC) $(CFLAGS) -c $< -o $@

$(OBJ_DIR)/backends/%.o: $(BACKENDS_DIR)/%.c
	@echo "CC $<"
	@$(CC) $(CFLAGS) -c $< -o $@

# Web build rules
$(WEB_OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@echo "EMCC $<"
	@$(EMCC) $(CFLAGS) $(EM_FLAGS) -c $< -o $@

$(WEB_OBJ_DIR)/graphics/%.o: $(GRAPHICS_DIR)/%.c
	@echo "EMCC $<"
	@$(EMCC) $(CFLAGS) $(EM_FLAGS) -c $< -o $@

$(WEB_OBJ_DIR)/ui/%.o: $(UI_DIR)/%.c
	@echo "EMCC $<"
	@$(EMCC) $(CFLAGS) $(EM_FLAGS) -c $< -o $@

$(WEB_OBJ_DIR)/backends/%.o: $(BACKENDS_DIR)/%.c
	@echo "EMCC $<"
	@$(EMCC) $(CFLAGS) $(EM_FLAGS) -c $< -o $@

$(LIB): $(ALL_OBJS)
	@echo "Creating library $@"
	@ar rcs $@ $(ALL_OBJS)

$(WEB_LIB): $(ALL_WEB_OBJS)
	@echo "Creating web library $@"
	@$(EMCC) -o $@ $(ALL_WEB_OBJS)

clean:
	rm -rf $(BUILD_DIR) $(LIB) $(WEB_LIB)
	