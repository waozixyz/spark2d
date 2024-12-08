// spark_lvgl.c
#include "spark_lvgl.h"
#include "internal.h"
#include <stdio.h>
#include <SDL2/SDL.h>

static SparkLVGL lvgl = {0};

// Input state tracking
static bool mouse_pressed = false;
static int mouse_x = 0, mouse_y = 0;
static bool has_wheel_event = false;
static SDL_Event wheel_event;

static SDL_Event current_event;
static bool has_event = false;

static SparkInput input = {0};

static void process_event(SDL_Event* e) {
    switch(e->type) {
        case SDL_MOUSEBUTTONDOWN:
        case SDL_MOUSEBUTTONUP:
            if (e->button.button == SDL_BUTTON_LEFT) {
                mouse_pressed = (e->type == SDL_MOUSEBUTTONDOWN);
                printf("Mouse button %s\n", mouse_pressed ? "pressed" : "released");
            }
            break;
            
        case SDL_MOUSEWHEEL:
            wheel_event = *e;
            has_wheel_event = true;
            break;
    }
}
// Mouse input handler
static void mouse_read(lv_indev_drv_t* drv, lv_indev_data_t* data) {
    // Get current mouse state
    uint32_t buttons = SDL_GetMouseState(&input.mouse_x, &input.mouse_y);
    
    // Get window scale for proper coordinate translation
    float scale_x, scale_y;
    spark_window_get_scale(&scale_x, &scale_y);
    
    // Adjust coordinates based on scale and viewport
    data->point.x = (lv_coord_t)((input.mouse_x - spark.window_state.viewport.x) / scale_x);
    data->point.y = (lv_coord_t)((input.mouse_y - spark.window_state.viewport.y) / scale_y);
    
    // Set pressed state based on left mouse button
    data->state = (buttons & SDL_BUTTON_LMASK) ? LV_INDEV_STATE_PRESSED : LV_INDEV_STATE_RELEASED;
}

// Keyboard input handler
static void keypad_read(lv_indev_drv_t* drv, lv_indev_data_t* data) {
    data->key = 0;
    data->state = LV_INDEV_STATE_RELEASED;

    SDL_Event e;
    if (SDL_PollEvent(&e)) {
        switch (e.type) {
            case SDL_TEXTINPUT:
                data->key = (uint32_t)e.text.text[0];
                data->state = LV_INDEV_STATE_PRESSED;
                return;

            case SDL_KEYDOWN:
            case SDL_KEYUP:
                switch(e.key.keysym.sym) {
                    case SDLK_UP:      data->key = LV_KEY_UP; break;
                    case SDLK_DOWN:    data->key = LV_KEY_DOWN; break;
                    case SDLK_RIGHT:   data->key = LV_KEY_RIGHT; break;
                    case SDLK_LEFT:    data->key = LV_KEY_LEFT; break;
                    case SDLK_ESCAPE:  data->key = LV_KEY_ESC; break;
                    case SDLK_DELETE:  data->key = LV_KEY_DEL; break;
                    case SDLK_BACKSPACE: data->key = LV_KEY_BACKSPACE; break;
                    case SDLK_RETURN:
                    case SDLK_KP_ENTER: data->key = LV_KEY_ENTER; break;
                    case SDLK_TAB:     data->key = LV_KEY_NEXT; break;
                    default:
                        if (e.type == SDL_KEYDOWN && 
                            e.key.keysym.sym >= 32 && 
                            e.key.keysym.sym <= 126) {
                            data->key = e.key.keysym.sym;
                        }
                        break;
                }
                data->state = (e.type == SDL_KEYDOWN) ? 
                             LV_INDEV_STATE_PRESSED : 
                             LV_INDEV_STATE_RELEASED;
                break;

            case SDL_MOUSEWHEEL:
                input.wheel_event = e;
                input.has_wheel_event = true;
                break;
        }
    }
}


static void encoder_read(lv_indev_drv_t* drv, lv_indev_data_t* data) {
    if (input.has_wheel_event) {
        data->enc_diff = -input.wheel_event.wheel.y;
        input.has_wheel_event = false;
    } else {
        data->enc_diff = 0;
    }
    data->state = LV_INDEV_STATE_RELEASED;
}


static void disp_flush(lv_disp_drv_t* disp_drv, const lv_area_t* area, lv_color_t* color_p) {
    if (!spark.ui_texture) {
        lv_disp_flush_ready(disp_drv);
        return;
    }

    SDL_Rect rect = {
        .x = area->x1,
        .y = area->y1,
        .w = area->x2 - area->x1 + 1,
        .h = area->y2 - area->y1 + 1
    };
    
    // Clear the area to transparent first
    void* pixels;
    int pitch;
    SDL_LockTexture(spark.ui_texture, &rect, &pixels, &pitch);
    memset(pixels, 0, rect.h * pitch);
    SDL_UnlockTexture(spark.ui_texture);
    
    int update_pitch = rect.w * sizeof(lv_color_t);
    SDL_UpdateTexture(spark.ui_texture, &rect, color_p, update_pitch);
    
    lv_disp_flush_ready(disp_drv);
}



// Initialize input handling
bool spark_input_init(void) {
    // Create and set default input group
    input.input_group = lv_group_create();
    if (!input.input_group) {
        fprintf(stderr, "Failed to create input group\n");
        return false;
    }
    lv_group_set_default(input.input_group);

    // Initialize mouse driver
    static lv_indev_drv_t mouse_drv;
    lv_indev_drv_init(&mouse_drv);
    mouse_drv.type = LV_INDEV_TYPE_POINTER;
    mouse_drv.read_cb = mouse_read;
    input.indev_mouse = lv_indev_drv_register(&mouse_drv);
    if (!input.indev_mouse) {
        fprintf(stderr, "Failed to register mouse driver\n");
        return false;
    }

    // Initialize keyboard driver
    static lv_indev_drv_t keypad_drv;
    lv_indev_drv_init(&keypad_drv);
    keypad_drv.type = LV_INDEV_TYPE_KEYPAD;
    keypad_drv.read_cb = keypad_read;
    input.indev_keypad = lv_indev_drv_register(&keypad_drv);
    if (input.indev_keypad) {
        lv_indev_set_group(input.indev_keypad, input.input_group);
    }

    // Initialize encoder/wheel driver
    static lv_indev_drv_t encoder_drv;
    lv_indev_drv_init(&encoder_drv);
    encoder_drv.type = LV_INDEV_TYPE_ENCODER;
    encoder_drv.read_cb = encoder_read;
    input.indev_encoder = lv_indev_drv_register(&encoder_drv);
    if (input.indev_encoder) {
        lv_indev_set_group(input.indev_encoder, input.input_group);
    }

    // Enable text input
    SDL_StartTextInput();

    return true;
}

bool spark_lvgl_init(void) {
    if (lvgl.initialized) return true;

    printf("Initializing LVGL\n");
    lv_init();

    // Initialize frame buffers
    int buffer_size = spark.window_state.base_width * spark.window_state.base_height;
    lvgl.fb1 = malloc(buffer_size * sizeof(lv_color_t));
    lvgl.fb2 = malloc(buffer_size * sizeof(lv_color_t));
    
    if (!lvgl.fb1 || !lvgl.fb2) {
        printf("Failed to allocate frame buffers\n");
        spark_lvgl_cleanup();
        return false;
    }

    // Clear frame buffers to transparent
    memset(lvgl.fb1, 0, buffer_size * sizeof(lv_color_t));
    memset(lvgl.fb2, 0, buffer_size * sizeof(lv_color_t));

    // Initialize display buffer
    lv_disp_draw_buf_init(&lvgl.draw_buf, lvgl.fb1, lvgl.fb2, buffer_size);
                         
    // Initialize display driver
    lv_disp_drv_init(&lvgl.disp_drv);
    lvgl.disp_drv.hor_res = spark.window_state.base_width;
    lvgl.disp_drv.ver_res = spark.window_state.base_height;
    lvgl.disp_drv.flush_cb = disp_flush;
    lvgl.disp_drv.draw_buf = &lvgl.draw_buf;
    lvgl.disp_drv.full_refresh = 1;
    
    lvgl.display = lv_disp_drv_register(&lvgl.disp_drv);
    if (!lvgl.display) {
        printf("Failed to register display driver\n");
        spark_lvgl_cleanup();
        return false;
    }

    // Initialize input system
    if (!spark_input_init()) {
        printf("Failed to initialize input system\n");
        spark_lvgl_cleanup();
        return false;
    }

    // Create UI texture with transparency
    spark.ui_texture = SDL_CreateTexture(spark.renderer,
        SDL_PIXELFORMAT_RGBA8888,
        SDL_TEXTUREACCESS_STREAMING,
        spark.window_state.base_width,
        spark.window_state.base_height);

    SDL_SetTextureBlendMode(spark.ui_texture, SDL_BLENDMODE_BLEND);

    // Set transparent background for LVGL screen
    lv_obj_t* screen = lv_scr_act();
    lv_obj_set_style_bg_opa(screen, LV_OPA_0, 0);
    lv_obj_set_style_bg_color(screen, lv_color_black(), 0);

    lvgl.initialized = true;
    printf("LVGL initialization complete\n");
    return true;
}

void spark_lvgl_update(void) {
    if (!lvgl.initialized) return;
    
    // Update LVGL timers and input
    lv_timer_handler();
    
    // Make sure to use proper blending
    SDL_SetTextureBlendMode(spark.ui_texture, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawBlendMode(spark.renderer, SDL_BLENDMODE_BLEND);
    
    // Render the UI with transparency
    SDL_RenderCopy(spark.renderer, spark.ui_texture, NULL, &spark.window_state.viewport);
}

// Coordinate transformation functions
void spark_lvgl_get_scale(float* scale_x, float* scale_y) {
    spark_window_get_scale(scale_x, scale_y);
}

float spark_lvgl_scale_x(float x) {
    float scale_x, scale_y;
    spark_lvgl_get_scale(&scale_x, &scale_y);
    return x * scale_x;
}

float spark_lvgl_scale_y(float y) {
    float scale_x, scale_y;
    spark_lvgl_get_scale(&scale_x, &scale_y);
    return y * scale_y;
}

void spark_lvgl_screen_to_ui(float screen_x, float screen_y, float* ui_x, float* ui_y) {
    float scale_x, scale_y;
    spark_lvgl_get_scale(&scale_x, &scale_y);
    *ui_x = screen_x / scale_x;
    *ui_y = screen_y / scale_y;
}

void spark_lvgl_get_mouse_position(float* x, float* y) {
    int mx, my;
    SDL_GetMouseState(&mx, &my);
    spark_lvgl_screen_to_ui(mx - spark.window_state.viewport.x,
                           my - spark.window_state.viewport.y,
                           x, y);
}

void spark_input_add_obj(lv_obj_t* obj) {
    if (input.input_group && obj) {
        lv_group_add_obj(input.input_group, obj);
    }
}

void spark_input_cleanup(void) {
    SDL_StopTextInput();
    
    if (input.input_group) {
        lv_group_del(input.input_group);
        input.input_group = NULL;
    }

    // LVGL will clean up the input devices when lv_deinit() is called
    input.indev_mouse = NULL;
    input.indev_keypad = NULL;
    input.indev_encoder = NULL;
}


void spark_lvgl_cleanup(void) {
    if (!lvgl.initialized) return;
    
    // Cleanup input devices
    spark_input_cleanup();
    
    // Cleanup frame buffers
    if (lvgl.fb1) {
        free(lvgl.fb1);
        lvgl.fb1 = NULL;
    }
    
    if (lvgl.fb2) {
        free(lvgl.fb2);
        lvgl.fb2 = NULL;
    }
    
    // Deinitialize LVGL
    lv_deinit();
    
    lvgl.initialized = false;
}