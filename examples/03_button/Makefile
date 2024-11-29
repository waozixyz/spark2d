CC=gcc
CFLAGS=-Wall -Wextra -I../../include
LDFLAGS=-L../.. -lspark2d -lSDL3 -lSDL3_ttf
TARGET=example

.PHONY: all clean

all: $(TARGET)

$(TARGET): main.c ../../libspark2d.a
	$(CC) $(CFLAGS) main.c -o $(TARGET) $(LDFLAGS)

clean:
	rm -f $(TARGET)
	