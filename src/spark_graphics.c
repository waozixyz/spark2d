#include "spark_graphics.h"
#include "spark_font.h"

#include "internal.h"
#include <stdlib.h>
#include <string.h>

static SparkFont* default_font = NULL;

static void ensure_default_font(void) {
   if (!default_font) {
       default_font = spark_font_new_default(spark.renderer); 
   }
}

void spark_graphics_cleanup(void) {
   if (default_font) {
       spark_font_free(default_font);
       default_font = NULL;
   }
}

void spark_graphics_set_color(float r, float g, float b) {
   spark_graphics_set_color_with_alpha(r, g, b, 1.0f);
}

void spark_graphics_set_color_with_alpha(float r, float g, float b, float a) {
   SDL_SetRenderDrawColor(spark.renderer,
       (uint8_t)(r * 255),
       (uint8_t)(g * 255),
       (uint8_t)(b * 255),
       (uint8_t)(a * 255));
}

void spark_graphics_rectangle(const char* mode, float x, float y, float w, float h) {
   SDL_FRect rect = {x, y, w, h};
   if (strcmp(mode, "fill") == 0) {
       SDL_RenderFillRect(spark.renderer, &rect);
   } else if (strcmp(mode, "line") == 0) {
       SDL_RenderRect(spark.renderer, &rect);
   }
}

void spark_graphics_clear(void) {
   SDL_RenderClear(spark.renderer);
}

void spark_graphics_present(void) {
   SDL_RenderPresent(spark.renderer);
}

SparkText* spark_graphics_new_text(SparkFont* font, const char* text) {
   SparkText* txt = malloc(sizeof(SparkText));
   txt->font = font ? font : default_font;
   txt->color = (SDL_Color){255, 255, 255, 255};
   txt->text = strdup(text);
   txt->width = strlen(text) * spark_font_get_width(txt->font);
   txt->height = spark_font_get_height(txt->font);
   txt->texture = NULL;
   return txt;
}

void spark_graphics_text_set_color(SparkText* text, float r, float g, float b, float a) {
   text->color.r = (uint8_t)(r * 255);
   text->color.g = (uint8_t)(g * 255);
   text->color.b = (uint8_t)(b * 255);
   text->color.a = (uint8_t)(a * 255);
}

void spark_graphics_text_draw(SparkText* text, float x, float y) {
   ensure_default_font();
   SparkFont* font = text->font ? text->font : default_font;
   
   float current_x = x;
   for (size_t i = 0; i < strlen(text->text); i++) {
       spark_font_draw_char(font, text->text[i], current_x, y);
       current_x += spark_font_get_width(font);
   }
}

void spark_graphics_text_free(SparkText* text) {
   if (text) {
       if (text->text) {
           free((void*)text->text);
       }
       free(text);
   }
}

void spark_graphics_print(const char* text, float x, float y) {
   ensure_default_font();
   
   float current_x = x;
   for (size_t i = 0; i < strlen(text); i++) {
       spark_font_draw_char(default_font, text[i], current_x, y);
       current_x += spark_font_get_width(default_font);
   }
}

void spark_graphics_print_centered(const char* text, float x, float y, float w, float h) {
   ensure_default_font();
   
   size_t len = strlen(text);
   float text_width = len * spark_font_get_width(default_font);
   float text_x = x + (w - text_width) / 2;
   float text_y = y + (h - spark_font_get_height(default_font)) / 2;
   
   spark_graphics_print(text, text_x, text_y);
}
