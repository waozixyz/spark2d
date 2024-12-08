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

static void disp_flush(lv_disp_drv_t* disp_drv, const lv_area_t* area, lv_color_t* color_p) {
    if (!spark.ui_texture) {
        printf("Error: UI texture is null\n");
        lv_disp_flush_ready(disp_drv);
        return;
    }

    SDL_Rect rect = {
        .x = area->x1,
        .y = area->y1,
        .w = area->x2 - area->x1 + 1,
        .h = area->y2 - area->y1 + 1
    };
    
    int pitch = rect.w * sizeof(lv_color_t);
    if (SDL_UpdateTexture(spark.ui_texture, &rect, color_p, pitch) < 0) {
        printf("Texture update failed: %s\n", SDL_GetError());
    }
    
    lv_disp_flush_ready(disp_drv);
}

static void keypad_read(lv_indev_drv_t* drv, lv_indev_data_t* data) {
    data->key = 0;
    data->state = LV_INDEV_STATE_RELEASED;

    SDL_Event e;
    while(SDL_PollEvent(&e)) {
        switch(e.type) {
            case SDL_MOUSEWHEEL:
                wheel_event = e;
                has_wheel_event = true;
                return;

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

            case SDL_MOUSEBUTTONDOWN:
            case SDL_MOUSEBUTTONUP:
                if (e.button.button == SDL_BUTTON_LEFT) {
                    mouse_pressed = (e.type == SDL_MOUSEBUTTONDOWN);
                }
                break;
        }
    }
}

static void mouse_read(lv_indev_drv_t* drv, lv_indev_data_t* data) {
    SDL_GetMouseState(&mouse_x, &mouse_y);
    data->point.x = mouse_x;
    data->point.y = mouse_y;
    data->state = mouse_pressed ? LV_INDEV_STATE_PRESSED : LV_INDEV_STATE_RELEASED;
}

static void encoder_read(lv_indev_drv_t* drv, lv_indev_data_t* data) {
    if (has_wheel_event) {
        data->enc_diff = -wheel_event.wheel.y;
        has_wheel_event = false;
    } else {
        data->enc_diff = 0;
    }
    data->state = LV_INDEV_STATE_RELEASED;
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

    // Create input group
    lvgl.input_group = lv_group_create();
    lv_group_set_default(lvgl.input_group);

    // Initialize input devices
    static lv_indev_drv_t keypad_drv, mouse_drv, encoder_drv;

    // Keypad
    lv_indev_drv_init(&keypad_drv);
    keypad_drv.type = LV_INDEV_TYPE_KEYPAD;
    keypad_drv.read_cb = keypad_read;
    lvgl.indev_keypad = lv_indev_drv_register(&keypad_drv);
    if (lvgl.indev_keypad) {
        lv_indev_set_group(lvgl.indev_keypad, lvgl.input_group);
    }

    // Mouse
    lv_indev_drv_init(&mouse_drv);
    mouse_drv.type = LV_INDEV_TYPE_POINTER;
    mouse_drv.read_cb = mouse_read;
    lvgl.indev_mouse = lv_indev_drv_register(&mouse_drv);

    // Encoder
    lv_indev_drv_init(&encoder_drv);
    encoder_drv.type = LV_INDEV_TYPE_ENCODER;
    encoder_drv.read_cb = encoder_read;
    lvgl.indev_encoder = lv_indev_drv_register(&encoder_drv);
    if (lvgl.indev_encoder) {
        lv_indev_set_group(lvgl.indev_encoder, lvgl.input_group);
    }

    // Set transparent background
    lv_obj_set_style_bg_opa(lv_scr_act(), LV_OPA_TRANSP, 0);

    lvgl.initialized = true;
    printf("LVGL initialization complete\n");
    return true;
}

void spark_lvgl_update(void) {
    if (!lvgl.initialized) return;

    // Clear with transparency
    SDL_SetRenderDrawColor(spark.renderer, 0, 0, 0, 0);
    SDL_RenderClear(spark.renderer);
    
    // Update LVGL
    lv_timer_handler();
    
    // Render LVGL content
    SDL_RenderCopy(spark.renderer, spark.ui_texture, NULL, NULL);
}

void spark_lvgl_cleanup(void) {
    if (!lvgl.initialized) return;
    
    if (lvgl.input_group) {
        lv_group_del(lvgl.input_group);
    }
    
    free(lvgl.fb1);
    free(lvgl.fb2);
    lvgl.fb1 = NULL;
    lvgl.fb2 = NULL;
    
    lv_deinit();
    lvgl.initialized = false;
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