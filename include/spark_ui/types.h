#ifndef SPARK_UI_TYPES_H
#define SPARK_UI_TYPES_H

#include <stdbool.h>
#include "spark_font.h"
#include "spark_graphics.h"

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

// Now define button structure
typedef struct SparkButton {
    float x, y, width, height;
    SparkButtonType type;
    char* text;
    SparkText* text_texture;
    SparkIcon* icon;
    SparkButtonCallback callback;
    void* user_data;
    bool hovered;
    bool pressed;
    SparkFont* font;
} SparkButton;


// Tab related structures
typedef struct SparkTab {
    SparkButton* button;
    bool active;
    struct SparkTabBar* parent_tabbar;
} SparkTab;

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
} SparkTabBar;

#endif /* SPARK_UI_TYPES_H */
