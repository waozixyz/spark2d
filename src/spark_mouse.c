#include "spark_mouse.h"
#include <SDL3/SDL.h>
#include "internal.h" 

#ifndef SDL_BUTTON_MASK
#define SDL_BUTTON_MASK(X)  (1 << ((X)-1))
#endif

static struct {
    SparkCursor* current_cursor;
    bool is_visible;
    bool is_grabbed;
    bool relative_mode;
} mouse_state = {
    .current_cursor = NULL,
    .is_visible = true,
    .is_grabbed = false,
    .relative_mode = false
};

bool spark_mouse_init(void) {
    mouse_state.current_cursor = NULL;
    mouse_state.is_visible = true;
    SDL_ShowCursor();
    return true;
}

void spark_mouse_shutdown(void) {
    if (mouse_state.current_cursor) {
        spark_mouse_free_cursor(mouse_state.current_cursor);
    }
}

void spark_mouse_get_position(float* x, float* y) {
    float wx, wy;
    SDL_GetMouseState(&wx, &wy);
    if (x) *x = wx;
    if (y) *y = wy;
}

float spark_mouse_get_x(void) {
    float x, y;
    SDL_GetMouseState(&x, &y);
    return x;
}

float spark_mouse_get_y(void) {
    float x, y;
    SDL_GetMouseState(&x, &y);
    return y;
}

void spark_mouse_set_position(float x, float y) {
    SDL_WarpMouseInWindow(spark.window, (int)x, (int)y);
}

void spark_mouse_set_x(float x) {
    float current_y = spark_mouse_get_y();
    spark_mouse_set_position(x, current_y);
}

void spark_mouse_set_y(float y) {
    float current_x = spark_mouse_get_x();
    spark_mouse_set_position(current_x, y);
}

bool spark_mouse_is_down(SparkMouseButton button) {
    uint32_t state = SDL_GetMouseState(NULL, NULL);
    return (state & SDL_BUTTON_MASK(button)) != 0;
}

SparkCursor* spark_mouse_get_cursor(void) {
    return mouse_state.current_cursor;
}

SparkCursor* spark_mouse_get_system_cursor(SparkCursorType type) {
    SDL_SystemCursor sdl_type;
    switch (type) {
        case SPARK_CURSOR_ARROW: sdl_type = SDL_SYSTEM_CURSOR_DEFAULT; break; 
        case SPARK_CURSOR_IBEAM: sdl_type = SDL_SYSTEM_CURSOR_TEXT; break;
        case SPARK_CURSOR_WAIT: sdl_type = SDL_SYSTEM_CURSOR_WAIT; break;
        case SPARK_CURSOR_CROSSHAIR: sdl_type = SDL_SYSTEM_CURSOR_CROSSHAIR; break;
        case SPARK_CURSOR_WAITARROW: sdl_type = SDL_SYSTEM_CURSOR_PROGRESS; break; 
        case SPARK_CURSOR_SIZENWSE: sdl_type = SDL_SYSTEM_CURSOR_NWSE_RESIZE; break;
        case SPARK_CURSOR_SIZENESW: sdl_type = SDL_SYSTEM_CURSOR_NESW_RESIZE; break;
        case SPARK_CURSOR_SIZEWE: sdl_type = SDL_SYSTEM_CURSOR_EW_RESIZE; break;
        case SPARK_CURSOR_SIZENS: sdl_type = SDL_SYSTEM_CURSOR_NS_RESIZE; break;
        case SPARK_CURSOR_SIZEALL: sdl_type = SDL_SYSTEM_CURSOR_MOVE; break;
        case SPARK_CURSOR_NO: sdl_type = SDL_SYSTEM_CURSOR_NOT_ALLOWED; break;
        case SPARK_CURSOR_HAND: sdl_type = SDL_SYSTEM_CURSOR_POINTER; break;
        default: return NULL;
    }
    
    SparkCursor* cursor = malloc(sizeof(SparkCursor));
    if (!cursor) return NULL;

    cursor->sdl_cursor = SDL_CreateSystemCursor(sdl_type);
    if (!cursor->sdl_cursor) {
        free(cursor);
        return NULL;
    }

    cursor->width = 32;  
    cursor->height = 32;
    cursor->hot_x = 0;
    cursor->hot_y = 0;

    return cursor;
}

bool spark_mouse_has_cursor(void) {
    return SDL_GetCursor() != NULL;
}

SparkCursor* spark_mouse_new_cursor(const unsigned char* pixels, int width, int height, int hot_x, int hot_y) {
    SparkCursor* cursor = malloc(sizeof(SparkCursor));
    if (!cursor) return NULL;

    SDL_Surface* surface = SDL_CreateSurfaceFrom(width, height,
                                                SDL_PIXELFORMAT_RGBA32,
                                                (void*)pixels,
                                                width * 4);

    if (!surface) {
        free(cursor);
        return NULL;
    }

    cursor->sdl_cursor = SDL_CreateColorCursor(surface, hot_x, hot_y);
    SDL_DestroySurface(surface);

    if (!cursor->sdl_cursor) {
        free(cursor);
        return NULL;
    }

    cursor->width = width;
    cursor->height = height;
    cursor->hot_x = hot_x;
    cursor->hot_y = hot_y;

    return cursor;
}

void spark_mouse_set_cursor(SparkCursor* cursor) {
    if (cursor && cursor->sdl_cursor) {
        SDL_SetCursor(cursor->sdl_cursor);
        mouse_state.current_cursor = cursor;
    }
}

void spark_mouse_free_cursor(SparkCursor* cursor) {
    if (cursor) {
        if (cursor->sdl_cursor) {
            SDL_DestroyCursor(cursor->sdl_cursor);
        }
        free(cursor);
    }
}

bool spark_mouse_is_visible(void) {
    return mouse_state.is_visible;
}

void spark_mouse_set_visible(bool visible) {
    mouse_state.is_visible = visible;
    if (visible) {
        SDL_ShowCursor();
    } else {
        SDL_HideCursor();
    }
}

bool spark_mouse_is_grabbed(void) {
    return mouse_state.is_grabbed;
}

void spark_mouse_set_grabbed(bool grabbed) {
    mouse_state.is_grabbed = grabbed;
    SDL_SetWindowMouseGrab(spark.window, grabbed);
}

bool spark_mouse_get_relative_mode(void) {
    return mouse_state.relative_mode;
}
void spark_mouse_set_relative_mode(bool enable) {
    mouse_state.relative_mode = enable;
    if (enable) {
        float x, y;
        SDL_GetRelativeMouseState(&x, &y);
    }
}