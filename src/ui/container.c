#include "spark_ui/container.h"
#include "../internal.h"
#include <stdlib.h>

struct SparkContainer {
    lv_obj_t* container;
    float x;
    float y;
    float width;
    float height;
    bool is_visible;
};

SparkContainer* spark_ui_container_create(const SparkContainerBuilder* builder) {
    SparkContainer* container = calloc(1, sizeof(SparkContainer));
    if (!container) return NULL;

    // Create LVGL container
    container->container = lv_obj_create(lv_screen_active());
    if (!container->container) {
        free(container);
        return NULL;
    }

    // Store dimensions
    container->x = builder->x;
    container->y = builder->y;
    container->width = builder->width;
    container->height = builder->height;
    container->is_visible = true;

    // Configure container
    if (builder->full_width) {
        lv_obj_set_width(container->container, LV_PCT(100));
    } else {
        lv_obj_set_width(container->container, (lv_coord_t)builder->width);
    }

    if (builder->full_height) {
        lv_obj_set_height(container->container, LV_PCT(100));
    } else {
        lv_obj_set_height(container->container, (lv_coord_t)builder->height);
    }

    lv_obj_set_pos(container->container, (lv_coord_t)builder->x, (lv_coord_t)builder->y);

    if (builder->transparent) {
        lv_obj_set_style_bg_opa(container->container, LV_OPA_TRANSP, 0);
    }

    if (builder->centered) {
        lv_obj_set_align(container->container, LV_ALIGN_CENTER);
    }

    return container;
}

void spark_ui_container_free(SparkContainer* container) {
    if (!container) return;
    if (container->container) {
        lv_obj_del(container->container);
    }
    free(container);
}

void spark_ui_container_set_parent(SparkContainer* container, SparkContainer* parent) {
    if (!container || !container->container) return;
    if (!parent) {
        lv_obj_set_parent(container->container, lv_screen_active());
    } else {
        lv_obj_set_parent(container->container, parent->container);
    }
}

SparkContainer* spark_ui_container_get_parent(const SparkContainer* container) {
    if (!container || !container->container) return NULL;
    lv_obj_t* parent = lv_obj_get_parent(container->container);
    if (!parent || parent == lv_screen_active()) return NULL;
    return lv_obj_get_user_data(parent);
}

void* spark_ui_container_get_native_handle(const SparkContainer* container) {
    if (!container) return NULL;
    return container->container;
}

void spark_ui_container_set_position(SparkContainer* container, float x, float y) {
    if (!container || !container->container) return;
    container->x = x;
    container->y = y;
    lv_obj_set_pos(container->container, (lv_coord_t)x, (lv_coord_t)y);
}

void spark_ui_container_set_size(SparkContainer* container, float width, float height) {
    if (!container || !container->container) return;
    container->width = width;
    container->height = height;
    lv_obj_set_size(container->container, (lv_coord_t)width, (lv_coord_t)height);
}

void spark_ui_container_get_position(const SparkContainer* container, float* x, float* y) {
    if (!container) return;
    if (x) *x = container->x;
    if (y) *y = container->y;
}

void spark_ui_container_get_size(const SparkContainer* container, float* width, float* height) {
    if (!container) return;
    if (width) *width = container->width;
    if (height) *height = container->height;
}

void spark_ui_container_clear(SparkContainer* container) {
    if (!container || !container->container) return;
    lv_obj_clean(container->container);
}

void spark_ui_container_set_visible(SparkContainer* container, bool visible) {
    if (!container || !container->container) return;
    container->is_visible = visible;
    if (visible) {
        lv_obj_clear_flag(container->container, LV_OBJ_FLAG_HIDDEN);
    } else {
        lv_obj_add_flag(container->container, LV_OBJ_FLAG_HIDDEN);
    }
}

bool spark_ui_container_is_visible(const SparkContainer* container) {
    if (!container) return false;
    return container->is_visible;
}

void spark_ui_container_set_transparent(SparkContainer* container, bool transparent) {
    if (!container || !container->container) return;
    lv_obj_set_style_bg_opa(container->container, 
                           transparent ? LV_OPA_TRANSP : LV_OPA_COVER, 
                           0);
}

void spark_ui_container_set_background_color(SparkContainer* container, float r, float g, float b) {
    if (!container || !container->container) return;
    lv_color_t color = lv_color_make((uint8_t)(r * 255), 
                                    (uint8_t)(g * 255), 
                                    (uint8_t)(b * 255));
    lv_obj_set_style_bg_color(container->container, color, 0);
}

void spark_ui_container_set_background_opacity(SparkContainer* container, float opacity) {
    if (!container || !container->container) return;
    lv_obj_set_style_bg_opa(container->container, (lv_opa_t)(opacity * 255), 0);
}

void spark_ui_set_container(SparkContainer* container) {
    spark.current_container = container;
}

SparkContainer* spark_ui_get_current_container(void) {
    return spark.current_container;
}
