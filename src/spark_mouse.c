#include "spark_mouse.h"
#include <SDL2/SDL.h>
#include "internal.h"
 


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
    SDL_ShowCursor(SDL_ENABLE);
    return true;
}



void spark_mouse_shutdown(void) {
    if (mouse_state.current_cursor) {
        spark_mouse_free_cursor(mouse_state.current_cursor);
    }
}

void spark_mouse_get_position(float* x, float* y) {
    int wx, wy;
    SDL_GetMouseState(&wx, &wy);
    if (x) *x = (float)wx;
    if (y) *y = (float)wy;
}

float spark_mouse_get_x(void) {
    int x, y;
    SDL_GetMouseState(&x, &y);
    return (float)x;
}

float spark_mouse_get_y(void) {
    int x, y;
    SDL_GetMouseState(&x, &y);
    return (float)y;
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
    return (state & SDL_BUTTON(button)) != 0;
}

SparkCursor* spark_mouse_get_cursor(void) {
    return mouse_state.current_cursor;
}

SparkCursor* spark_mouse_get_system_cursor(SparkCursorType type) {
    SDL_SystemCursor sdl_type;
    switch (type) {
        case SPARK_CURSOR_ARROW: sdl_type = SDL_SYSTEM_CURSOR_ARROW; break;
        case SPARK_CURSOR_IBEAM: sdl_type = SDL_SYSTEM_CURSOR_IBEAM; break;
        case SPARK_CURSOR_WAIT: sdl_type = SDL_SYSTEM_CURSOR_WAIT; break;
        case SPARK_CURSOR_CROSSHAIR: sdl_type = SDL_SYSTEM_CURSOR_CROSSHAIR; break;
        case SPARK_CURSOR_WAITARROW: sdl_type = SDL_SYSTEM_CURSOR_WAITARROW; break;
        case SPARK_CURSOR_SIZENWSE: sdl_type = SDL_SYSTEM_CURSOR_SIZENWSE; break;
        case SPARK_CURSOR_SIZENESW: sdl_type = SDL_SYSTEM_CURSOR_SIZENESW; break;
        case SPARK_CURSOR_SIZEWE: sdl_type = SDL_SYSTEM_CURSOR_SIZEWE; break;
        case SPARK_CURSOR_SIZENS: sdl_type = SDL_SYSTEM_CURSOR_SIZENS; break;
        case SPARK_CURSOR_SIZEALL: sdl_type = SDL_SYSTEM_CURSOR_SIZEALL; break;
        case SPARK_CURSOR_NO: sdl_type = SDL_SYSTEM_CURSOR_NO; break;
        case SPARK_CURSOR_HAND: sdl_type = SDL_SYSTEM_CURSOR_HAND; break;
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

    SDL_Surface* surface = SDL_CreateRGBSurfaceFrom((void*)pixels, width, height, 32,
                                                   width * 4,
                                                   0xFF000000,
                                                   0x00FF0000,
                                                   0x0000FF00,
                                                   0x000000FF);

    if (!surface) {
        free(cursor);
        return NULL;
    }

    cursor->sdl_cursor = SDL_CreateColorCursor(surface, hot_x, hot_y);
    SDL_FreeSurface(surface);

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
            SDL_FreeCursor(cursor->sdl_cursor);
        }
        free(cursor);
    }
}

bool spark_mouse_is_visible(void) {
    return mouse_state.is_visible;
}

void spark_mouse_set_visible(bool visible) {
    mouse_state.is_visible = visible;
    SDL_ShowCursor(visible ? SDL_ENABLE : SDL_DISABLE);
}

bool spark_mouse_is_grabbed(void) {
    return mouse_state.is_grabbed;
}

void spark_mouse_set_grabbed(bool grabbed) {
    mouse_state.is_grabbed = grabbed;
    SDL_SetWindowGrab(spark.window, grabbed ? SDL_TRUE : SDL_FALSE);
}

bool spark_mouse_get_relative_mode(void) {
    return mouse_state.relative_mode;
}

void spark_mouse_set_relative_mode(bool enable) {
    mouse_state.relative_mode = enable;
    SDL_SetRelativeMouseMode(enable ? SDL_TRUE : SDL_FALSE);
}