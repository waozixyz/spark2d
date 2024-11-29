CC=gcc
CFLAGS=-Wall -Wextra -I../include
LDFLAGS=-L.. -lspark2d -lSDL3
EXAMPLES=$(wildcard */main.c)
TARGETS=$(EXAMPLES:/main.c=)

.PHONY: all clean

all: $(TARGETS)

%: %/main.c ../libspark2d.a
	$(CC) $(CFLAGS) $< -o $@/example $(LDFLAGS)

clean:
	rm -f */example
	