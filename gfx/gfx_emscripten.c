static Gfx *gfx_create(Gfx gfx) {
    g_gfx = gfx;
    g_gfx.t_delta = 1.f / (f32)g_gfx.fps;
    return &g_gfx;
}

static void gfx_get_input(void) {
    // log_info("TODO");
}

static void (*gfx__user_loop_fn)(void);

static void gfx__main_loop_fn_emscripten(void) {
    gfx_get_input();
    gfx__user_loop_fn();
    if (!gfx_render()) emscripten_cancel_main_loop();
}

static void gfx_main_loop(void (*user_loop_fn)(void)) {
    gfx__user_loop_fn = user_loop_fn;
    emscripten_set_main_loop(gfx__main_loop_fn_emscripten, g_gfx.fps, false);
}

static bool gfx_render(void) {
    EM_ASM({
        var canvas = document.getElementById("canvas");
        var ctx = canvas.getContext("2d");
        var image_data = ctx.createImageData($0, $1);
        image_data.data.set(new Uint8Array(Module.HEAP8.buffer, $2, $0 * $1 * 4));
        ctx.putImageData(image_data, 0, 0);
    }, g_gfx.width, g_gfx.height, g_gfx.pixels);

    gfx_update_t_delta();
    return true;
}

static force_inline u32 gfx_rgb(u32 rgb) { return byte_swap_u32(rgb << 8) | 0xff000000; }

static void gfx_update_t_delta(void) { /* guaranteed by emscripten main loop timing */ }
