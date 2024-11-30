CC=gcc
EMCC=emcc
CFLAGS=-Wall -Wextra -I../include $(shell pkg-config --cflags sdl3-ttf)
LDFLAGS=-L.. -lspark2d -lSDL3 -lSDL3_ttf -lm $(shell pkg-config --libs sdl3-ttf)

EM_INCLUDES=-I$(shell em-config CACHE)/sysroot/include
EM_LIBPATH=-L$(shell em-config CACHE)/sysroot/lib

EM_FLAGS=-s USE_SDL=3 -s USE_SDL_TTF=3 -s WASM=1 \
    -s EXPORTED_RUNTIME_METHODS='["ccall","cwrap"]' \
    -s EXPORTED_FUNCTIONS='["_main"]' \
    -s ALLOW_MEMORY_GROWTH=1

EXAMPLES=$(wildcard */main.c)
TARGETS=$(EXAMPLES:/main.c=)
EXECUTABLES=$(TARGETS:%=%/example)
WEB_BUILD_DIR=web_build

.PHONY: all clean web web_clean

all: $(EXECUTABLES)

%/example: %/main.c ../libspark2d.a
	$(CC) $(CFLAGS) $< -o $@ $(LDFLAGS)

web: web_dirs
	@for example in $(TARGETS); do \
		echo "Building web version of $$example..."; \
		mkdir -p $(WEB_BUILD_DIR)/$$example; \
		$(EMCC) $(CFLAGS) $(EM_INCLUDES) $(EM_LIBPATH) \
		$$example/main.c ../src/*.c \
		-I../include \
		$(EM_FLAGS) \
		-s USE_SDL=3 \
		-s USE_SDL_TTF=3 \
		-l SDL3 \
		-l SDL3_ttf \
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

web_clean:
	rm -rf $(WEB_BUILD_DIR)