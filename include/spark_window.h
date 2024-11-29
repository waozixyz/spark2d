
// spark_window.h
#ifndef SPARK_WINDOW_H
#define SPARK_WINDOW_H

#include <SDL3/SDL_rect.h>


typedef enum {
    SPARK_SCALE_NONE,             // No scaling, just stretch
    SPARK_SCALE_MAINTAIN_WIDTH,   // Keep width ratio, adjust height
    SPARK_SCALE_MAINTAIN_HEIGHT,  // Keep height ratio, adjust width
    SPARK_SCALE_MAINTAIN_ASPECT,  // Keep aspect ratio with letterboxing/pillarboxing
} SparkScaleMode;


typedef enum {
    SPARK_WINDOW_MODE_FIXED,       // Window size is fixed
    SPARK_WINDOW_MODE_RESPONSIVE,  // Window can be resized
} SparkWindowMode;

typedef struct {
    int base_width;          // Original/base window width
    int base_height;         // Original/base window height
    float scale_x;           // Current horizontal scale factor
    float scale_y;           // Current vertical scale factor
    SparkWindowMode mode;    // Current window mode
    SparkScaleMode scale;    // Current scale mode
    SDL_Rect viewport;       // Current viewport for maintaining aspect ratio
} SparkWindowState;

// Function declarations
void spark_window_set_mode(SparkWindowMode mode);
void spark_window_set_scale_mode(SparkScaleMode scale);
void spark_window_get_scale(float* scale_x, float* scale_y);
void spark_window_update_scale(void);

void spark_window_set_title(const char* title);

#endif
