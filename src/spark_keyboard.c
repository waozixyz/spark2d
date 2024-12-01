// spark_keyboard.c
#include "spark_keyboard.h"
#include "internal.h"
#include <SDL3/SDL.h>

static struct {
    bool key_repeat_enabled;
    bool text_input_enabled;
    int key_repeat_delay;
    int key_repeat_interval;
} keyboard_state = {0};

// We only need one table now
static SDL_Scancode spark_to_sdl_scancode[SPARK_SCANCODE_COUNT] = {0};

static void init_scancode_mappings(void) {
    // Initialize all to unknown first
    for (int i = 0; i < SPARK_SCANCODE_COUNT; i++) {
        spark_to_sdl_scancode[i] = SDL_SCANCODE_UNKNOWN;
    }

    // Define all mappings in single array
    struct ScancodeMapping {
        SparkScancode spark;
        SDL_Scancode sdl;
    };

    static const struct ScancodeMapping mappings[] = {
        {SPARK_SCANCODE_A, SDL_SCANCODE_A},
        {SPARK_SCANCODE_B, SDL_SCANCODE_B},
        {SPARK_SCANCODE_C, SDL_SCANCODE_C},
        {SPARK_SCANCODE_D, SDL_SCANCODE_D},
        {SPARK_SCANCODE_E, SDL_SCANCODE_E},
        {SPARK_SCANCODE_F, SDL_SCANCODE_F},
        {SPARK_SCANCODE_G, SDL_SCANCODE_G},
        {SPARK_SCANCODE_H, SDL_SCANCODE_H},
        {SPARK_SCANCODE_I, SDL_SCANCODE_I},
        {SPARK_SCANCODE_J, SDL_SCANCODE_J},
        {SPARK_SCANCODE_K, SDL_SCANCODE_K},
        {SPARK_SCANCODE_L, SDL_SCANCODE_L},
        {SPARK_SCANCODE_M, SDL_SCANCODE_M},
        {SPARK_SCANCODE_N, SDL_SCANCODE_N},
        {SPARK_SCANCODE_O, SDL_SCANCODE_O},
        {SPARK_SCANCODE_P, SDL_SCANCODE_P},
        {SPARK_SCANCODE_Q, SDL_SCANCODE_Q},
        {SPARK_SCANCODE_R, SDL_SCANCODE_R},
        {SPARK_SCANCODE_S, SDL_SCANCODE_S},
        {SPARK_SCANCODE_T, SDL_SCANCODE_T},
        {SPARK_SCANCODE_U, SDL_SCANCODE_U},
        {SPARK_SCANCODE_V, SDL_SCANCODE_V},
        {SPARK_SCANCODE_W, SDL_SCANCODE_W},
        {SPARK_SCANCODE_X, SDL_SCANCODE_X},
        {SPARK_SCANCODE_Y, SDL_SCANCODE_Y},
        {SPARK_SCANCODE_Z, SDL_SCANCODE_Z},

        {SPARK_SCANCODE_1, SDL_SCANCODE_1},
        {SPARK_SCANCODE_2, SDL_SCANCODE_2},
        {SPARK_SCANCODE_3, SDL_SCANCODE_3},
        {SPARK_SCANCODE_4, SDL_SCANCODE_4},
        {SPARK_SCANCODE_5, SDL_SCANCODE_5},
        {SPARK_SCANCODE_6, SDL_SCANCODE_6},
        {SPARK_SCANCODE_7, SDL_SCANCODE_7},
        {SPARK_SCANCODE_8, SDL_SCANCODE_8},
        {SPARK_SCANCODE_9, SDL_SCANCODE_9},
        {SPARK_SCANCODE_0, SDL_SCANCODE_0},

        {SPARK_SCANCODE_RETURN, SDL_SCANCODE_RETURN},
        {SPARK_SCANCODE_ESCAPE, SDL_SCANCODE_ESCAPE},
        {SPARK_SCANCODE_BACKSPACE, SDL_SCANCODE_BACKSPACE},
        {SPARK_SCANCODE_TAB, SDL_SCANCODE_TAB},
        {SPARK_SCANCODE_SPACE, SDL_SCANCODE_SPACE},

        {SPARK_SCANCODE_MINUS, SDL_SCANCODE_MINUS},
        {SPARK_SCANCODE_EQUALS, SDL_SCANCODE_EQUALS},
        {SPARK_SCANCODE_LEFTBRACKET, SDL_SCANCODE_LEFTBRACKET},
        {SPARK_SCANCODE_RIGHTBRACKET, SDL_SCANCODE_RIGHTBRACKET},
        {SPARK_SCANCODE_BACKSLASH, SDL_SCANCODE_BACKSLASH},
        {SPARK_SCANCODE_SEMICOLON, SDL_SCANCODE_SEMICOLON},
        {SPARK_SCANCODE_APOSTROPHE, SDL_SCANCODE_APOSTROPHE},
        {SPARK_SCANCODE_GRAVE, SDL_SCANCODE_GRAVE},
        {SPARK_SCANCODE_COMMA, SDL_SCANCODE_COMMA},
        {SPARK_SCANCODE_PERIOD, SDL_SCANCODE_PERIOD},
        {SPARK_SCANCODE_SLASH, SDL_SCANCODE_SLASH},
        {SPARK_SCANCODE_CAPSLOCK, SDL_SCANCODE_CAPSLOCK},

        {SPARK_SCANCODE_F1, SDL_SCANCODE_F1},
        {SPARK_SCANCODE_F2, SDL_SCANCODE_F2},
        {SPARK_SCANCODE_F3, SDL_SCANCODE_F3},
        {SPARK_SCANCODE_F4, SDL_SCANCODE_F4},
        {SPARK_SCANCODE_F5, SDL_SCANCODE_F5},
        {SPARK_SCANCODE_F6, SDL_SCANCODE_F6},
        {SPARK_SCANCODE_F7, SDL_SCANCODE_F7},
        {SPARK_SCANCODE_F8, SDL_SCANCODE_F8},
        {SPARK_SCANCODE_F9, SDL_SCANCODE_F9},
        {SPARK_SCANCODE_F10, SDL_SCANCODE_F10},
        {SPARK_SCANCODE_F11, SDL_SCANCODE_F11},
        {SPARK_SCANCODE_F12, SDL_SCANCODE_F12},

        {SPARK_SCANCODE_RIGHT, SDL_SCANCODE_RIGHT},
        {SPARK_SCANCODE_LEFT, SDL_SCANCODE_LEFT},
        {SPARK_SCANCODE_DOWN, SDL_SCANCODE_DOWN},
        {SPARK_SCANCODE_UP, SDL_SCANCODE_UP},

        {SPARK_SCANCODE_NUMLOCK, SDL_SCANCODE_NUMLOCKCLEAR},
        {SPARK_SCANCODE_KP_DIVIDE, SDL_SCANCODE_KP_DIVIDE},
        {SPARK_SCANCODE_KP_MULTIPLY, SDL_SCANCODE_KP_MULTIPLY},
        {SPARK_SCANCODE_KP_MINUS, SDL_SCANCODE_KP_MINUS},
        {SPARK_SCANCODE_KP_PLUS, SDL_SCANCODE_KP_PLUS},
        {SPARK_SCANCODE_KP_ENTER, SDL_SCANCODE_KP_ENTER},
        {SPARK_SCANCODE_KP_1, SDL_SCANCODE_KP_1},
        {SPARK_SCANCODE_KP_2, SDL_SCANCODE_KP_2},
        {SPARK_SCANCODE_KP_3, SDL_SCANCODE_KP_3},
        {SPARK_SCANCODE_KP_4, SDL_SCANCODE_KP_4},
        {SPARK_SCANCODE_KP_5, SDL_SCANCODE_KP_5},
        {SPARK_SCANCODE_KP_6, SDL_SCANCODE_KP_6},
        {SPARK_SCANCODE_KP_7, SDL_SCANCODE_KP_7},
        {SPARK_SCANCODE_KP_8, SDL_SCANCODE_KP_8},
        {SPARK_SCANCODE_KP_9, SDL_SCANCODE_KP_9},
        {SPARK_SCANCODE_KP_0, SDL_SCANCODE_KP_0},
        {SPARK_SCANCODE_KP_PERIOD, SDL_SCANCODE_KP_PERIOD}
    };
 for (size_t i = 0; i < sizeof(mappings) / sizeof(mappings[0]); i++) {
        spark_to_sdl_scancode[mappings[i].spark] = mappings[i].sdl;
    }
}

void spark_keyboard_init(void) {
    init_scancode_mappings();  // Fixed function name
    keyboard_state.key_repeat_enabled = true;
    keyboard_state.key_repeat_delay = 400;    // Default delay (ms)
    keyboard_state.key_repeat_interval = 30;  // Default interval (ms)
    keyboard_state.text_input_enabled = false;
}

void spark_keyboard_shutdown(void) {
    if (keyboard_state.text_input_enabled) {
        SDL_StopTextInput(spark.window);  // Add window parameter
    }
}

void spark_keyboard_update(void) {
    // This could be used for any per-frame keyboard state updates
}

bool spark_keyboard_is_down(SparkScancode scancode) {
    const bool* keyboard_state = SDL_GetKeyboardState(NULL);  // Changed to bool*
    return keyboard_state[spark_to_sdl_scancode[scancode]];
}

void spark_keyboard_set_key_repeat(bool enable) {
    keyboard_state.key_repeat_enabled = enable;
}

bool spark_keyboard_has_key_repeat(void) {
    return keyboard_state.key_repeat_enabled;
}

void spark_keyboard_get_key_repeat(int* delay, int* interval) {
    if (delay) *delay = keyboard_state.key_repeat_delay;
    if (interval) *interval = keyboard_state.key_repeat_interval;
}

void spark_keyboard_set_text_input(bool enable) {
    if (enable && !keyboard_state.text_input_enabled) {
        SDL_StartTextInput(spark.window);  // Add window parameter
        keyboard_state.text_input_enabled = true;
    } else if (!enable && keyboard_state.text_input_enabled) {
        SDL_StopTextInput(spark.window);   // Add window parameter
        keyboard_state.text_input_enabled = false;
    }
}

bool spark_keyboard_has_text_input(void) {
    return keyboard_state.text_input_enabled;
}

bool spark_keyboard_has_screen_keyboard(void) {
    return SDL_HasScreenKeyboardSupport();
}
