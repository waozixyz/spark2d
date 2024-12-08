#ifndef SPARK_EVENT_H
#define SPARK_EVENT_H

#include <stddef.h>
#include <stdbool.h>
#define MAX_CUSTOM_HANDLERS 64

// First, declare all forward declarations
typedef struct SparkEvent SparkEvent;
typedef void (*SparkEventHandler)(SparkEvent* event);

// Then define all enums
typedef enum {
    SPARK_EVENT_NONE,
    SPARK_EVENT_QUIT,
    SPARK_EVENT_FOCUS,
    SPARK_EVENT_VISIBLE,
    SPARK_EVENT_KEYPRESSED,
    SPARK_EVENT_KEYRELEASED,
    SPARK_EVENT_MOUSEPRESSED,
    SPARK_EVENT_MOUSERELEASED,
    SPARK_EVENT_MOUSEMOVED,
    SPARK_EVENT_MOUSEWHEEL,
    SPARK_EVENT_RESIZE,
    SPARK_EVENT_CUSTOM_BEGIN = 1000
} SparkEventType;

// Then define all event data structures
typedef struct {
    int key;
    int scancode;
    bool repeat;
    int mod;
} SparkKeyEvent;

typedef struct {
    int x;
    int y;
    int button;
    int clicks;
    bool istouch;
} SparkMouseEvent;

typedef struct {
    int x;
    int y;
    float dx;
    float dy;
    bool istouch;
} SparkMouseMoveEvent;

typedef struct {
    float x;
    float y;
} SparkWheelEvent;

typedef struct {
    int width;
    int height;
} SparkResizeEvent;

// Then define the main event structure
struct SparkEvent {
    SparkEventType type;
    void* data;
    size_t data_size;
};

typedef struct {
    SparkEventType type;
    SparkEventHandler handler;
    bool active;
} SparkEventHandlerInfo;

typedef struct {
    SparkEvent* events;
    int head;
    int tail;
    int size;
    SparkEventHandlerInfo handlers[MAX_CUSTOM_HANDLERS];  // Use SparkEventHandlerInfo type
    int handler_count;
} EventSystem;
// Function declarations
void spark_event_init(void);
void spark_event_cleanup(void);
void spark_event_clear(void);
bool spark_event_poll(SparkEvent* out_event);
bool spark_event_wait(SparkEvent* out_event);
bool spark_event_push(SparkEventType type, void* data, size_t data_size);
void spark_event_quit(void);
bool spark_event_add_handler(SparkEventType type, SparkEventHandler handler);
void spark_event_remove_handler(SparkEventType type);
void spark_set_event_handler(SparkEventHandler handler);

#endif
