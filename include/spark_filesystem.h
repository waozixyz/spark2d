// spark_filesystem.h
#ifndef SPARK_FILESYSTEM_H
#define SPARK_FILESYSTEM_H

#include <stdbool.h>
#include <stdio.h>

// File info structure
typedef struct SparkFileInfo {
    const char* filename;
    size_t size;
    bool is_directory;
    long long modified_time;
} SparkFileInfo;

// Core functions
bool spark_filesystem_init(void);
void spark_filesystem_shutdown(void);

// Directory and file operations
bool spark_filesystem_create_directory(const char* path);
bool spark_filesystem_exists(const char* path);
bool spark_filesystem_remove(const char* path);
char** spark_filesystem_get_directory_items(const char* path, int* count);
void spark_filesystem_free_directory_items(char** items, int count);

// File I/O
bool spark_filesystem_read(const char* filename, char** data, size_t* size);
bool spark_filesystem_write(const char* filename, const char* data, size_t size);
bool spark_filesystem_append(const char* filename, const char* data, size_t size);

// Path and identity management
void spark_filesystem_set_identity(const char* identity);
const char* spark_filesystem_get_identity(void);
const char* spark_filesystem_get_save_directory(void);
const char* spark_filesystem_get_user_directory(void);

// File information
SparkFileInfo* spark_filesystem_get_info(const char* path);
void spark_filesystem_free_info(SparkFileInfo* info);

#endif // SPARK_FILESYSTEM_H
