#ifndef SPARK_UI_CONTAINER_H
#define SPARK_UI_CONTAINER_H

#include "../../../deps/lvgl/lvgl.h"
#include <stdbool.h>

typedef struct SparkContainer SparkContainer;

typedef struct {
    float x;
    float y;
    float width;
    float height;
    bool full_width;
    bool full_height;
    bool transparent;
    bool centered;
} SparkContainerBuilder;

// Creation and destruction
SparkContainer* spark_ui_container_create(const SparkContainerBuilder* builder);
void spark_ui_container_free(SparkContainer* container);

// Parent/Child management
void spark_ui_container_set_parent(SparkContainer* container, SparkContainer* parent);
SparkContainer* spark_ui_container_get_parent(const SparkContainer* container);
void* spark_ui_container_get_native_handle(const SparkContainer* container);

// Container properties
void spark_ui_container_set_position(SparkContainer* container, float x, float y);
void spark_ui_container_set_size(SparkContainer* container, float width, float height);
void spark_ui_container_get_position(const SparkContainer* container, float* x, float* y);
void spark_ui_container_get_size(const SparkContainer* container, float* width, float* height);

// Container operations
void spark_ui_container_clear(SparkContainer* container);
void spark_ui_container_set_visible(SparkContainer* container, bool visible);
bool spark_ui_container_is_visible(const SparkContainer* container);

// Container style
void spark_ui_container_set_transparent(SparkContainer* container, bool transparent);
void spark_ui_container_set_background_color(SparkContainer* container, float r, float g, float b);
void spark_ui_container_set_background_opacity(SparkContainer* container, float opacity);


void spark_ui_set_container(SparkContainer* container);
SparkContainer* spark_ui_get_current_container(void);

#endif
