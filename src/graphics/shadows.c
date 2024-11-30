#include <stdint.h>
#include "spark_graphics/shadows.h"
#include "spark_graphics/color.h"
#include "spark_graphics/primitives.h"

void spark_graphics_rounded_rectangle_shadow(const char* mode, float x, float y, 
                                          float width, float height, 
                                          float radius, float blur) {
    for (int i = 1; i <= (int)blur; i++) {
        float spread = i;
        float alpha = 0.1f * (1.0f - (i / blur));
        
        spark_graphics_set_color_with_alpha(0, 0, 0, alpha);
        
        SDL_FRect shadow_rect = {
            x - spread,
            y - spread,
            width + (spread * 2),
            height + (spread * 2)
        };
        
        spark_graphics_rectangle(mode, shadow_rect.x, shadow_rect.y, 
                               shadow_rect.w, shadow_rect.h);
    }
}

void spark_graphics_rectangle_shadow(const char* mode, float x, float y, 
                                   float width, float height, float blur) {
    spark_graphics_rounded_rectangle_shadow(mode, x, y, width, height, 0, blur);
}
void spark_graphics_apply_elevation(float x, float y, float width, float height, 
                                  uint8_t elevation) {
    if (elevation == 0) return;

    // Get shadow colors
    SDL_Color shadow = spark_graphics_get_shadow_color(elevation);
    SDL_Color ambient = spark_graphics_get_ambient_shadow_color(elevation);

    // Calculate shadow properties based on elevation
    float shadow_blur = elevation * 0.5f;
    float shadow_spread = elevation * 0.25f;
    float ambient_blur = elevation * 1.0f;
    float ambient_spread = elevation * 0.5f;

    // Draw ambient shadow first (wider, softer)
    spark_graphics_set_color_with_alpha(0, 0, 0, ambient.a / 255.0f);
    spark_graphics_rectangle_shadow("fill", 
        x - ambient_spread, 
        y - ambient_spread,
        width + (ambient_spread * 2),
        height + (ambient_spread * 2),
        ambient_blur);

    // Draw main shadow
    spark_graphics_set_color_with_alpha(0, 0, 0, shadow.a / 255.0f);
    spark_graphics_rectangle_shadow("fill",
        x - shadow_spread,
        y + (elevation * 0.5f), // Offset shadow downward
        width + (shadow_spread * 2),
        height + shadow_spread,
        shadow_blur);
}
