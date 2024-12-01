#ifndef SPARK_UI_TYPES_H
#define SPARK_UI_TYPES_H

#include <stdbool.h>
#include "spark_font.h"
#include "spark_graphics.h"



#define SPARK_MAX_TABS 32  

// Forward declarations
struct SparkButton;
struct SparkTabBar;

// Define enums first
typedef enum {
    SPARK_BUTTON_TEXT,
    SPARK_BUTTON_ICON,
    SPARK_BUTTON_TEXT_AND_ICON
} SparkButtonType;

typedef enum {
    SPARK_TAB_TOP,
    SPARK_TAB_BOTTOM
} SparkTabPosition;

// Define callback types
typedef void (*SparkButtonCallback)(void* user_data);
typedef void (*SparkTabCallback)(int tab_index);


typedef struct SparkButton {
    float x, y;
    float width, height;
    SparkButtonType type;
    char* text;
    SparkText* text_texture;
    SparkImage* image;
    SparkButtonCallback callback;
    void* user_data;
    bool hovered;
    bool pressed;
    SparkFont* font;
} SparkButton;



typedef struct SparkTab {
    SparkButton* button;
    bool active;
    struct SparkTabBar* parent_tabbar;
    bool enabled;
    void* user_data;
} SparkTab;

typedef enum SparkTabBarError {
    SPARK_TABBAR_SUCCESS,
    SPARK_TABBAR_ERROR_MEMORY,
    SPARK_TABBAR_ERROR_INVALID_INDEX,
    SPARK_TABBAR_ERROR_MAX_TABS
} SparkTabBarError;

// Add animation config
typedef struct SparkTabAnimationConfig {
    float transition_speed;
    bool smooth_scrolling;
    bool fade_effect;
    float indicator_thickness;
} SparkTabAnimationConfig;

// Add tab config
typedef struct SparkTabConfig {
    const char* text;
    SparkImage* image;
    bool enabled;
    void* user_data;
} SparkTabConfig;

// Add tab group
typedef struct SparkTabGroup {
    const char* name;
    SparkTabConfig* tabs;
    int tab_count;
} SparkTabGroup;

// Add builder
typedef struct SparkTabBarBuilder {
    SparkTabPosition position;
    float height;
    float max_width;
    SparkTabCallback callback;
    bool centered;
    bool auto_resize;
} SparkTabBarBuilder;

// Update SparkTabBar structure
typedef struct SparkTabBar {
    float x, y, width, height;
    SparkTab* tabs;
    int tab_count;
    int capacity;
    int active_tab;
    SparkTabCallback callback;
    SparkTabPosition position;
    float max_width;
    float indicator_pos;
    float indicator_target;
    // New fields
    bool centered;
    bool auto_resize;
    SparkTabAnimationConfig animation;
} SparkTabBar;

#endif /* SPARK_UI_TYPES_H */
