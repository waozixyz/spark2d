#include "spark_filesystem.h"
#include <SDL3/SDL.h>
#include <sys/stat.h>
#include <dirent.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <limits.h>

#define SPARK_FS_MAX_PATH PATH_MAX

typedef struct {
    char* identity;            // Current identity for save directory
    char* save_directory;      // Full path to save directory
    char* appdata_directory;   // Full path to appdata directory
    char* user_directory;      // Full path to user directory
    char* source_directory;    // Full path to source/working directory
} SparkFileSystem;

static SparkFileSystem fs = {0};

// Helper functions
static char* create_string_copy(const char* str) {
    return str ? strdup(str) : NULL;
}

static void cleanup_path(char* path) {
    if (!path) return;
    
    for (char* p = path; *p; p++) {
        if (*p == '\\') *p = '/';
    }
    
    size_t len = strlen(path);
    while (len > 0 && path[len - 1] == '/') {
        path[--len] = '\0';
    }
}

static bool is_path_absolute(const char* path) {
    #ifdef _WIN32
    return path && (strlen(path) > 2 && path[1] == ':');
    #else
    return path && path[0] == '/';
    #endif
}

bool spark_filesystem_init(void) {
    // Get current working directory as source
    char temp[SPARK_FS_MAX_PATH];
    if (getcwd(temp, sizeof(temp))) {
        fs.source_directory = create_string_copy(temp);
    }
    
    // Get user directory
    fs.user_directory = SDL_GetBasePath();
    
    // Set default identity
    spark_filesystem_set_identity("default");
    
    return true;
}

void spark_filesystem_shutdown(void) {
    free(fs.identity);
    free(fs.save_directory);
    free(fs.appdata_directory);
    free(fs.user_directory);
    free(fs.source_directory);
    fs = (SparkFileSystem){0};
}

bool spark_filesystem_create_directory(const char* path) {
    if (!path) return false;
    
    char full_path[SPARK_FS_MAX_PATH];
    
    if (is_path_absolute(path)) {
        strncpy(full_path, path, SPARK_FS_MAX_PATH - 1);
    } else {
        snprintf(full_path, SPARK_FS_MAX_PATH, "%s/%s", 
                fs.source_directory, path);
    }
    
    cleanup_path(full_path);
    
    #ifdef _WIN32
    return _mkdir(full_path) == 0;
    #else
    return mkdir(full_path, 0777) == 0;
    #endif
}

bool spark_filesystem_exists(const char* path) {
    if (!path) return false;
    
    char full_path[SPARK_FS_MAX_PATH];
    if (is_path_absolute(path)) {
        strncpy(full_path, path, SPARK_FS_MAX_PATH - 1);
    } else {
        snprintf(full_path, SPARK_FS_MAX_PATH, "%s/%s", 
                fs.source_directory, path);
    }
    
    cleanup_path(full_path);
    
    struct stat buffer;
    return stat(full_path, &buffer) == 0;
}

bool spark_filesystem_remove(const char* path) {
    if (!path) return false;
    
    char full_path[SPARK_FS_MAX_PATH];
    if (is_path_absolute(path)) {
        strncpy(full_path, path, SPARK_FS_MAX_PATH - 1);
    } else {
        snprintf(full_path, SPARK_FS_MAX_PATH, "%s/%s", 
                fs.source_directory, path);
    }
    
    cleanup_path(full_path);
    return remove(full_path) == 0;
}

char** spark_filesystem_get_directory_items(const char* path, int* count) {
    if (!path || !count) return NULL;
    *count = 0;
    
    char full_path[SPARK_FS_MAX_PATH];
    if (is_path_absolute(path)) {
        strncpy(full_path, path, SPARK_FS_MAX_PATH - 1);
    } else {
        snprintf(full_path, SPARK_FS_MAX_PATH, "%s/%s", 
                fs.source_directory, path);
    }
    
    cleanup_path(full_path);
    
    DIR* dir = opendir(full_path);
    if (!dir) return NULL;
    
    struct dirent* entry;
    while ((entry = readdir(dir))) {
        if (entry->d_name[0] != '.') {
            (*count)++;
        }
    }
    
    if (*count == 0) {
        closedir(dir);
        return NULL;
    }
    
    char** items = malloc(*count * sizeof(char*));
    if (!items) {
        closedir(dir);
        *count = 0;
        return NULL;
    }
    
    rewinddir(dir);
    int index = 0;
    while ((entry = readdir(dir)) && index < *count) {
        if (entry->d_name[0] != '.') {
            items[index] = create_string_copy(entry->d_name);
            if (!items[index]) {
                for (int i = 0; i < index; i++) {
                    free(items[i]);
                }
                free(items);
                closedir(dir);
                *count = 0;
                return NULL;
            }
            index++;
        }
    }
    
    closedir(dir);
    return items;
}

void spark_filesystem_free_directory_items(char** items, int count) {
    if (!items) return;
    for (int i = 0; i < count; i++) {
        free(items[i]);
    }
    free(items);
}

bool spark_filesystem_read(const char* filename, char** data, size_t* size) {
    if (!filename || !data || !size) return false;
    
    char full_path[SPARK_FS_MAX_PATH];
    if (is_path_absolute(filename)) {
        strncpy(full_path, filename, SPARK_FS_MAX_PATH - 1);
    } else {
        snprintf(full_path, SPARK_FS_MAX_PATH, "%s/%s", 
                fs.source_directory, filename);
    }
    
    cleanup_path(full_path);
    
    FILE* file = fopen(full_path, "rb");
    if (!file) return false;
    
    fseek(file, 0, SEEK_END);
    *size = ftell(file);
    fseek(file, 0, SEEK_SET);
    
    *data = malloc(*size + 1);
    if (!*data) {
        fclose(file);
        return false;
    }
    
    size_t read_size = fread(*data, 1, *size, file);
    (*data)[*size] = '\0';
    fclose(file);
    
    if (read_size != *size) {
        free(*data);
        *data = NULL;
        *size = 0;
        return false;
    }
    
    return true;
}

bool spark_filesystem_write(const char* filename, const char* data, size_t size) {
    if (!filename || !data) return false;
    
    char full_path[SPARK_FS_MAX_PATH];
    if (is_path_absolute(filename)) {
        strncpy(full_path, filename, SPARK_FS_MAX_PATH - 1);
    } else {
        snprintf(full_path, SPARK_FS_MAX_PATH, "%s/%s", 
                fs.source_directory, filename);
    }
    
    cleanup_path(full_path);
    
    FILE* file = fopen(full_path, "wb");
    if (!file) return false;
    
    size_t written = fwrite(data, 1, size, file);
    fclose(file);
    
    return written == size;
}

bool spark_filesystem_append(const char* filename, const char* data, size_t size) {
    if (!filename || !data) return false;
    
    char full_path[SPARK_FS_MAX_PATH];
    if (is_path_absolute(filename)) {
        strncpy(full_path, filename, SPARK_FS_MAX_PATH - 1);
    } else {
        snprintf(full_path, SPARK_FS_MAX_PATH, "%s/%s", 
                fs.source_directory, filename);
    }
    
    cleanup_path(full_path);
    
    FILE* file = fopen(full_path, "ab");
    if (!file) return false;
    
    size_t written = fwrite(data, 1, size, file);
    fclose(file);
    
    return written == size;
}

void spark_filesystem_set_identity(const char* identity) {
    if (!identity) return;
    
    free(fs.identity);
    fs.identity = create_string_copy(identity);
    
    if (fs.save_directory) {
        free(fs.save_directory);
        fs.save_directory = NULL;
    }
    
    // Create save directory path based on identity
    char* save_path = SDL_GetPrefPath("Spark2D", identity);
    if (save_path) {
        fs.save_directory = create_string_copy(save_path);
        SDL_free(save_path);
    }
}

const char* spark_filesystem_get_identity(void) {
    return fs.identity;
}

const char* spark_filesystem_get_save_directory(void) {
    return fs.save_directory;
}

const char* spark_filesystem_get_user_directory(void) {
    return fs.user_directory;
}

SparkFileInfo* spark_filesystem_get_info(const char* path) {
    if (!path) return NULL;
    
    char full_path[SPARK_FS_MAX_PATH];
    if (is_path_absolute(path)) {
        strncpy(full_path, path, SPARK_FS_MAX_PATH - 1);
    } else {
        snprintf(full_path, SPARK_FS_MAX_PATH, "%s/%s", 
                fs.source_directory, path);
    }
    
    cleanup_path(full_path);
    
    struct stat buffer;
    if (stat(full_path, &buffer) != 0) {
        return NULL;
    }
    
    SparkFileInfo* info = malloc(sizeof(SparkFileInfo));
    if (!info) return NULL;
    
    info->filename = create_string_copy(path);
    if (!info->filename) {
        free(info);
        return NULL;
    }
    
    info->size = buffer.st_size;
    info->is_directory = S_ISDIR(buffer.st_mode);
    info->modified_time = buffer.st_mtime;
    
    return info;
}

void spark_filesystem_free_info(SparkFileInfo* info) {
    if (!info) return;
    free((void*)info->filename);
    free(info);
}
