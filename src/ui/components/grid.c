#include "spark_ui/components/grid.h"
#include "spark_ui/components/button.h"
#include "spark_ui/core/ui_system.h"
#include "spark_mouse.h"
#include <stdlib.h>
#include <math.h>

#define INITIAL_CAPACITY 16
#define SCROLLBAR_WIDTH 15.0f
#define MIN_SCROLLBAR_HEIGHT 30.0f

SparkGrid* spark_ui_grid_new(int columns, float padding) {
    SparkGrid* grid = (SparkGrid*)malloc(sizeof(SparkGrid));
    if (!grid) return NULL;

    grid->x = 0;
    grid->y = 0;
    grid->width = 800;  // Default width
    grid->height = 600; // Default height
    grid->cell_width = (grid->width - (padding * (columns + 1))) / columns;
    grid->cell_height = grid->cell_width;  // Square cells by default
    grid->padding = padding;
    grid->columns = columns;
    grid->scroll_y = 0;
    grid->max_scroll = 0;
    grid->scrolling = false;
    
    grid->buttons = (SparkButton**)malloc(sizeof(SparkButton*) * INITIAL_CAPACITY);
    grid->button_count = 0;
    grid->capacity = INITIAL_CAPACITY;
    
    grid->scrollbar_width = SCROLLBAR_WIDTH;
    grid->scrollbar_hovered = false;
    grid->scrollbar_dragging = false;
    grid->scrollbar_y = 0;
    grid->scrollbar_height = grid->height;

    return grid;
}

void spark_ui_grid_free(SparkGrid* grid) {
    if (!grid) return;
    
    for (int i = 0; i < grid->button_count; i++) {
        spark_ui_button_free(grid->buttons[i]);
    }
    
    free(grid->buttons);
    free(grid);
}

void spark_ui_grid_add_button(SparkGrid* grid, const char* text, SparkButtonCallback callback, void* user_data) {
    if (grid->button_count >= grid->capacity) {
        grid->capacity *= 2;
        SparkButton** new_buttons = (SparkButton**)realloc(grid->buttons, sizeof(SparkButton*) * grid->capacity);
        if (!new_buttons) return;
        grid->buttons = new_buttons;
    }

    int row = grid->button_count / grid->columns;
    int col = grid->button_count % grid->columns;
    
    float x = grid->x + grid->padding + col * (grid->cell_width + grid->padding);
    float y = grid->y + grid->padding + row * (grid->cell_height + grid->padding);
    
    SparkButton* button = spark_ui_button_new_text(x, y, grid->cell_width, grid->cell_height, text);
    spark_ui_button_set_callback(button, callback, user_data);
    
    grid->buttons[grid->button_count++] = button;
    
    // Update max scroll
    float total_height = ceilf((float)grid->button_count / grid->columns) * 
                        (grid->cell_height + grid->padding) + grid->padding;
    grid->max_scroll = fmaxf(0, total_height - grid->height);
}

void spark_ui_grid_update(SparkGrid* grid) {
    float mouse_x, mouse_y;
    spark_ui_get_mouse_position(&mouse_x, &mouse_y);
    
    // Handle scrollbar interaction
    bool mouse_in_scrollbar = (mouse_x >= grid->x + grid->width - grid->scrollbar_width &&
                             mouse_x <= grid->x + grid->width &&
                             mouse_y >= grid->y &&
                             mouse_y <= grid->y + grid->height);
    
    grid->scrollbar_hovered = mouse_in_scrollbar;
    
    if (spark_mouse_is_down(SPARK_MOUSE_LEFT)) {
        if (mouse_in_scrollbar || grid->scrollbar_dragging) {
            grid->scrollbar_dragging = true;
            float scroll_percent = (mouse_y - grid->y - grid->scrollbar_height/2) / 
                                 (grid->height - grid->scrollbar_height);
            grid->scroll_y = fmaxf(0, fminf(grid->max_scroll, scroll_percent * grid->max_scroll));
        }
    } else {
        grid->scrollbar_dragging = false;
    }
    
    // Update buttons
    for (int i = 0; i < grid->button_count; i++) {
        SparkButton* button = grid->buttons[i];
        float original_y = button->y;
        button->y = original_y - grid->scroll_y;
        
        // Only update buttons that are visible
        if (button->y + button->height >= grid->y && 
            button->y <= grid->y + grid->height) {
            spark_ui_button_update(button);
        }
        
        button->y = original_y;  // Restore original position
    }
}

void spark_ui_grid_draw(SparkGrid* grid) {
    // Draw buttons
    for (int i = 0; i < grid->button_count; i++) {
        SparkButton* button = grid->buttons[i];
        float original_y = button->y;
        button->y = original_y - grid->scroll_y;
        
        // Only draw buttons that are visible
        if (button->y + button->height >= grid->y && 
            button->y <= grid->y + grid->height) {
            spark_ui_button_draw(button);
        }
        
        button->y = original_y;  // Restore original position
    }
    
    if (grid->max_scroll > 0) {
        // Draw scrollbar background
        spark_graphics_set_color_with_alpha(0.3f, 0.3f, 0.3f, 1.0f);
        spark_graphics_rectangle("fill",
                               grid->x + grid->width - grid->scrollbar_width,
                               grid->y,
                               grid->scrollbar_width,
                               grid->height);
        
        // Draw scrollbar handle
        float alpha = grid->scrollbar_hovered || grid->scrollbar_dragging ? 1.0f : 0.7f;
        spark_graphics_set_color_with_alpha(0.6f, 0.6f, 0.6f, alpha);
        spark_graphics_rectangle("fill",
                               grid->x + grid->width - grid->scrollbar_width,
                               grid->scrollbar_y,
                               grid->scrollbar_width,
                               grid->scrollbar_height);
    }
}

void spark_ui_grid_set_size(SparkGrid* grid, float width, float height) {
    grid->width = width;
    grid->height = height;
    grid->cell_width = (width - (grid->padding * (grid->columns + 1))) / grid->columns;
    
    // Recalculate button positions
    for (int i = 0; i < grid->button_count; i++) {
        int row = i / grid->columns;
        int col = i % grid->columns;
        
        float x = grid->x + grid->padding + col * (grid->cell_width + grid->padding);
        float y = grid->y + grid->padding + row * (grid->cell_height + grid->padding);
        
        spark_ui_button_set_position(grid->buttons[i], x, y);
        spark_ui_button_set_size(grid->buttons[i], grid->cell_width, grid->cell_height);
    }
}
