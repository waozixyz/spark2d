#ifndef SPARK_UI_GRID_H
#define SPARK_UI_GRID_H

#include "../common.h"

typedef struct SparkGrid {
    float x, y;
    float width, height;
    float cell_width, cell_height;
    float padding;
    int columns;
    float scroll_y;
    float max_scroll;
    bool scrolling;
    
    SparkButton** buttons;
    int button_count;
    int capacity;
    
    // Scrollbar properties
    float scrollbar_width;
    bool scrollbar_hovered;
    bool scrollbar_dragging;
    float scrollbar_y;
    float scrollbar_height;
} SparkGrid;

// Creation and destruction
SparkGrid* spark_ui_grid_new(int columns, float padding);
void spark_ui_grid_free(SparkGrid* grid);

// Grid configuration
void spark_ui_grid_set_position(SparkGrid* grid, float x, float y);
void spark_ui_grid_set_size(SparkGrid* grid, float width, float height);
void spark_ui_grid_set_cell_size(SparkGrid* grid, float width, float height);

// Button management
void spark_ui_grid_add_button(SparkGrid* grid, const char* text, SparkButtonCallback callback, void* user_data);
void spark_ui_grid_clear(SparkGrid* grid);

// Grid state queries
void spark_ui_grid_get_position(SparkGrid* grid, float* x, float* y);
void spark_ui_grid_get_size(SparkGrid* grid, float* width, float* height);
void spark_ui_grid_get_cell_size(SparkGrid* grid, float* width, float* height);

// Grid rendering and updates
void spark_ui_grid_update(SparkGrid* grid);
void spark_ui_grid_draw(SparkGrid* grid);

#endif /* SPARK_UI_GRID_H */
