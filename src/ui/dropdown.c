
// spark_ui_dropdown.c
#include "spark_ui/dropdown.h"
#include "../internal.h"
#include <stdlib.h>
#include <string.h>
#include <lvgl.h>

struct SparkDropdown {
    lv_obj_t* dropdown;        // LVGL dropdown object
    float x;
    float y;
    float width;
    float height;
    SparkDropdownCallback callback;
    void* user_data;
};

static void dropdown_event_cb(lv_event_t* e) {
    SparkDropdown* dropdown = lv_event_get_user_data(e);
    if (!dropdown || !dropdown->callback) return;

    char buf[128];
    lv_dropdown_get_selected_str(dropdown->dropdown, buf, sizeof(buf));
    uint16_t selected = lv_dropdown_get_selected(dropdown->dropdown);
    
    dropdown->callback(selected, buf, dropdown->user_data);
}
SparkDropdown* spark_ui_dropdown_new(float x, float y, float width, float height) {
    SparkDropdown* dropdown = malloc(sizeof(SparkDropdown));
    if (!dropdown) return NULL;

    dropdown->x = x;
    dropdown->y = y;
    dropdown->width = width;
    dropdown->height = height;
    dropdown->callback = NULL;
    dropdown->user_data = NULL;

    // Create LVGL dropdown - use current container if set
    lv_obj_t* parent = spark.current_container ? 
        (lv_obj_t*)spark_ui_container_get_native_handle(spark.current_container) : 
        lv_scr_act();

    dropdown->dropdown = lv_dropdown_create(parent);
    if (!dropdown->dropdown) {
        free(dropdown);
        return NULL;
    }

    // Set initial position and size
    lv_obj_set_pos(dropdown->dropdown, (lv_coord_t)x, (lv_coord_t)y);
    lv_obj_set_size(dropdown->dropdown, (lv_coord_t)width, (lv_coord_t)height);

    // Add event handler
    lv_obj_add_event_cb(dropdown->dropdown, dropdown_event_cb, LV_EVENT_VALUE_CHANGED, dropdown);

    return dropdown;
}


void spark_ui_dropdown_free(SparkDropdown* dropdown) {
    if (!dropdown) return;
    
    if (dropdown->dropdown) {
        lv_obj_del(dropdown->dropdown);
    }
    free(dropdown);
}

void spark_ui_dropdown_add_item(SparkDropdown* dropdown, const char* item) {
    if (!dropdown || !item) return;
    lv_dropdown_add_option(dropdown->dropdown, item, LV_DROPDOWN_POS_LAST);
}

void spark_ui_dropdown_remove_item(SparkDropdown* dropdown, int index) {
    if (!dropdown) return;
    // LVGL doesn't have direct remove option, need to rebuild options
    char* options = lv_dropdown_get_options(dropdown->dropdown);
    char* new_options = strdup(options);
    // Remove the specified option from new_options string
    // This requires parsing the \n separated string
    lv_dropdown_set_options(dropdown->dropdown, new_options);
    free(new_options);
}

void spark_ui_dropdown_clear_items(SparkDropdown* dropdown) {
    if (!dropdown) return;
    lv_dropdown_clear_options(dropdown->dropdown);
}

void spark_ui_dropdown_set_selected(SparkDropdown* dropdown, int index) {
    if (!dropdown) return;
    lv_dropdown_set_selected(dropdown->dropdown, index);
}

int spark_ui_dropdown_get_selected(const SparkDropdown* dropdown) {
    if (!dropdown) return -1;
    return lv_dropdown_get_selected(dropdown->dropdown);
}

const char* spark_ui_dropdown_get_selected_text(const SparkDropdown* dropdown) {
    if (!dropdown) return NULL;
    static char buf[128];  // Static buffer for the selected text
    lv_dropdown_get_selected_str(dropdown->dropdown, buf, sizeof(buf));
    return buf;
}

void spark_ui_dropdown_set_callback(SparkDropdown* dropdown, SparkDropdownCallback callback, void* user_data) {
    if (!dropdown) return;
    dropdown->callback = callback;
    dropdown->user_data = user_data;
}

void spark_ui_dropdown_set_position(SparkDropdown* dropdown, float x, float y) {
    if (!dropdown) return;
    dropdown->x = x;
    dropdown->y = y;
    lv_obj_set_pos(dropdown->dropdown, (lv_coord_t)x, (lv_coord_t)y);
}

void spark_ui_dropdown_set_size(SparkDropdown* dropdown, float width, float height) {
    if (!dropdown) return;
    dropdown->width = width;
    dropdown->height = height;
    lv_obj_set_size(dropdown->dropdown, (lv_coord_t)width, (lv_coord_t)height);
}

void spark_ui_dropdown_get_position(const SparkDropdown* dropdown, float* x, float* y) {
    if (!dropdown) return;
    if (x) *x = dropdown->x;
    if (y) *y = dropdown->y;
}

void spark_ui_dropdown_get_size(const SparkDropdown* dropdown, float* width, float* height) {
    if (!dropdown) return;
    if (width) *width = dropdown->width;
    if (height) *height = dropdown->height;
}

bool spark_ui_dropdown_is_open(const SparkDropdown* dropdown) {
    if (!dropdown) return false;
    return lv_dropdown_is_open(dropdown->dropdown);
}