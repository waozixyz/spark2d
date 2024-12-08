#include "spark_event.h"
#include "internal.h"
#include <stdlib.h>
#include <string.h>
#include "lvgl.h"

#define MAX_EVENT_QUEUE_SIZE 1024

static EventSystem event_system = {0};

static bool queue_event(SparkEvent* event) {
    if (event_system.size >= MAX_EVENT_QUEUE_SIZE) {
        return false;
    }

    event_system.events[event_system.tail] = *event;
    event_system.tail = (event_system.tail + 1) % MAX_EVENT_QUEUE_SIZE;
    event_system.size++;
    return true;
}

static void lv_input_cb(lv_event_t* e) {
    lv_event_code_t code = lv_event_get_code(e);
    
    SparkEvent event = {0};
    
    switch(code) {
        case LV_EVENT_PRESSED: {
            event.type = SPARK_EVENT_MOUSEPRESSED;
            SparkMouseEvent* mouse_data = malloc(sizeof(SparkMouseEvent));
            lv_point_t point = {0};
            lv_indev_t* indev = lv_indev_get_act();
            if (indev) {
                lv_indev_get_point(indev, &point);
                mouse_data->x = point.x;
                mouse_data->y = point.y;
            }
            mouse_data->button = 1;
            mouse_data->clicks = 1;
            mouse_data->istouch = true;
            event.data = mouse_data;
            event.data_size = sizeof(SparkMouseEvent);
            break;
        }

        case LV_EVENT_RELEASED: {
            event.type = SPARK_EVENT_MOUSERELEASED;
            SparkMouseEvent* mouse_data = malloc(sizeof(SparkMouseEvent));
            lv_point_t point = {0};
            lv_indev_t* indev = lv_indev_get_act();
            if (indev) {
                lv_indev_get_point(indev, &point);
                mouse_data->x = point.x;
                mouse_data->y = point.y;
            }
            mouse_data->button = 1;
            mouse_data->clicks = 1;
            mouse_data->istouch = true;
            event.data = mouse_data;
            event.data_size = sizeof(SparkMouseEvent);
            break;
        }

        case LV_EVENT_PRESSING: {
            event.type = SPARK_EVENT_MOUSEMOVED;
            SparkMouseMoveEvent* move_data = malloc(sizeof(SparkMouseMoveEvent));
            lv_point_t point = {0};
            lv_indev_t* indev = lv_indev_get_act();
            if (indev) {
                lv_indev_get_point(indev, &point);
                move_data->x = point.x;
                move_data->y = point.y;
            }
            move_data->istouch = true;
            event.data = move_data;
            event.data_size = sizeof(SparkMouseMoveEvent);
            break;
        }

        case LV_EVENT_KEY: {
            event.type = SPARK_EVENT_KEYPRESSED;
            SparkKeyEvent* key_data = malloc(sizeof(SparkKeyEvent));
            lv_indev_t* indev = lv_indev_get_act();
            key_data->key = indev ? lv_indev_get_key(indev) : 0;
            key_data->scancode = 0;
            key_data->repeat = false;
            key_data->mod = 0;
            event.data = key_data;
            event.data_size = sizeof(SparkKeyEvent);
            break;
        }

        default:
            break;
    }

    if (event.type != SPARK_EVENT_NONE) {
        queue_event(&event);
    }
}

void spark_event_init(void) {
    event_system.events = malloc(sizeof(SparkEvent) * MAX_EVENT_QUEUE_SIZE);
    event_system.head = 0;
    event_system.tail = 0;
    event_system.size = 0;
    event_system.handler_count = 0;

    lv_obj_add_event_cb(lv_scr_act(), lv_input_cb, LV_EVENT_ALL, NULL);
}

void spark_event_cleanup(void) {
    if (event_system.events) {
        spark_event_clear();
        free(event_system.events);
        event_system.events = NULL;
    }
}

void spark_event_clear(void) {
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

void spark_event_pump(void) {
    lv_timer_handler();
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
        lv_timer_handler();
        SDL_Delay(1);
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
    
    event_system.handlers[event_system.handler_count++] = (SparkEventHandlerInfo){
        .type = type,
        .handler = handler,
        .active = true
    };
    
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
    for (SparkEventType type = SPARK_EVENT_NONE; type < SPARK_EVENT_CUSTOM_BEGIN; type++) {
        spark_event_add_handler(type, handler);
    }
}

void spark_event_quit(void) {
    SparkEvent quit_event = {
        .type = SPARK_EVENT_QUIT,
        .data = NULL,
        .data_size = 0
    };
    queue_event(&quit_event);
}