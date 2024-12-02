#ifndef SPARK_UI_DROPDOWN_H
#define SPARK_UI_DROPDOWN_H

#include "../common.h"
#include "spark_font.h"
#include "spark_graphics.h"
#include "spark_theme.h"

typedef struct SparkDropdown SparkDropdown;
typedef void (*SparkDropdownCallback)(int selected_index, const char* selected_item, void* user_data);

// Creation and destruction
SparkDropdown* spark_ui_dropdown_new(float x, float y, float width, float height);
void spark_ui_dropdown_free(SparkDropdown* dropdown);

// Item management
void spark_ui_dropdown_add_item(SparkDropdown* dropdown, const char* item);
void spark_ui_dropdown_remove_item(SparkDropdown* dropdown, int index);
void spark_ui_dropdown_clear_items(SparkDropdown* dropdown);
void spark_ui_dropdown_set_selected(SparkDropdown* dropdown, int index);
int spark_ui_dropdown_get_selected(const SparkDropdown* dropdown);
const char* spark_ui_dropdown_get_selected_text(const SparkDropdown* dropdown);

// Dropdown configuration
void spark_ui_dropdown_set_callback(SparkDropdown* dropdown, SparkDropdownCallback callback, void* user_data);
void spark_ui_dropdown_set_font(SparkDropdown* dropdown, SparkFont* font);
void spark_ui_dropdown_set_position(SparkDropdown* dropdown, float x, float y);
void spark_ui_dropdown_set_size(SparkDropdown* dropdown, float width, float height);

// Dropdown state queries
void spark_ui_dropdown_get_position(const SparkDropdown* dropdown, float* x, float* y);
void spark_ui_dropdown_get_size(const SparkDropdown* dropdown, float* width, float* height);
bool spark_ui_dropdown_is_open(const SparkDropdown* dropdown);

// Dropdown rendering and updates
void spark_ui_dropdown_update(SparkDropdown* dropdown);
void spark_ui_dropdown_draw(SparkDropdown* dropdown);

#endif /* SPARK_UI_DROPDOWN_H */
