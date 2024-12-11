// spark_filesystem.c
#include "spark_filesystem.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>
#include <limits.h>

#define MAX_PATH PATH_MAX

static char working_dir[MAX_PATH] = {0};

bool spark_filesystem_init(void) {
    return getcwd(working_dir, sizeof(working_dir)) != NULL;
}

void spark_filesystem_shutdown(void) {
    // Nothing to clean up
}

bool spark_filesystem_read(const char* filename, char** data, size_t* size) {
    if (!filename || !data || !size) return false;
    
    // If path starts with A:, skip it for actual file operations
    const char* actual_path = filename;
    if (strncmp(filename, "A:", 2) == 0) {
        actual_path = filename + 2;
    }
    
    FILE* file = fopen(actual_path, "rb");
    if (!file) {
        printf("Failed to open file: %s\n", actual_path);
        return false;
    }
    
    fseek(file, 0, SEEK_END);
    *size = ftell(file);
    fseek(file, 0, SEEK_SET);
    
    *data = malloc(*size);
    if (!*data) {
        fclose(file);
        return false;
    }
    
    size_t read = fread(*data, 1, *size, file);
    fclose(file);
    
    return read == *size;
}

bool spark_filesystem_exists(const char* filename) {
    if (!filename) return false;
    
    // If path starts with A:, skip it for actual file operations
    const char* actual_path = filename;
    if (strncmp(filename, "A:", 2) == 0) {
        actual_path = filename + 2;
    }
    
    FILE* file = fopen(actual_path, "rb");
    if (file) {
        fclose(file);
        return true;
    }
    return false;
}
bool spark_filesystem_get_lvgl_path(const char* path, char* out_path, size_t out_size) {
    if (!path || !out_path || out_size == 0) return false;
    
    // Just prefix with "C:" - this matches LVGL's expected format
    int result = snprintf(out_path, out_size, "A:%s", path);
    return result > 0 && (size_t)result < out_size;
}

bool spark_filesystem_is_supported_format(const char* path) {
    if (!path) return false;
    
    const char* ext = strrchr(path, '.');
    if (!ext) return false;
    
    ext++; // Skip the dot
    
    // Convert extension to lowercase for comparison
    char ext_lower[8] = {0};
    size_t i;
    for (i = 0; i < 7 && ext[i]; i++) {
        ext_lower[i] = tolower(ext[i]);
    }
    ext_lower[i] = '\0';
    
    // Check supported formats
    return (strcmp(ext_lower, "png") == 0 ||
            strcmp(ext_lower, "jpg") == 0 ||
            strcmp(ext_lower, "jpeg") == 0 ||
            strcmp(ext_lower, "bmp") == 0);
}