#ifndef SPARK_GRAPHICS_INTERNAL_H
#define SPARK_GRAPHICS_INTERNAL_H

#include "nanosvg.h"
#include "nanosvgrast.h"

#include "spark_graphics/types.h"


// Font management functions
extern SparkFont* default_font;
void ensure_default_font(void);


// SVG rasterizer
extern NSVGrasterizer* global_rasterizer;

#endif
