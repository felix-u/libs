// NOTE(felix): useful resources for using sokol:
// https://floooh.github.io/sokol-html5/
// https://zeromake.github.io/learnopengl-examples/


#if !defined(BASE_PLATFORM_H)
#define BASE_PLATFORM_H

#include "base/base_context.h"

#if COMPILER_CLANG
	#pragma clang diagnostic push
	// #pragma diagnostic ignored "-Wgnu-empty-struct"
#elif COMPILER_MSVC
	#pragma warning(push)
	#pragma warning(disable: 4267)
	#pragma warning(disable: 4996)
#endif

#if BASE_OS == BASE_OS_WINDOWS
	#define SOKOL_D3D11
#elif BASE_OS == BASE_OS_MACOS
	#define SOKOL_METAL
#elif BASE_OS == BASE_OS_EMSCRIPTEN
	// #define SOKOL_WGPU
	#define SOKOL_GLES3
#else
	#error "unsupported OS"
#endif

#if BUILD_DEBUG
    #define SOKOL_DEBUG
#endif

#define SOKOL_NO_ENTRY

#if defined(BASE_PLATFORM_IMPLEMENTATION)
	#define SOKOL_IMPL
	#define FONTSTASH_IMPLEMENTATION
#endif

#include "sokol/sokol_gfx.h"
#include "sokol/sokol_gl.h"
#include <stdio.h>
#include "fontstash/fontstash.h"
#include "sokol/sokol_fontstash.h" // NOTE(felix): commented out the 3 stdlib includes on line 187
#include "sokol/sokol_app.h"
#include "sokol/sokol_glue.h"

#if COMPILER_CLANG
	#pragma clang diagnostic pop
#elif COMPILER_MSVC
	#pragma warning(pop)
#endif


#endif // !defined(BASE_PLATFORM_H)


#if defined(BASE_IMPLEMENTATION)


struct Platform {
    using(Platform_Common, common);

    sg_pipeline pipeline;
    sg_pass_action pass_action;
    f32 max_font_size;
    FONScontext *fontstash_context;
    i32 font_id;
};

#include "../shader.c"

static void app_quit(Platform *);
static void app_start(Platform *);
static void app_update_and_render(Platform *);

static void sokol_event(const sapp_event *event, void *user_data) {
    Platform *platform = user_data;
    App_Frame_Info *info = &platform->frame_info;

    App_Key key_code = 0;
    // NOTE(felix): up to 96 (ASCII grave '`'), both Sokol and us use the corresponding ASCII value
    if (event->key_code <= '`')  {
        key_code = cast(App_Key) event->key_code;
    } else switch (event->key_code) {
        case SAPP_KEYCODE_LEFT_CONTROL: key_code = App_Key_CONTROL; break;
        case SAPP_KEYCODE_LEFT: key_code = App_Key_LEFT; break;
        case SAPP_KEYCODE_RIGHT: key_code = App_Key_RIGHT; break;
        default: {
            log_info("unimplemented key translation from sokol key code %d", cast(i32) event->key_code);
        } break;
    }

    App_Mouse_Button mouse_button = 0;
    switch (event->mouse_button) {
        case SAPP_MOUSEBUTTON_INVALID: mouse_button = App_Mouse_Button_NIL; break;
        case SAPP_MOUSEBUTTON_LEFT: mouse_button = App_Mouse_Button_LEFT; break;
        case SAPP_MOUSEBUTTON_RIGHT: mouse_button = App_Mouse_Button_RIGHT; break;
        case SAPP_MOUSEBUTTON_MIDDLE: mouse_button = App_Mouse_Button_MIDDLE; break;
        default: unreachable;
    }

    switch (event->type) {
        case SAPP_EVENTTYPE_KEY_DOWN: {
            info->key_pressed[key_code] = true;
            info->key_down[key_code] = true;
        } break;
        case SAPP_EVENTTYPE_KEY_UP: {
            info->key_down[key_code] = false;
        } break;
        case SAPP_EVENTTYPE_MOUSE_DOWN: {
            info->mouse_clicked[mouse_button] = true;
            info->mouse_down[mouse_button] = true;
        } break;
        case SAPP_EVENTTYPE_MOUSE_UP: {
            info->mouse_down[mouse_button] = false;
        } break;
        case SAPP_EVENTTYPE_MOUSE_MOVE: {
            info->mouse_position[0] = event->mouse_x;
            info->mouse_position[1] = event->mouse_y;
        } break;
        default: break;
    }
}

static void sokol_logger(const char *tag, u32 level, u32 item, const char *message, u32 line_number, const char *filename, void *user_data) {
    discard user_data;
    print("%s:%u: %s %u %u '%s'\n", filename, line_number, tag, level, item, message);
}

static void sokol_populate_frame_info(App_Frame_Info *info) {
    info->dpi_scale = sapp_dpi_scale();
    info->window_size[0] = sapp_widthf();
    info->window_size[1] = sapp_heightf();
    info->seconds_since_last_frame = cast(f32) sapp_frame_duration();
}

static void sokol_cleanup(void *user_data) {
    Platform *platform = user_data;

    app_quit(platform);

    sfons_destroy(platform->fontstash_context);
    sgl_shutdown();
    sg_shutdown();
}

static void sokol_init(void *user_data) {
    Platform *platform = user_data;
    sokol_populate_frame_info(&platform->frame_info);

    sg_setup(&(sg_desc){
        .environment = sglue_environment(),
        .logger.func = sokol_logger,
    });

    sgl_setup(&(sgl_desc_t){
        .logger.func = sokol_logger,
    });

    sg_shader shader = sg_make_shader(uber_shader_desc(sg_query_backend()));

    platform->pipeline = sg_make_pipeline(&(sg_pipeline_desc){
        .shader = shader,
        .colors[0].blend = {
            .enabled = true,
            .src_factor_rgb = SG_BLENDFACTOR_SRC_ALPHA,
            .dst_factor_rgb = SG_BLENDFACTOR_ONE_MINUS_SRC_ALPHA,
            .src_factor_alpha = SG_BLENDFACTOR_ONE,
            .dst_factor_alpha = SG_BLENDFACTOR_ONE_MINUS_SRC_ALPHA,
        },
    });

    platform->pass_action = (sg_pass_action){
        .colors[0] = { .load_action = SG_LOADACTION_CLEAR },
    };

    {
        platform->max_font_size = 1024.f * platform->dpi_scale;
        u64 next_power_of_2_from_dpi = 0;
        {
            u64 x = cast(u64) (platform->max_font_size) - 1;
            for (u64 i = 0; i < 5; i += 1) x |= (x >> (1 << i));
            next_power_of_2_from_dpi = x + 1;
        }
        i32 atlas_dimension = cast(i32) next_power_of_2_from_dpi;

        platform->fontstash_context = sfons_create(&(sfons_desc_t){
            .width = atlas_dimension,
            .height = atlas_dimension,
        });

        // TODO(felix): embed via metaprogram
        String font_ttf_file = {0};
        #if BASE_OS == BASE_OS_EMSCRIPTEN
        {
            #include "inter.c"
            font_ttf_file.data = (u8 *)inter_ttf_bytes;
            font_ttf_file.count = sizeof inter_ttf_bytes;
        }
        #else
        {
            font_ttf_file = os_read_entire_file(platform->persistent_arena, "deps/Inter-4.1/InterVariable.ttf", 0);
        }
        #endif
        assert(font_ttf_file.count != 0);
        platform->font_id = fonsAddFontMem(platform->fontstash_context, "sans", font_ttf_file.data, cast(i32) font_ttf_file.count, false);
        fonsSetFont(platform->fontstash_context, platform->font_id);
    }

    app_start(platform);
    platform->pass_action.colors[0].clear_value = bit_cast(sg_color) platform->clear_color;
}

static void sokol_frame(void *user_data) {
    Platform *platform = user_data;

    sokol_populate_frame_info(&platform->frame_info);
    platform->draw_commands = (Array_Draw_Command){ .arena = platform->frame_arena };
    Scratch scratch = scratch_begin(platform->frame_arena);

    if (platform->fontstash_context != 0) fonsClearState(platform->fontstash_context);

    sg_begin_pass(&(sg_pass){
        .action = platform->pass_action,
        .swapchain = sglue_swapchain(),
    });
    sg_apply_pipeline(platform->pipeline);

    platform->pass_action.colors[0].clear_value = bit_cast(sg_color) platform->clear_color;
    app_update_and_render(platform);

    // TODO(felix): fix
    bool hack_because_sdf_shader_is_being_weird_on_web = BASE_OS == BASE_OS_EMSCRIPTEN;
    if (hack_because_sdf_shader_is_being_weird_on_web) for_slice (Draw_Command *, command, platform->draw_commands) switch (command->kind) {
        case Draw_Kind_RECTANGLE: {
            command->kind = Draw_Kind_QUADRILATERAL;
            Draw_Command new = { .quadrilateral = {
                [Draw_Corner_BOTTOM_LEFT] = { command->position[0], command->position[1] + command->rectangle.size[1] },
                [Draw_Corner_TOP_RIGHT]   = { command->position[0] + command->rectangle.size[0], command->position[1] },
            } };

            v_copy(new.quadrilateral[Draw_Corner_TOP_LEFT], command->position);

            v_copy(new.quadrilateral[Draw_Corner_BOTTOM_RIGHT], command->position);
            v_add(new.quadrilateral[Draw_Corner_BOTTOM_RIGHT], command->rectangle.size);

            for (Draw_Corner corner = 0; corner < Draw_Corner_COUNT; corner += 1) {
                v_copy(command->quadrilateral[corner], new.quadrilateral[corner]);
            }
        } break;
        default: break;
    }

    for_slice (Draw_Command *, command, platform->draw_commands) switch (command->kind) {
        case Draw_Kind_TEXT: {
            u32 font_color_abgr = 0;
            {
                f32 *c = command->color[Draw_Color_SOLID];
                u8 r = cast(u8) (c[0] * 255.f + 0.5f);
                u8 g = cast(u8) (c[1] * 255.f + 0.5f);
                u8 b = cast(u8) (c[2] * 255.f + 0.5f);
                u8 a = cast(u8) (c[3] * 255.f + 0.5f);
                font_color_abgr = sfons_rgba(r, g, b, a);
            }

            fonsSetSize(platform->fontstash_context, command->text.font_size);

            String s = command->text.string;

            fonsSetAlign(platform->fontstash_context, FONS_ALIGN_LEFT | FONS_ALIGN_TOP);
            fonsSetColor(platform->fontstash_context, font_color_abgr);
            fonsDrawText(platform->fontstash_context, command->position[0], command->position[1], cast(char *) s.data, cast(char *) s.data + s.count);
        } break;
        case Draw_Kind_RECTANGLE: {
            assert(command->rectangle.size[0] >= 0);
            assert(command->rectangle.size[1] >= 0);

            f32 pivot[2] = {0};
            if (v_equals_(command->rectangle.pivot, (f32[2]){0}, 2)) {
                f32 add[2]; v_copy(add, command->rectangle.size);
                v_scale(add, 0.5f);

                v_copy(pivot, command->position);
                v_add(pivot, add);
            } else v_copy(pivot, command->rectangle.pivot);

            M3 model = m3_from_rotation(command->rectangle.rotation_radians, pivot);

            f32 (*c)[4] = command->color;

            vertex_parameters_t vertex_parameters = {
                .kind = 0,
                .model = m4_from_top_left_m3(model),
                .inverse_model = m4_from_top_left_m3(m3_inverse(model)),
                .border_data = { command->rectangle.border_width, command->rectangle.border_radius },
            };

            v_copy(vertex_parameters.framebuffer_size, platform->window_size);

            v_copy(vertex_parameters.top_left_pixels, command->position);

            v_copy(vertex_parameters.bottom_right_pixels, command->position);
            v_add(vertex_parameters.bottom_right_pixels, command->rectangle.size);

            v_copy_(vertex_parameters.color, c[Draw_Color_SOLID], 4);
            v_copy_(vertex_parameters.bottom_left_color, command->gradient ? c[Draw_Color_BOTTOM_LEFT] : c[Draw_Color_SOLID], 4);
            v_copy_(vertex_parameters.bottom_right_color, command->gradient ? c[Draw_Color_BOTTOM_RIGHT] : c[Draw_Color_SOLID], 4);
            v_copy_(vertex_parameters.top_right_color, command->gradient ? c[Draw_Color_TOP_RIGHT] : c[Draw_Color_SOLID], 4);
            v_copy(vertex_parameters.border_color, command->rectangle.border_color);

            static_assert(sizeof(vertex_parameters_t) == sizeof(fragment_parameters_t), "");
            fragment_parameters_t fragment_parameters = bit_cast(fragment_parameters_t) vertex_parameters;

            sg_apply_uniforms(UB_vertex_parameters, &SG_RANGE(vertex_parameters));
            sg_apply_uniforms(UB_fragment_parameters, &SG_RANGE(fragment_parameters));

            sg_draw(0, 6, 1);
        } break;
        case Draw_Kind_QUADRILATERAL: {
            f32 (*c)[4] = command->color;
            vertex_parameters_t vertex_parameters = {
                .kind = 1,
                .model = m4_fill_diagonal(1.f),
            };

            v_copy(vertex_parameters.framebuffer_size, platform->window_size);

            v_copy(vertex_parameters.quad_top_left, command->quadrilateral[Draw_Corner_TOP_LEFT]);
            v_copy(vertex_parameters.quad_bottom_left, command->quadrilateral[Draw_Corner_BOTTOM_LEFT]);
            v_copy(vertex_parameters.quad_bottom_right, command->quadrilateral[Draw_Corner_BOTTOM_RIGHT]);
            v_copy(vertex_parameters.quad_top_right, command->quadrilateral[Draw_Corner_TOP_RIGHT]);

            v_copy_(vertex_parameters.color, c[Draw_Color_SOLID], 4);
            v_copy_(vertex_parameters.bottom_left_color, command->gradient ? c[Draw_Color_BOTTOM_LEFT] : c[Draw_Color_SOLID], 4);
            v_copy_(vertex_parameters.bottom_right_color, command->gradient ? c[Draw_Color_BOTTOM_RIGHT] : c[Draw_Color_SOLID], 4);
            v_copy_(vertex_parameters.top_right_color, command->gradient ? c[Draw_Color_TOP_RIGHT] : c[Draw_Color_SOLID], 4);
            v_copy(vertex_parameters.border_color, command->rectangle.border_color);

            static_assert(sizeof(vertex_parameters_t) == sizeof(fragment_parameters_t), "");
            fragment_parameters_t fragment_parameters = bit_cast(fragment_parameters_t) vertex_parameters;

            sg_apply_uniforms(UB_vertex_parameters, &SG_RANGE(vertex_parameters));
            sg_apply_uniforms(UB_fragment_parameters, &SG_RANGE(fragment_parameters));

            sg_draw(0, 6, 1);
        } break;
        default: unreachable;
    }

    sfons_flush(platform->fontstash_context);
    sgl_defaults();
    sgl_matrix_mode_projection();
    sgl_ortho(0, platform->window_size[0], platform->window_size[1], 0, -1.f, 1.f);
    sgl_draw();

    sg_end_pass();
    sg_commit();

    memset(platform->mouse_clicked, 0, sizeof platform->mouse_clicked);
    memset(platform->key_pressed, 0, sizeof platform->key_pressed);
    scratch_end(scratch);

    if (platform->should_quit) sapp_quit();
}

static void platform_measure_text(Platform_Common *platform, String text, f32 font_size, f32 out[2]) {
    FONScontext *fontstash_context = (cast(Platform *) platform)->fontstash_context;
    fonsSetSize(fontstash_context, font_size);

    f32 bounds[4] = {0};
    String s = text;
    out[0] = fonsTextBounds(fontstash_context, 0, 0, cast(char *) s.data, cast(char *) s.data + s.count, bounds);
    out[1] = bounds[3] - bounds[1];
}

static void program(void) {
    static Arena persistent_arena;
    persistent_arena = arena_init(32 * 1024 * 1024);
    static Arena frame_arena;
    frame_arena = arena_init(32 * 1024 * 1024);

    static Platform platform = {
        .persistent_arena = &persistent_arena,
        .frame_arena = &frame_arena,
    };

    sapp_run(&(sapp_desc){
        .user_data = &platform,
        .init_userdata_cb = sokol_init,
        .frame_userdata_cb = sokol_frame,
        .cleanup_userdata_cb = sokol_cleanup,
        .event_userdata_cb = sokol_event,
        .window_title = "app",
        .logger.func = sokol_logger,
        .high_dpi = true,
    });
}


#endif // defined(BASE_IMPLEMENTATION)
