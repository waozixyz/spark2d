#ifndef SPARK_GRAPHICS_SHADOWS_H
#define SPARK_GRAPHICS_SHADOWS_H

void spark_graphics_rounded_rectangle_shadow(const char* mode, float x, float y,
                                          float width, float height,
                                          float radius, float blur);
void spark_graphics_rectangle_shadow(const char* mode, float x, float y,
                                   float width, float height, float blur);
void spark_graphics_apply_elevation(float x, float y, float width, float height,
                                  uint8_t elevation);

#endif
