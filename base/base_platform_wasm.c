#define BASE_PLATFORM_WASM

#include "base/base_platform_gles3_common.c"

struct Platform_Implementation {
    Platform base;
    GLES3_GPU gpu;
};

static GLES3_GPU *gles3__gpu_from_platform(Platform *platform) {
    return &((Platform_Implementation *)platform)->gpu;
}

static Platform_Implementation platform__;

typedef enum { // https://developer.mozilla.org/en-US/docs/Web/API/MouseEvent/button
    JS_Mouse_Button_LEFT = 0,
    JS_Mouse_Button_MIDDLE = 1,
    JS_Mouse_Button_RIGHT = 2,

    JS_Mouse_Button_MAX_VALUE,
} JS_Mouse_Button;

static App_Key app_key_from_js(const char *bytes, u32 count) {
    App_Key key = 0;
    String js_key = { .data = bytes, .count = (u64)count };

    // https://developer.mozilla.org/en-US/docs/Web/API/UI_Events/Keyboard_event_code_values

    char compare_alpha_buffer[] = "KeyA";
    String compare_alpha = { .data = compare_alpha_buffer, .count = 4 };
    for (char letter = 'A'; letter <= 'Z'; letter += 1) {
        compare_alpha_buffer[3] = letter;
        if (string_equals(js_key, compare_alpha)) {
            key = (App_Key)letter;
            break;
        }
    }

    if (key == 0) {
        char compare_digit_buffer[] = "Digit0";
        String compare_digit = { .data = compare_digit_buffer, .count = 6 };
        for (char digit = '0'; digit <= '9'; digit += 1) {
            if (string_equals(js_key, compare_digit)) {
                key = (App_Key)digit;
                break;
            }
        }
    }

    if (key == 0) {
        if (0) {}
        else if (string_equals(js_key, S("ShiftLeft"))) key = App_Key_SHIFT;
        else if (string_equals(js_key, S("ControlLeft")) || string_equals(js_key, S("ControlRight"))) key = App_Key_CONTROL;
        else if (string_equals(js_key, S("Equal"))) key = '=';
        else if (string_equals(js_key, S("Minus")))  key = '-';
        else {
            static bool once = false;
            if (!once) {
                log_info("unimplemented translation from JS key '%S' (logging ONCE for all keys)", js_key);
                once = true;
            }
        }
    }

    return key;
}

static App_Mouse_Button app_mouse_button_from_js(JS_Mouse_Button js_button) {
    App_Mouse_Button button = 0;
    switch (js_button) {
        case JS_Mouse_Button_LEFT: button = App_Mouse_Button_LEFT; break;
        case JS_Mouse_Button_MIDDLE: button = App_Mouse_Button_MIDDLE; break;
        case JS_Mouse_Button_RIGHT: button = App_Mouse_Button_RIGHT; break;
        default: {
            static bool once = false;
            if (!once) {
                log_info("unimplemented translation from JS button <0x%x> (logging ONCE for all buttons)", js_button);
                once = true;
            }
        } break;
    }
    return button;
}

// TODO(felix): Replace these with a bespoke META(export) directive? Once the metaprogram has semantic parsing.
META(link_flag "-Wl,--export=platform__get_temp_js_string_buffer")
char *platform__get_temp_js_string_buffer(void) {
    static char platform__js_string_buffer[64];
    return platform__js_string_buffer;
}

META(link_flag "-Wl,--export=platform__on_key_down")
void platform__on_key_down(const char *bytes, u32 count) {
    App_Frame_Info *frame = &platform__.base.frame;
    App_Key key = app_key_from_js(bytes, count);
    frame->key_pressed[key] = !frame->key_down[key];
    frame->key_down[key] = true;
}

META(link_flag "-Wl,--export=platform__on_key_up")
void platform__on_key_up(const char *bytes, u32 count) {
    App_Key key = app_key_from_js(bytes, count);
    platform__.base.frame.key_down[key] = false;
}

META(link_flag "-Wl,--export=platform__on_mouse_down")
void platform__on_mouse_down(JS_Mouse_Button js_button) {
    App_Frame_Info *frame = &platform__.base.frame;
    App_Mouse_Button button = app_mouse_button_from_js(js_button);
    frame->mouse_clicked[button] = !frame->mouse_down[button];
    frame->mouse_down[button] = true;
}

META(link_flag "-Wl,--export=platform__on_mouse_up")
void platform__on_mouse_up(JS_Mouse_Button js_button) {
    App_Mouse_Button button = app_mouse_button_from_js(js_button);
    platform__.base.frame.mouse_down[button] = false;
}

META(link_flag "-Wl,--export=platform__on_scroll")
void platform__on_scroll(f32 x, f32 y) {
    (void)x; // for now
    platform__.base.frame.scroll += y / -120.f;
}

META(link_flag "-Wl,--export=platform__start")
void platform__start(void) {
    static Arena persistent_arena, frame_arena;
    Platform *base = &platform__.base;
    base->persistent_arena = &persistent_arena;
    base->frame_arena = &frame_arena;

    app_start(base);
    base_platform_reserve_frame_arena(base);

    base->frame.dpi_scale = 1.f;
    gles3__initialise(&platform__.gpu, base->persistent_arena);
}

META(link_flag "-Wl,--export=platform__update_and_render")
void platform__update_and_render(f32 window_width, f32 window_height, f32 mouse_x, f32 mouse_y) {
    Scratch scratch = scratch_begin(platform__.base.frame_arena);
    App_Frame_Info *frame = &platform__.base.frame;
    frame->window_size = (V2){ .x = window_width, .y = window_height };
    frame->mouse_position = (V2){ .x = mouse_x, .y = mouse_y };

    gles3__begin_frame(&platform__.base);

    static Time_Averaged_Sampler sampler = {0};
    static bool sampler_init = false;
    if (!sampler_init) {
        #define PLATFORM__TIME_SAMPLES 16
        static f64 samples[PLATFORM__TIME_SAMPLES] = {0};

        sampler = time_averaged_sampler(samples, PLATFORM__TIME_SAMPLES);
        sampler_init = true;
    }
    frame->seconds_since_last_frame = (f32)time_averaged_sample_elapsed_seconds(&sampler);
    frame->seconds_since_last_frame = CLAMP_HIGH(0.1f, frame->seconds_since_last_frame);

    app_update_and_render(&platform__.base);

    void js_set_cursor_hidden(bool hidden);
    js_set_cursor_hidden(platform__.base.hide_cursor);

    gles3__end_frame(&platform__.base);

    app_frame_info_reset_end_of_frame(frame);
    scratch_end(scratch);
}

static void program(void) {
    panic("stub, don't call");
}
