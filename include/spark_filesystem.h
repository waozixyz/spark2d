// spark_filesystem.h
#ifndef SPARK_FILESYSTEM_H
#define SPARK_FILESYSTEM_H

#include <stdbool.h>
#include <stddef.h>

// Initialize the filesystem
bool spark_filesystem_init(void);  // Changed from spark_fs_init

// Cleanup
void spark_filesystem_shutdown(void);  // Changed from spark_fs_shutdown

// Read file into memory
bool spark_filesystem_read(const char* filename, char** data, size_t* size);

// Check if file exists
bool spark_filesystem_exists(const char* filename);

// Get full path for LVGL 
bool spark_filesystem_get_lvgl_path(const char* path, char* out_path, size_t out_size);

// Check if file format is supported
bool spark_filesystem_is_supported_format(const char* path);

#endif