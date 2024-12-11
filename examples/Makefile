CC=gcc
EMCC=emcc

# LVGL configuration
LVGL_DIR=../deps/lvgl
LVGL_SOURCES=$(shell find $(LVGL_DIR)/src -name "*.c")
LVGL_OBJECTS=$(LVGL_SOURCES:.c=.o)

# Include paths
CFLAGS=-Wall -Wextra \
-I../include \
-I/usr/local/include \
-I$(LVGL_DIR) \
-I$(LVGL_DIR)/src \
-I..

# Library flags
LDFLAGS=-L/usr/local/lib -L.. \
-lspark2d -lSDL2 -lm -lpng

# Emscripten configuration
EM_INCLUDES=-I$(shell em-config CACHE)/sysroot/include
EM_LIBPATH=-L$(shell em-config CACHE)/sysroot/lib
EM_FLAGS=-s USE_SDL=2 -s WASM=1 \
-s EXPORTED_RUNTIME_METHODS='["ccall","cwrap"]' \
-s EXPORTED_FUNCTIONS='["_main"]' \
-s ALLOW_MEMORY_GROWTH=1

# Find all examples
EXAMPLES=$(wildcard */main.c)
TARGETS=$(EXAMPLES:/main.c=)
EXECUTABLES=$(TARGETS:%=%/example)
WEB_BUILD_DIR=web_build

.PHONY: all clean web web_clean

all: $(LVGL_OBJECTS) $(EXECUTABLES)

# Compile LVGL source files
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

%/example: %/main.c ../libspark2d.a $(LVGL_OBJECTS)
	$(CC) $(CFLAGS) $< $(LVGL_OBJECTS) -o $@ $(LDFLAGS)

web: web_dirs
	@for example in $(TARGETS); do \
		echo "Building web version of $$example..."; \
		mkdir -p $(WEB_BUILD_DIR)/$$example; \
		$(EMCC) $(CFLAGS) $(EM_INCLUDES) $(EM_LIBPATH) \
			$$example/main.c ../src/*.c $(LVGL_SOURCES) \
			-I../include \
			$(EM_FLAGS) \
			-s USE_SDL=2 \
			-l SDL2 \
			-o $(WEB_BUILD_DIR)/$$example/index.html \
			--shell-file ../templates/web/index.html \
			$(if $(wildcard $$example/assets),--preload-file $$example/assets@/assets,); \
		if [ -f ../templates/web/style.css ]; then \
			cp ../templates/web/style.css $(WEB_BUILD_DIR)/$$example/; \
		fi; \
	done

web_dirs:
	mkdir -p $(WEB_BUILD_DIR)
	@for example in $(TARGETS); do \
		mkdir -p $(WEB_BUILD_DIR)/$$example; \
	done

clean:
	rm -f */example
	rm -f $(LVGL_OBJECTS)

web_clean:
	rm -rf $(WEB_BUILD_DIR)
	