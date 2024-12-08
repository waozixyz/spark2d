// spark_graphics/text.c
#include "spark_graphics/text.h"
#include "../internal.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

SparkText* spark_graphics_new_text(const char* text) {
    SparkText* txt = malloc(sizeof(SparkText));
    if (!txt) return NULL;

    // Create and initialize style
    txt->style = malloc(sizeof(lv_style_t));
    if (!txt->style) {
        free(txt);
        return NULL;
    }

    lv_style_init(txt->style);
    lv_style_set_text_color(txt->style, lv_color_white());
    lv_style_set_text_font(txt->style, LV_FONT_DEFAULT);
    lv_style_set_text_opa(txt->style, LV_OPA_COVER);

    txt->text = strdup(text);
    txt->color = (SDL_Color){255, 255, 255, 255}; // Default white
    txt->scale = 1.0f;

    // Calculate dimensions
    lv_point_t size;
    lv_txt_get_size(&size, text, LV_FONT_DEFAULT, 
                    0, 0, LV_COORD_MAX, 0);
    txt->width = size.x;
    txt->height = size.y;

    return txt;
}

void spark_graphics_text_draw(SparkText* text, float x, float y) {
    if (!text || !text->text || !text->style) return;

    // Create label object
    lv_obj_t* label = lv_label_create(lv_scr_act());
    if (!label) return;

    // Apply style and position
    lv_obj_add_style(label, text->style, 0);
    lv_obj_set_pos(label, (lv_coord_t)x, (lv_coord_t)y);
    lv_obj_set_size(label, 
                    (lv_coord_t)(text->width * text->scale), 
                    (lv_coord_t)(text->height * text->scale));
    
    // Set text
    lv_label_set_text(label, text->text);

    // Queue deletion after render
    lv_obj_add_flag(label, LV_OBJ_FLAG_HIDDEN);
    lv_obj_del_async(label);
}

void spark_graphics_print(const char* text, float x, float y) {
    SparkText* txt = spark_graphics_new_text(text);
    if (!txt) return;
    
    spark_graphics_text_draw(txt, x, y);
    spark_graphics_text_free(txt);
}

void spark_graphics_printf(const char* text, float x, float y, float wrap_width, SparkTextAlign align) {
    SparkText* txt = spark_graphics_new_text(text);
    if (!txt) return;

    // Configure alignment
    switch (align) {
        case SPARK_TEXT_ALIGN_CENTER:
            lv_style_set_text_align(txt->style, LV_TEXT_ALIGN_CENTER);
            break;
        case SPARK_TEXT_ALIGN_RIGHT:
            lv_style_set_text_align(txt->style, LV_TEXT_ALIGN_RIGHT);
            break;
        default:
            lv_style_set_text_align(txt->style, LV_TEXT_ALIGN_LEFT);
            break;
    }

    // Setup wrapping
    lv_style_set_width(txt->style, (lv_coord_t)wrap_width);
    
    spark_graphics_text_draw(txt, x, y);
    spark_graphics_text_free(txt);
}

void spark_graphics_text_set_color(SparkText* text, float r, float g, float b, float a) {
    if (!text || !text->style) return;
    
    text->color.r = (uint8_t)(r * 255);
    text->color.g = (uint8_t)(g * 255);
    text->color.b = (uint8_t)(b * 255);
    text->color.a = (uint8_t)(a * 255);
    
    lv_style_set_text_color(text->style, 
        lv_color_make(text->color.r, text->color.g, text->color.b));
    lv_style_set_text_opa(text->style, text->color.a);
}

void spark_graphics_text_get_scaled_size(SparkText* text, float* width, float* height) {
    if (!text) return;
    
    float scale_x, scale_y;
    spark_window_get_scale(&scale_x, &scale_y);
    
    if (width) *width = text->width * text->scale * scale_x;
    if (height) *height = text->height * text->scale * scale_y;
}

float spark_graphics_text_get_width(SparkText* text) {
    return text ? text->width * text->scale : 0;
}

float spark_graphics_text_get_height(SparkText* text) {
    return text ? text->height * text->scale : 0;
}

void spark_graphics_text_free(SparkText* text) {
    if (!text) return;
    
    if (text->style) {
        lv_style_reset(text->style);
        free(text->style);
    }
    if (text->text) {
        free((void*)text->text);
    }
    free(text);
}