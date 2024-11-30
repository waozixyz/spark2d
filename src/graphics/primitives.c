#include "spark_graphics/primitives.h"
#include "../internal.h"
#include <SDL3/SDL.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>

#define PI 3.14159265358979323846f
#define DEG_TO_RAD(x) ((x) * PI / 180.0f)

static SDL_Vertex* create_vertex_array(const float* positions, int vertex_count) {
    SDL_Vertex* vertices = malloc(vertex_count * sizeof(SDL_Vertex));
    if (!vertices) return NULL;
    
    uint8_t r, g, b, a;
    SDL_GetRenderDrawColor(spark.renderer, &r, &g, &b, &a);
    
    SDL_FColor color = {
        r / 255.0f,
        g / 255.0f,
        b / 255.0f,
        a / 255.0f
    };
    
    for (int i = 0; i < vertex_count; i++) {
        vertices[i].position.x = positions[i * 2];
        vertices[i].position.y = positions[i * 2 + 1];
        vertices[i].color = color;
        vertices[i].tex_coord.x = 0;
        vertices[i].tex_coord.y = 0;
    }
    
    return vertices;
}

void spark_graphics_rectangle(const char* mode, float x, float y, float w, float h) {
    SDL_FRect rect = {x, y, w, h};
    if (strcmp(mode, "fill") == 0) {
        SDL_RenderFillRect(spark.renderer, &rect);
    } else if (strcmp(mode, "line") == 0) {
        SDL_RenderRect(spark.renderer, &rect);
    }
}

void spark_graphics_circle(const char* mode, float x, float y, float radius) {
    const int segments = (int)(10 * sqrtf(radius));
    float* pos = malloc(segments * 2 * sizeof(float));
    
    for (int i = 0; i < segments; i++) {
        float angle = (float)i / (float)segments * 2.0f * PI;
        pos[i * 2] = x + cosf(angle) * radius;
        pos[i * 2 + 1] = y + sinf(angle) * radius;
    }
    
    if (strcmp(mode, "line") == 0) {
        for (int i = 0; i < segments - 1; i++) {
            SDL_RenderLine(spark.renderer,
                          pos[i * 2], pos[i * 2 + 1],
                          pos[(i + 1) * 2], pos[(i + 1) * 2 + 1]);
        }
        SDL_RenderLine(spark.renderer,
                      pos[(segments - 1) * 2], pos[(segments - 1) * 2 + 1],
                      pos[0], pos[1]);
    } else if (strcmp(mode, "fill") == 0) {
        SDL_Vertex* vertices = malloc((segments + 2) * sizeof(SDL_Vertex));
        if (!vertices) {
            free(pos);
            return;
        }
        
        uint8_t r, g, b, a;
        SDL_GetRenderDrawColor(spark.renderer, &r, &g, &b, &a);
        SDL_FColor color = {
            r / 255.0f,
            g / 255.0f,
            b / 255.0f,
            a / 255.0f
        };
        
        // Center vertex
        vertices[0].position.x = x;
        vertices[0].position.y = y;
        vertices[0].color = color;
        vertices[0].tex_coord.x = 0;
        vertices[0].tex_coord.y = 0;
        
        // Circle vertices
        for (int i = 0; i < segments; i++) {
            vertices[i + 1].position.x = pos[i * 2];
            vertices[i + 1].position.y = pos[i * 2 + 1];
            vertices[i + 1].color = color;
            vertices[i + 1].tex_coord.x = 0;
            vertices[i + 1].tex_coord.y = 0;
        }
        
        // Close the circle
        vertices[segments + 1] = vertices[1];
        
        SDL_RenderGeometry(spark.renderer, NULL, vertices, segments + 2, NULL, 0);
        
        free(vertices);
    }
    
    free(pos);
}

void spark_graphics_arc(const char* mode, float x, float y, float radius, 
                       float start_angle, float end_angle) {
    float start_rad = DEG_TO_RAD(start_angle);
    float end_rad = DEG_TO_RAD(end_angle);
    
    float arc_length = fabsf(end_rad - start_rad);
    int segments = (int)(10 * sqrtf(radius) * (arc_length / (2.0f * PI)));
    if (segments < 1) segments = 1;
    
    float* pos = malloc((segments + 1) * 2 * sizeof(float));
    
    for (int i = 0; i <= segments; i++) {
        float angle = start_rad + (end_rad - start_rad) * ((float)i / segments);
        pos[i * 2] = x + cosf(angle) * radius;
        pos[i * 2 + 1] = y + sinf(angle) * radius;
    }
    
    if (strcmp(mode, "line") == 0) {
        for (int i = 0; i < segments; i++) {
            SDL_RenderLine(spark.renderer,
                          pos[i * 2], pos[i * 2 + 1],
                          pos[(i + 1) * 2], pos[(i + 1) * 2 + 1]);
        }
    } else if (strcmp(mode, "fill") == 0) {
        for (int i = 0; i < segments; i++) {
            float tri_pos[] = {
                x, y,
                pos[i * 2], pos[i * 2 + 1],
                pos[(i + 1) * 2], pos[(i + 1) * 2 + 1]
            };
            SDL_Vertex* vertices = create_vertex_array(tri_pos, 3);
            if (vertices) {
                SDL_RenderGeometry(spark.renderer, NULL, vertices, 3, NULL, 0);
                free(vertices);
            }
        }
    }
    
    free(pos);
}

void spark_graphics_line(float x1, float y1, float x2, float y2) {
    SDL_RenderLine(spark.renderer, x1, y1, x2, y2);
}
void spark_graphics_rounded_rectangle(const char* mode, float x, float y, float w, float h, float radius) {
    // Clamp radius to half the minimum dimension to prevent overlapping
    radius = fminf(radius, fminf(w/2, h/2));
    
    if (strcmp(mode, "fill") == 0) {
        // Draw the main rectangle in the middle
        spark_graphics_rectangle("fill", x + radius, y, w - 2 * radius, h);
        
        // Draw the rectangles on the sides
        spark_graphics_rectangle("fill", x, y + radius, w, h - 2 * radius);
        
        // Draw the four corner circles last
        spark_graphics_circle("fill", x + radius, y + radius, radius);                  // Top-left
        spark_graphics_circle("fill", x + w - radius, y + radius, radius);             // Top-right
        spark_graphics_circle("fill", x + radius, y + h - radius, radius);             // Bottom-left
        spark_graphics_circle("fill", x + w - radius, y + h - radius, radius);         // Bottom-right
    } 
    else if (strcmp(mode, "line") == 0) {
        // Draw top and bottom lines
        spark_graphics_line(x + radius, y, x + w - radius, y);
        spark_graphics_line(x + radius, y + h, x + w - radius, y + h);
        
        // Draw left and right lines
        spark_graphics_line(x, y + radius, x, y + h - radius);
        spark_graphics_line(x + w, y + radius, x + w, y + h - radius);
        
        // Draw the four corner arcs
        spark_graphics_arc("line", x + radius, y + radius, radius, 180, 270);          // Top-left
        spark_graphics_arc("line", x + w - radius, y + radius, radius, 270, 360);      // Top-right
        spark_graphics_arc("line", x + radius, y + h - radius, radius, 90, 180);       // Bottom-left
        spark_graphics_arc("line", x + w - radius, y + h - radius, radius, 0, 90);     // Bottom-right
    }
}

void spark_graphics_polygon(const char* mode, const float* vertices, int count) {
    if (count < 3) return;
    
    if (strcmp(mode, "line") == 0) {
        for (int i = 0; i < count - 1; i++) {
            SDL_RenderLine(spark.renderer,
                          vertices[i * 2], vertices[i * 2 + 1],
                          vertices[(i + 1) * 2], vertices[(i + 1) * 2 + 1]);
        }
        SDL_RenderLine(spark.renderer,
                      vertices[(count - 1) * 2], vertices[(count - 1) * 2 + 1],
                      vertices[0], vertices[1]);
    } else if (strcmp(mode, "fill") == 0) {
        for (int i = 1; i < count - 1; i++) {
            float tri_pos[] = {
                vertices[0], vertices[1],
                vertices[i * 2], vertices[i * 2 + 1],
                vertices[(i + 1) * 2], vertices[(i + 1) * 2 + 1]
            };
            SDL_Vertex* tri_vertices = create_vertex_array(tri_pos, 3);
            if (tri_vertices) {
                SDL_RenderGeometry(spark.renderer, NULL, tri_vertices, 3, NULL, 0);
                free(tri_vertices);
            }
        }
    }
}

void spark_graphics_ellipse(const char* mode, float x, float y, float radiusx, float radiusy) {
    const int segments = (int)(10 * sqrtf(fmaxf(radiusx, radiusy)));
    float* pos = malloc(segments * 2 * sizeof(float));
    
    for (int i = 0; i < segments; i++) {
        float angle = (float)i / (float)segments * 2.0f * PI;
        pos[i * 2] = x + cosf(angle) * radiusx;
        pos[i * 2 + 1] = y + sinf(angle) * radiusy;
    }
    
    if (strcmp(mode, "line") == 0) {
        for (int i = 0; i < segments - 1; i++) {
            SDL_RenderLine(spark.renderer,
                          pos[i * 2], pos[i * 2 + 1],
                          pos[(i + 1) * 2], pos[(i + 1) * 2 + 1]);
        }
        SDL_RenderLine(spark.renderer,
                      pos[(segments - 1) * 2], pos[(segments - 1) * 2 + 1],
                      pos[0], pos[1]);
    } else if (strcmp(mode, "fill") == 0) {
        for (int i = 1; i < segments - 1; i++) {
            float tri_pos[] = {
                x, y,
                pos[i * 2], pos[i * 2 + 1],
                pos[(i + 1) * 2], pos[(i + 1) * 2 + 1]
            };
            SDL_Vertex* vertices = create_vertex_array(tri_pos, 3);
            if (vertices) {
                SDL_RenderGeometry(spark.renderer, NULL, vertices, 3, NULL, 0);
                free(vertices);
            }
        }
    }
    
    free(pos);
}

void spark_graphics_point(float x, float y) {
    SDL_RenderPoint(spark.renderer, x, y);
}

void spark_graphics_points(const float* points, int count) {
    for (int i = 0; i < count; i++) {
        SDL_RenderPoint(spark.renderer, points[i * 2], points[i * 2 + 1]);
    }
}

void spark_graphics_triangle(const char* mode, float x1, float y1, float x2, float y2, float x3, float y3) {
    if (strcmp(mode, "line") == 0) {
        SDL_RenderLine(spark.renderer, x1, y1, x2, y2);
        SDL_RenderLine(spark.renderer, x2, y2, x3, y3);
        SDL_RenderLine(spark.renderer, x3, y3, x1, y1);
    } else if (strcmp(mode, "fill") == 0) {
        float pos[] = {x1, y1, x2, y2, x3, y3};
        SDL_Vertex* vertices = create_vertex_array(pos, 3);
        if (vertices) {
            SDL_RenderGeometry(spark.renderer, NULL, vertices, 3, NULL, 0);
            free(vertices);
        }
    }
}

void spark_graphics_quad(const char* mode, float x1, float y1, float x2, float y2, 
                        float x3, float y3, float x4, float y4) {
    if (strcmp(mode, "line") == 0) {
        SDL_RenderLine(spark.renderer, x1, y1, x2, y2);
        SDL_RenderLine(spark.renderer, x2, y2, x3, y3);
        SDL_RenderLine(spark.renderer, x3, y3, x4, y4);
        SDL_RenderLine(spark.renderer, x4, y4, x1, y1);
    } else if (strcmp(mode, "fill") == 0) {
        float tri1_pos[] = {x1, y1, x2, y2, x3, y3};
        float tri2_pos[] = {x1, y1, x3, y3, x4, y4};
        
        SDL_Vertex* vertices1 = create_vertex_array(tri1_pos, 3);
        SDL_Vertex* vertices2 = create_vertex_array(tri2_pos, 3);
        
        if (vertices1 && vertices2) {
            SDL_RenderGeometry(spark.renderer, NULL, vertices1, 3, NULL, 0);
            SDL_RenderGeometry(spark.renderer, NULL, vertices2, 3, NULL, 0);
        }
        
        free(vertices1);
        free(vertices2);
    }
}
