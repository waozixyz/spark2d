// spark_event.c
#include "spark_event.h"
#include "internal.h"
#include <stdlib.h>
#include <string.h>

#define MAX_EVENT_QUEUE_SIZE 1024
#define MAX_CUSTOM_HANDLERS 64

static EventSystem event_system = {0};

void spark_event_init(void) {
    event_system.events = malloc(sizeof(SparkEvent) * MAX_EVENT_QUEUE_SIZE);
    event_system.head = 0;
    event_system.tail = 0;
    event_system.size = 0;
    event_system.handler_count = 0;
}

void spark_event_cleanup(void) {
    if (event_system.events) {
        // Clean up any remaining event data
        spark_event_clear();
        free(event_system.events);
    }
}

void spark_event_clear(void) {
    // Free any custom data in the queue
    while (event_system.size > 0) {
        SparkEvent* event = &event_system.events[event_system.head];
        if (event->data) {
            free(event->data);
        }
        event_system.head = (event_system.head + 1) % MAX_EVENT_QUEUE_SIZE;
        event_system.size--;
    }
    event_system.head = event_system.tail = 0;
}

static bool queue_event(SparkEvent* event) {
    if (event_system.size >= MAX_EVENT_QUEUE_SIZE) {
        return false;
    }

    event_system.events[event_system.tail] = *event;
    event_system.tail = (event_system.tail + 1) % MAX_EVENT_QUEUE_SIZE;
    event_system.size++;
    return true;
}

void spark_event_pump(void) {
    SDL_Event sdl_event;
    while (SDL_PollEvent(&sdl_event)) {
        SparkEvent event = {0};
        
        switch (sdl_event.type) {
            case SDL_EVENT_QUIT:
                event.type = SPARK_EVENT_QUIT;
                break;

            case SDL_EVENT_KEY_DOWN: {
                event.type = SPARK_EVENT_KEYPRESSED;
                SparkKeyEvent key_data = {
                    .key = sdl_event.key.key,
                    .scancode = sdl_event.key.scancode,
                    .repeat = sdl_event.key.repeat != 0,
                    .mod = sdl_event.key.mod
                };
                event.data = malloc(sizeof(SparkKeyEvent));
                memcpy(event.data, &key_data, sizeof(SparkKeyEvent));
                event.data_size = sizeof(SparkKeyEvent);
                break;
            }

            case SDL_EVENT_KEY_UP: {
                event.type = SPARK_EVENT_KEYRELEASED;
                SparkKeyEvent key_data = {
                    .key = sdl_event.key.key,
                    .scancode = sdl_event.key.scancode,
                    .repeat = false,
                    .mod = sdl_event.key.mod
                };
                event.data = malloc(sizeof(SparkKeyEvent));
                memcpy(event.data, &key_data, sizeof(SparkKeyEvent));
                event.data_size = sizeof(SparkKeyEvent);
                break;
            }

            case SDL_EVENT_MOUSE_BUTTON_DOWN:
            case SDL_EVENT_MOUSE_BUTTON_UP: {
                event.type = (sdl_event.type == SDL_EVENT_MOUSE_BUTTON_DOWN) ? 
                            SPARK_EVENT_MOUSEPRESSED : SPARK_EVENT_MOUSERELEASED;
                SparkMouseEvent mouse_data = {
                    .x = sdl_event.button.x,
                    .y = sdl_event.button.y,
                    .button = sdl_event.button.button,
                    .clicks = sdl_event.button.clicks,
                    .istouch = false
                };
                event.data = malloc(sizeof(SparkMouseEvent));
                memcpy(event.data, &mouse_data, sizeof(SparkMouseEvent));
                event.data_size = sizeof(SparkMouseEvent);
                break;
            }

            case SDL_EVENT_MOUSE_MOTION: {
                event.type = SPARK_EVENT_MOUSEMOVED;
                SparkMouseMoveEvent move_data = {
                    .x = sdl_event.motion.x,
                    .y = sdl_event.motion.y,
                    .dx = sdl_event.motion.xrel,
                    .dy = sdl_event.motion.yrel,
                    .istouch = false
                };
                event.data = malloc(sizeof(SparkMouseMoveEvent));
                memcpy(event.data, &move_data, sizeof(SparkMouseMoveEvent));
                event.data_size = sizeof(SparkMouseMoveEvent);
                break;
            }

            case SDL_EVENT_WINDOW_RESIZED: {
                event.type = SPARK_EVENT_RESIZE;
                SparkResizeEvent resize_data = {
                    .width = sdl_event.window.data1,
                    .height = sdl_event.window.data2
                };
                event.data = malloc(sizeof(SparkResizeEvent));
                memcpy(event.data, &resize_data, sizeof(SparkResizeEvent));
                event.data_size = sizeof(SparkResizeEvent);
                break;
            }

            case SDL_EVENT_WINDOW_FOCUS_GAINED:
            case SDL_EVENT_WINDOW_FOCUS_LOST: {
                event.type = SPARK_EVENT_FOCUS;
                bool focus_data = (sdl_event.type == SDL_EVENT_WINDOW_FOCUS_GAINED);
                event.data = malloc(sizeof(bool));
                memcpy(event.data, &focus_data, sizeof(bool));
                event.data_size = sizeof(bool);
                break;
            }

            case SDL_EVENT_WINDOW_MINIMIZED:
            case SDL_EVENT_WINDOW_RESTORED:
            case SDL_EVENT_WINDOW_MAXIMIZED: {
                event.type = SPARK_EVENT_VISIBLE;
                bool visible_data = (sdl_event.type != SDL_EVENT_WINDOW_MINIMIZED);
                event.data = malloc(sizeof(bool));
                memcpy(event.data, &visible_data, sizeof(bool));
                event.data_size = sizeof(bool);
                break;
            }

            case SDL_EVENT_MOUSE_WHEEL: {
                event.type = SPARK_EVENT_MOUSEWHEEL;
                SparkWheelEvent wheel_data = {
                    .x = sdl_event.wheel.x,
                    .y = sdl_event.wheel.y
                };
                event.data = malloc(sizeof(SparkWheelEvent));
                memcpy(event.data, &wheel_data, sizeof(SparkWheelEvent));
                event.data_size = sizeof(SparkWheelEvent);
                break;
            }
            
            default:
                break;
        }

        if (event.type != SPARK_EVENT_NONE) {
            queue_event(&event);
        }
    }
}

bool spark_event_poll(SparkEvent* out_event) {
    if (event_system.size == 0) {
        return false;
    }

    *out_event = event_system.events[event_system.head];
    event_system.head = (event_system.head + 1) % MAX_EVENT_QUEUE_SIZE;
    event_system.size--;
    return true;
}

bool spark_event_wait(SparkEvent* out_event) {
    while (event_system.size == 0) {
        SDL_Event sdl_event;
        if (SDL_WaitEvent(&sdl_event)) {
            SparkEvent event = {0};
            // Convert SDL event
            queue_event(&event);
        }
    }
    return spark_event_poll(out_event);
}

bool spark_event_push(SparkEventType type, void* data, size_t data_size) {
    SparkEvent event = {
        .type = type,
        .data = NULL,
        .data_size = data_size
    };
    
    if (data && data_size > 0) {
        event.data = malloc(data_size);
        if (!event.data) return false;
        memcpy(event.data, data, data_size);
    }
    
    return queue_event(&event);
}
bool spark_event_add_handler(SparkEventType type, SparkEventHandler handler) {
    if (event_system.handler_count >= MAX_CUSTOM_HANDLERS) {
        return false;
    }
    
    SparkEventHandlerInfo new_handler = {
        .type = type,
        .handler = handler,
        .active = true
    };
    
    event_system.handlers[event_system.handler_count++] = new_handler;
    return true;
}


void spark_event_remove_handler(SparkEventType type) {
    for (int i = 0; i < event_system.handler_count; i++) {
        if (event_system.handlers[i].type == type) {
            event_system.handlers[i].active = false;
        }
    }
}

void spark_set_event_handler(SparkEventHandler handler) {
    // Add as a global event handler for all event types
    for (SparkEventType type = SPARK_EVENT_NONE; type < SPARK_EVENT_CUSTOM_BEGIN; type++) {
        spark_event_add_handler(type, handler);
    }
}
