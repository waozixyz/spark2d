#ifndef SPARK_MOUSE_H
#define SPARK_MOUSE_H

#include <stdbool.h>
#include <stdlib.h>

// Mouse button constants
typedef enum SparkMouseButton {
    SPARK_MOUSE_LEFT = 1,
    SPARK_MOUSE_MIDDLE = 2,
    SPARK_MOUSE_RIGHT = 3,
    SPARK_MOUSE_X1 = 4,
    SPARK_MOUSE_X2 = 5
} SparkMouseButton;

// Cursor types enum (similar to LÖVE's CursorType)
typedef enum SparkCursorType {
    SPARK_CURSOR_ARROW,
    SPARK_CURSOR_IBEAM,
    SPARK_CURSOR_WAIT,
    SPARK_CURSOR_CROSSHAIR,
    SPARK_CURSOR_WAITARROW,
    SPARK_CURSOR_SIZENWSE,
    SPARK_CURSOR_SIZENESW,
    SPARK_CURSOR_SIZEWE,
    SPARK_CURSOR_SIZENS,
    SPARK_CURSOR_SIZEALL,
    SPARK_CURSOR_NO,
    SPARK_CURSOR_HAND
} SparkCursorType;

// Cursor structure
typedef struct SparkCursor {
    void* sdl_cursor;  // Internal SDL cursor pointer
    int width;
    int height;
    int hot_x;
    int hot_y;
} SparkCursor;

// Core functions
bool spark_mouse_init(void);
void spark_mouse_shutdown(void);

// Position functions
void spark_mouse_get_position(float* x, float* y);
float spark_mouse_get_x(void);
float spark_mouse_get_y(void);
void spark_mouse_set_position(float x, float y);
void spark_mouse_set_x(float x);
void spark_mouse_set_y(float y);

// Button state
bool spark_mouse_is_down(SparkMouseButton button);

// Cursor management
SparkCursor* spark_mouse_get_cursor(void);
SparkCursor* spark_mouse_get_system_cursor(SparkCursorType type);
bool spark_mouse_has_cursor(void);
SparkCursor* spark_mouse_new_cursor(const unsigned char* pixels, int width, int height, int hot_x, int hot_y);
void spark_mouse_set_cursor(SparkCursor* cursor);
void spark_mouse_free_cursor(SparkCursor* cursor);

// Visibility and grabbing
bool spark_mouse_is_visible(void);
void spark_mouse_set_visible(bool visible);
bool spark_mouse_is_grabbed(void);
void spark_mouse_set_grabbed(bool grabbed);

// Relative mode
bool spark_mouse_get_relative_mode(void);
void spark_mouse_set_relative_mode(bool enable);

#endif // SPARK_MOUSE_H
