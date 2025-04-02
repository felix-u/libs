// TODO(felix): is vsync enabled by default? if not, let's enable it
// TODO(felix): get delta time

static Gfx_Font gfx_font_default_3x5 = {
    .width = 3, .height = 5,
    .glyphs = {
        [' '] = {
            0, 0, 0,
            0, 0, 0,
            0, 0, 0,
            0, 0, 0,
            0, 0, 0,
        },
        ['!'] = {
            0, 1, 0,
            0, 1, 0,
            0, 1, 0,
            0, 0, 0,
            0, 1, 0,
        },
        ['"'] = {
            1, 0, 1,
            1, 0, 1,
            0, 0, 0,
            0, 0, 0,
            0, 0, 0,
        },
        ['#'] = {
            1, 0, 1,
            1, 1, 1,
            1, 0, 1,
            1, 1, 1,
            1, 0, 1,
        },
        ['$'] = {
            0, 1, 0,
            0, 1, 1,
            0, 1, 0,
            1, 1, 0,
            0, 1, 0,
        },
        ['%'] = {
            1, 0, 1,
            0, 0, 1,
            0, 1, 0,
            1, 0, 0,
            1, 0, 1,
        },
        ['&'] = {
            0, 1, 0,
            1, 0, 1,
            1, 1, 0,
            1, 0, 1,
            0, 1, 1,
        },
        ['\''] = {
            0, 1, 0,
            0, 1, 0,
            0, 0, 0,
            0, 0, 0,
            0, 0, 0,
        },
        ['('] = {
            0, 1, 0,
            1, 0, 0,
            1, 0, 0,
            1, 0, 0,
            0, 1, 0,
        },
        [')'] = {
            0, 1, 0,
            0, 0, 1,
            0, 0, 1,
            0, 0, 1,
            0, 1, 0,
        },
        ['*'] = {
            0, 0, 0,
            1, 0, 1,
            0, 1, 0,
            1, 0, 1,
            0, 0, 0,
        },
        ['+'] = {
            0, 0, 0,
            0, 1, 0,
            1, 1, 1,
            0, 1, 0,
            0, 0, 0,
        },
        [','] = {
            0, 0, 0,
            0, 0, 0,
            0, 0, 0,
            0, 1, 0,
            1, 0, 0,
        },
        ['-'] = {
            0, 0, 0,
            0, 0, 0,
            1, 1, 1,
            0, 0, 0,
            0, 0, 0,
        },
        ['.'] = {
            0, 0, 0,
            0, 0, 0,
            0, 0, 0,
            0, 0, 0,
            0, 1, 0,
        },
        ['/'] = {
            0, 0, 1,
            0, 0, 1,
            0, 1, 0,
            1, 0, 0,
            1, 0, 0,
        },
        ['0'] = {
            1, 1, 1,
            1, 0, 1,
            1, 0, 1,
            1, 0, 1,
            1, 1, 1,
        },
        ['1'] = {
            0, 1, 0,
            1, 1, 0,
            0, 1, 0,
            0, 1, 0,
            0, 1, 0,
        },
        ['2'] = {
            1, 1, 1,
            1, 0, 1,
            0, 0, 1,
            1, 1, 0,
            1, 1, 1,
        },
        ['3'] = {
            1, 1, 1,
            0, 0, 1,
            1, 1, 1,
            0, 0, 1,
            1, 1, 1,
        },
        ['4'] = {
            1, 0, 1,
            1, 0, 1,
            1, 1, 1,
            0, 0, 1,
            0, 0, 1,
        },
        ['5'] = {
            1, 1, 1,
            1, 0, 0,
            1, 1, 0,
            0, 0, 1,
            1, 1, 0,
        },
        ['6'] = {
            1, 1, 1,
            1, 0, 0,
            1, 1, 1,
            1, 0, 1,
            1, 1, 1,
        },
        ['7'] = {
            1, 1, 1,
            0, 0, 1,
            0, 1, 0,
            0, 1, 0,
            0, 1, 0,
        },
        ['8'] = {
            1, 1, 1,
            1, 0, 1,
            1, 1, 1,
            1, 0, 1,
            1, 1, 1,
        },
        ['9'] = {
            1, 1, 1,
            1, 0, 1,
            1, 1, 1,
            0, 0, 1,
            1, 1, 1,
        },
        [':'] = {
            0, 0, 0,
            0, 0, 0,
            0, 1, 0,
            0, 0, 0,
            0, 1, 0,
        },
        [';'] = {
            0, 0, 0,
            0, 1, 0,
            0, 0, 0,
            0, 1, 0,
            1, 0, 0,
        },
        ['<'] = {
            0, 0, 1,
            0, 1, 0,
            1, 0, 0,
            0, 1, 0,
            0, 0, 1,
        },
        ['='] = {
            0, 0, 0,
            1, 1, 1,
            0, 0, 0,
            1, 1, 1,
            0, 0, 0,
        },
        ['>'] = {
            1, 0, 0,
            0, 1, 0,
            0, 0, 1,
            0, 1, 0,
            1, 0, 0,
        },
        ['?'] = {
            1, 1, 0,
            0, 0, 1,
            0, 1, 0,
            0, 0, 0,
            0, 1, 0,
        },
        ['@'] = {
            1, 1, 1,
            1, 0, 1,
            1, 0, 1,
            1, 0, 0,
            1, 1, 1,
        },
        ['A'] = {
            0, 1, 0,
            1, 0, 1,
            1, 1, 1,
            1, 0, 1,
            1, 0, 1,
        },
        ['B'] = {
            1, 1, 0,
            1, 0, 1,
            1, 1, 1,
            1, 0, 1,
            1, 1, 0,
        },
        ['C'] = {
            0, 1, 1,
            1, 0, 0,
            1, 0, 0,
            1, 0, 0,
            0, 1, 1,
        },
        ['D'] = {
            1, 1, 0,
            1, 0, 1,
            1, 0, 1,
            1, 0, 1,
            1, 1, 0,
        },
        ['E'] = {
            1, 1, 1,
            1, 0, 0,
            1, 1, 1,
            1, 0, 0,
            1, 1, 1,
        },
        ['F'] = {
            1, 1, 1,
            1, 0, 0,
            1, 1, 1,
            1, 0, 0,
            1, 0, 0,
        },
        ['G'] = {
            0, 1, 1,
            1, 0, 0,
            1, 0, 1,
            1, 0, 1,
            0, 1, 0,
        },
        ['H'] = {
            1, 0, 1,
            1, 0, 1,
            1, 1, 1,
            1, 0, 1,
            1, 0, 1,
        },
        ['I'] = {
            1, 1, 1,
            0, 1, 0,
            0, 1, 0,
            0, 1, 0,
            1, 1, 1,
        },
        ['J'] = {
            1, 1, 1,
            0, 0, 1,
            1, 0, 1,
            1, 0, 1,
            0, 1, 0,
        },
        ['K'] = {
            1, 0, 1,
            1, 0, 1,
            1, 1, 0,
            1, 0, 1,
            1, 0, 1,
        },
        ['L'] = {
            1, 0, 0,
            1, 0, 0,
            1, 0, 0,
            1, 0, 0,
            1, 1, 1,
        },
        ['M'] = {
            1, 0, 1,
            1, 1, 1,
            1, 0, 1,
            1, 0, 1,
            1, 0, 1,
        },
        ['N'] = {
            1, 0, 1,
            1, 1, 1,
            1, 1, 1,
            1, 0, 1,
            1, 0, 1,
        },
        ['O'] = {
            0, 1, 0,
            1, 0, 1,
            1, 0, 1,
            1, 0, 1,
            0, 1, 0,
        },
        ['P'] = {
            1, 1, 0,
            1, 0, 1,
            1, 1, 0,
            1, 0, 0,
            1, 0, 0,
        },
        ['Q'] = {
            0, 1, 0,
            1, 0, 1,
            1, 0, 1,
            0, 1, 0,
            0, 0, 1,
        },
        ['R'] = {
            1, 1, 0,
            1, 0, 1,
            1, 1, 0,
            1, 0, 1,
            1, 0, 1,
        },
        ['S'] = {
            0, 1, 1,
            1, 0, 0,
            0, 1, 0,
            0, 0, 1,
            1, 1, 0,
        },
        ['T'] = {
            1, 1, 1,
            0, 1, 0,
            0, 1, 0,
            0, 1, 0,
            0, 1, 0,
        },
        ['U'] = {
            1, 0, 1,
            1, 0, 1,
            1, 0, 1,
            1, 0, 1,
            1, 1, 1,
        },
        ['V'] = {
            1, 0, 1,
            1, 0, 1,
            1, 0, 1,
            1, 0, 1,
            0, 1, 0,
        },
        ['W'] = {
            1, 0, 1,
            1, 0, 1,
            1, 0, 1,
            1, 1, 1,
            1, 0, 1,
        },
        ['X'] = {
            1, 0, 1,
            1, 0, 1,
            0, 1, 0,
            1, 0, 1,
            1, 0, 1,
        },
        ['Y'] = {
            1, 0, 1,
            1, 0, 1,
            0, 1, 0,
            0, 1, 0,
            0, 1, 0,
        },
        ['Z'] = {
            1, 1, 1,
            0, 0, 1,
            0, 1, 0,
            1, 0, 0,
            1, 1, 1,
        },
        ['['] = {
            1, 1, 0,
            1, 0, 0,
            1, 0, 0,
            1, 0, 0,
            1, 1, 0,
        },
        ['\\'] = {
            1, 0, 0,
            1, 0, 0,
            0, 1, 0,
            0, 0, 1,
            0, 0, 1,
        },
        [']'] = {
            0, 1, 1,
            0, 0, 1,
            0, 0, 1,
            0, 0, 1,
            0, 1, 1,
        },
        ['^'] = {
            0, 1, 0,
            1, 0, 1,
            1, 0, 1,
            0, 0, 0,
            0, 0, 0,
        },
        ['_'] = {
            0, 0, 0,
            0, 0, 0,
            0, 0, 0,
            0, 0, 0,
            1, 1, 1,
        },
        ['`'] = {
            1, 0, 0,
            0, 1, 0,
            0, 0, 0,
            0, 0, 0,
            0, 0, 0,
        },
        ['a'] = {
            0, 0, 0,
            0, 1, 1,
            1, 0, 1,
            1, 0, 1,
            1, 1, 1,
        },
        ['b'] = {
            1, 0, 0,
            1, 1, 1,
            1, 0, 1,
            1, 0, 1,
            1, 1, 1,
        },
        ['c'] = {
            0, 0, 0,
            1, 1, 1,
            1, 0, 0,
            1, 0, 0,
            1, 1, 1,
        },
        ['d'] = {
            0, 0, 1,
            1, 1, 1,
            1, 0, 1,
            1, 0, 1,
            1, 1, 1,
        },
        ['e'] = {
            0, 0, 0,
            1, 1, 1,
            1, 1, 1,
            1, 0, 0,
            1, 1, 1,
        },
        ['f'] = {
            0, 1, 1,
            0, 1, 0,
            1, 1, 1,
            0, 1, 0,
            0, 1, 0,
        },
        ['g'] = {
            0, 0, 0,
            1, 1, 1,
            1, 1, 1,
            0, 0, 1,
            1, 1, 1,
        },
        ['h'] = {
            1, 0, 0,
            1, 1, 1,
            1, 0, 1,
            1, 0, 1,
            1, 0, 1,
        },
        ['i'] = {
            0, 0, 0,
            1, 1, 0,
            0, 1, 0,
            0, 1, 0,
            1, 1, 1,
        },
        ['j'] = {
            0, 0, 0,
            0, 1, 1,
            0, 0, 1,
            1, 0, 1,
            0, 1, 1,
        },
        ['k'] = {
            1, 0, 0,
            1, 0, 1,
            1, 1, 0,
            1, 1, 0,
            1, 0, 1,
        },
        ['l'] = {
            1, 1, 0,
            0, 1, 0,
            0, 1, 0,
            0, 1, 0,
            0, 1, 1,
        },
        ['m'] = {
            0, 0, 0,
            1, 0, 1,
            1, 1, 1,
            1, 0, 1,
            1, 0, 1,
        },
        ['n'] = {
            0, 0, 0,
            1, 1, 0,
            1, 0, 1,
            1, 0, 1,
            1, 0, 1,
        },
        ['o'] = {
            0, 0, 0,
            1, 1, 1,
            1, 0, 1,
            1, 0, 1,
            1, 1, 1,
        },
        ['p'] = {
            0, 0, 0,
            1, 1, 1,
            1, 0, 1,
            1, 1, 1,
            1, 0, 0,
        },
        ['q'] = {
            0, 0, 0,
            1, 1, 1,
            1, 0, 1,
            1, 1, 1,
            0, 0, 1,
        },
        ['r'] = {
            0, 0, 0,
            1, 1, 0,
            1, 0, 1,
            1, 0, 0,
            1, 0, 0,
        },
        ['s'] = {
            0, 0, 0,
            0, 1, 1,
            1, 1, 0,
            0, 0, 1,
            1, 1, 0,
        },
        ['t'] = {
            0, 1, 0,
            1, 1, 1,
            0, 1, 0,
            0, 1, 0,
            0, 1, 0,
        },
        ['u'] = {
            0, 0, 0,
            1, 0, 1,
            1, 0, 1,
            1, 0, 1,
            1, 1, 1,
        },
        ['v'] = {
            0, 0, 0,
            1, 0, 1,
            1, 0, 1,
            1, 0, 1,
            0, 1, 0,
        },
        ['w'] = {
            0, 0, 0,
            1, 0, 1,
            1, 0, 1,
            1, 1, 1,
            1, 0, 1,
        },
        ['x'] = {
            0, 0, 0,
            1, 0, 1,
            0, 1, 0,
            0, 1, 0,
            1, 0, 1,
        },
        ['y'] = {
            0, 0, 0,
            1, 0, 1,
            1, 1, 1,
            0, 0, 1,
            1, 1, 0,
        },
        ['z'] = {
            0, 0, 0,
            1, 1, 1,
            0, 1, 1,
            1, 0, 0,
            1, 1, 1,
        },
        ['{'] = {
            0, 1, 1,
            0, 1, 0,
            1, 0, 0,
            0, 1, 0,
            0, 1, 1,
        },
        ['|'] = {
            0, 1, 0,
            0, 1, 0,
            0, 0, 0,
            0, 1, 0,
            0, 1, 0,
        },
        ['}'] = {
            1, 1, 0,
            0, 1, 0,
            0, 0, 1,
            0, 1, 0,
            1, 1, 0,
        },
        ['~'] = {
            0, 0, 0,
            1, 0, 0,
            1, 1, 1,
            0, 0, 1,
            0, 0, 0,
        },
    },
};

static force_inline u32 gfx_abgr_from_rgba(u32 rgba) {
    u32 result =
        ((rgba & 0x0000ff00) << 8) |
        ((rgba & 0x00ff0000) >> 8) |
        ((rgba & 0xff000000) >> 24) |
        ((rgba & 0x000000ff) << 24);
    return result;
}

static V2 gfx_get_real_screen_size(Gfx_Render_Context *gfx) {
    gfx->frame_info.monitor_handle = MonitorFromWindow(gfx->window_handle, MONITOR_DEFAULTTONEAREST);

    MONITORINFO info = { .cbSize = sizeof(MONITORINFO) };
    bool ok = GetMonitorInfoA(gfx->frame_info.monitor_handle, &info);
    assert(ok);

    V2 size;
    size.x = (f32)(info.rcMonitor.right - info.rcMonitor.left);
    size.y = (f32)(info.rcMonitor.bottom - info.rcMonitor.top);
    return size;
}

static void gfx_render_begin(Gfx_Render_Context *gfx) {
    vcalla(gfx->device_context, IASetInputLayout, gfx->input_layout);
    vcalla(gfx->device_context, IASetPrimitiveTopology, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    u32 stride = sizeof(Gfx_Vertex);
    u32 offset = 0;
    vcalla(gfx->device_context, IASetVertexBuffers, 0, 1, &gfx->vertex_buffer, &stride, &offset);

    vcalla(gfx->device_context, VSSetShader, gfx->vertex_shader, 0, 0);
    vcalla(gfx->device_context, PSSetShader, gfx->pixel_shader, 0, 0);

    V2 window_size = gfx->frame_info.real_window_size;
    D3D11_VIEWPORT viewport = { .TopLeftX = 0, .TopLeftY = 0, .Width = window_size.x, .Height = window_size.y, .MinDepth = 0, .MaxDepth = 1 };
    vcalla(gfx->device_context, RSSetViewports, 1, &viewport);
    vcalla(gfx->device_context, OMSetRenderTargets, 1, &gfx->render_target_view, 0);
}

static void gfx_render_end(Gfx_Render_Context *gfx) {
    gfx_update_from_own_bytes(gfx, &gfx->texture);
    V2 window_size = gfx->frame_info.real_window_size;
    Gfx_Rectangle rectangle = { .size = window_size };
    {
        V2 translate = rectangle.position;
        translate = v2_add(translate, v2_scale(rectangle.size, 0.5f)); // centre -> top left
        translate = v2_div(translate, window_size); // 0..window_width -> 0..1
        translate = v2_scale(translate, 2.f); // 0..1 -> 0..2
        translate = v2_sub(translate, (V2){ .x = 1.f, .y = 1.f }); // 0..2 -> -1..1
        translate.y *= -1.f;

        V2 scale = v2_div(rectangle.size, window_size);

        M4 transform = m4_from_translation((V3){ .xy = translate });
        transform.columns[0].x = scale.x;
        transform.columns[1].y = scale.y;

        HRESULT hresult = vcalla(gfx->device_context, Map, (ID3D11Resource *)gfx->texture.transform_buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &gfx->mapped_subresource);
        ensure(hresult == S_OK);
        {
            memcpy(gfx->mapped_subresource.pData, &transform, sizeof(transform));
        }
        vcalla(gfx->device_context, Unmap, (ID3D11Resource *)gfx->texture.transform_buffer, 0);

        vcalla(gfx->device_context, IASetInputLayout, gfx->texture.input_layout);
        vcalla(gfx->device_context, IASetPrimitiveTopology, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
        u32 texture_stride = sizeof(Gfx_Texture_Vertex);
        u32 texture_offset = 0;
        vcalla(gfx->device_context, IASetVertexBuffers, 0, 1, &gfx->texture.vertex_buffer, &texture_stride, &texture_offset);

        vcalla(gfx->device_context, VSSetShader, gfx->texture.vertex_shader, 0, 0);
        vcalla(gfx->device_context, PSSetShader, gfx->texture.pixel_shader, 0, 0);

        vcalla(gfx->device_context, PSSetShaderResources, 0, 1, &gfx->texture.shader_resource_view);
        vcalla(gfx->device_context, PSSetSamplers, 0, 1, &gfx->texture.sampler_state);

        vcalla(gfx->device_context, VSSetConstantBuffers, 0, 1, &gfx->texture.transform_buffer);

        vcalla(gfx->device_context, Draw, 6, 0);
    }

    u32 sync_interval = 1, flags = 0;
    win32_ensure_hr(vcalla(gfx->swapchain, Present, sync_interval, flags));
}

static inline u32 gfx_rgba8_from_v4(V4 v) {
    u32 result =
        ((u32)(v.r * 255.f + 0.5f) << 24) |
        ((u32)(v.g * 255.f + 0.5f) << 16) |
        ((u32)(v.b * 255.f + 0.5f) << 8) |
        ((u32)(v.a * 255.f + 0.5f));
    return result;
}

static void gfx_clear(Gfx_Texture *texture, u32 rgba) {
    // TODO(felix): memcpy once first row is filled
    u32 abgr = gfx_abgr_from_rgba(rgba);
    for (u32 y = 0; y < texture->height; y += 1) {
        u32 *row = (u32 *)texture->bytes.data + y * texture->width;
        for (u32 x = 0; x < texture->width; x += 1) {
            u32 *pixel = row + x;
            *pixel = abgr;
        }
    }
}

static void gfx_draw_line(Gfx_Texture *texture, V2 start, V2 end, f32 thickness, u32 rgba) {
    // Reference: https://en.wikipedia.org/wiki/Bresenham%27s_line_algorithm

    // TODO(felix)
    assert(thickness == 1.f);

    i32 start_x = (i32)(start.x + 0.5f);
    i32 end_x = (i32)(end.x + 0.5f);

    i32 start_y = (i32)(start.y + 0.5f);
    i32 end_y = (i32)(end.y + 0.5f);

    bool is_steep = abs(end_y - start_y) > abs(end_x - start_x);
    if (is_steep) {
        swap(i32, &start_x, &start_y);
        swap(i32, &end_x, &end_y);
    }

    bool right_to_left = start_x > end_x;
    if (right_to_left) {
        swap(i32, &start_x, &end_x);
        swap(i32, &start_y, &end_y);
    }

    i32 distance_x = end_x - start_x;
    i32 distance_y = abs(end_y - start_y);
    i32 error = distance_x / 2;
    i32 y_step = start_y < end_y ? 1 : -1;

    i32 y = start_y;
    for (i32 x = start_x; x < end_x; x += 1) {
        if (is_steep) {
            if (y < 0 || y >= (i32)texture->width) continue;
            if (x < 0 || x >= (i32)texture->height) continue;
            gfx_set_pixel(texture, y, x, rgba);
        } else {
            if (x < 0 || x >= (i32)texture->width) continue;
            if (y < 0 || y >= (i32)texture->height) continue;
            gfx_set_pixel(texture, x, y, rgba);
        }

        error -= distance_y;
        if (error < 0) {
            y += y_step;
            error += distance_x;
        }
    }
}

static inline void gfx_set_pixel(Gfx_Texture *texture, i32 x, i32 y, u32 rgba) {
    // TODO(felix)
    if ((rgba & 0x000000ff) < 0xff) {
        rgba |= 0xff;
    }

    assert(x >= 0);
    assert(x < (i32)texture->width);

    assert(y >= 0);
    assert(y < (i32)texture->height);

    u32 *pixel = &((u32 *)texture->bytes.data)[y * (i32)texture->width + x];
    *pixel = gfx_abgr_from_rgba(rgba);
}

static void gfx_draw_rectangle(Gfx_Texture *texture, i32 top, i32 left, i32 width, i32 height, u32 rgba) {
    i32 bottom = clamp(top + height, 0, (i32)texture->height);
    i32 right = clamp(left + width, 0, (i32)texture->width);
    top = clamp(top, 0, (i32)texture->height - 1);
    left = clamp(left, 0, (i32)texture->width - 1);

    // TODO(felix): can optimise by doing memcpy after first row
    for (i32 y = top; y < bottom; y += 1) {
        for (i32 x = left; x < right; x += 1) {
            gfx_set_pixel(texture, x, y, rgba);
        }
    }
}

static void gfx_draw_text(Gfx_Texture *texture, Gfx_Font font, String string, i32 x, i32 y, u32 rgba) {
    i32 top = y;
    i32 left = x;

    // TODO(felix): make customisable
    i32 tracking = 1;
    i32 step = font.width + tracking;

    assert(string.count > 0);
    for (usize i = 0; i < string.count; i += 1, left += step) {
        u8 character = string.data[i];
        assert(character < 128);

        u8 *glyph = font.glyphs[character];
        for (i32 font_y = 0; font_y < font.height; font_y += 1) {
            y = top + font_y;
            u8 *glyph_row = &glyph[font_y * font.width];

            for (i32 font_x = 0; font_x < font.width; font_x += 1) {
                u8 glyph_value = *(glyph_row + font_x);
                if (glyph_value == 0) continue;

                x = left + font_x;
                gfx_set_pixel(texture, x, y, rgba);
            }
        }
    }
}

static void gfx_update_from_own_bytes(Gfx_Render_Context *gfx, Gfx_Texture *texture) {
    String bytes = texture->bytes;
    HRESULT hresult = vcalla(gfx->device_context, Map, (ID3D11Resource *)texture->texture_2d, 0, D3D11_MAP_WRITE_DISCARD, 0, &gfx->mapped_subresource);
    ensure(hresult == S_OK);
    u8 *destination = gfx->mapped_subresource.pData;
    u32 width_bytes = (u32)gfx->mapped_subresource.RowPitch;
    {
        for (u32 y = 0; y < texture->height; y += 1) {
            memcpy(destination + y * width_bytes, bytes.data + y * width_bytes, width_bytes);
        }
    }
    vcalla(gfx->device_context, Unmap, (ID3D11Resource *)texture->texture_2d, 0);
}

static Gfx_Render_Context *gfx_win32_window_procedure_context__;
static LRESULT gfx_win32_window_procedure(HWND window, u32 message, WPARAM w, LPARAM l) {
    Gfx_Render_Context *gfx = gfx_win32_window_procedure_context__;
    switch (message) {
        case WM_CLOSE: DestroyWindow(window); break;
        case WM_DESTROY: PostQuitMessage(0); break;
        case WM_DISPLAYCHANGE: gfx->frame_info.real_screen_size = gfx_get_real_screen_size(gfx); break;
        case WM_DPICHANGED: {
            f32 dpi_1x = 96.f;
            gfx->frame_info.dpi_scale = (f32)GetDpiForWindow(window) / dpi_1x;
        } break;
        case WM_GETMINMAXINFO: { // TODO(felix): not working. The goal is to not allow the window to become smaller than a certain size
            MINMAXINFO *info = (MINMAXINFO *)l;
            int min_width = 100, min_height = 100;
            info->ptMinTrackSize.x = min_width;
            info->ptMinTrackSize.y = min_height;
        } break;
        case WM_KEYDOWN: {
            gfx->frame_info.key_is_up[w] = false;
            gfx->frame_info.key_is_down[w] = true;
        } break;
        case WM_KEYUP: {
            gfx->frame_info.key_is_down[w] = false;
            gfx->frame_info.key_is_up[w] = true;
        } break;
        case WM_LBUTTONDOWN: gfx->frame_info.mouse_left_clicked = true; gfx->frame_info.mouse_left_down = true; break;
        case WM_LBUTTONUP: gfx->frame_info.mouse_left_clicked = false; gfx->frame_info.mouse_left_down = false; break;
        case WM_MOUSEMOVE: {
            // NOTE(felix): we include windowsx.h just for these two lparam macros. Can we remove that and do something else?
            gfx->frame_info.real_mouse_position.x = (f32)GET_X_LPARAM(l);
            gfx->frame_info.real_mouse_position.y = (f32)GET_Y_LPARAM(l);
        } break;
        case WM_RBUTTONDOWN: gfx->frame_info.mouse_right_clicked = true; gfx->frame_info.mouse_right_down = true; break;
        case WM_RBUTTONUP: gfx->frame_info.mouse_right_clicked = false; gfx->frame_info.mouse_right_down = false; break;
        case WM_SIZE: {
            gfx->frame_info.real_window_size.x = (f32)LOWORD(l);
            gfx->frame_info.real_window_size.y = (f32)HIWORD(l);
        } break;
        case WM_SIZING: {
            // TODO(felix): handle redrawing here so that we're not blocked while resizing
            // NOTE(felix): apparently there might be some problems with this and the better way to do it is for the thread with window events to be different than the application thread that submits draw calls and updates the program
        } break;
    }
    return DefWindowProcA(window, message, w, l);
}

static Gfx_Render_Context gfx_window_create(Arena *arena, char *window_name, u32 width, u32 height) {
    Gfx_Render_Context gfx = {0};
    gfx_win32_window_procedure_context__ = &gfx;

    // TODO(felix): option to have resizable *virtual* window, not just resizable real window
    gfx.frame_info.virtual_window_size = (V2){ .x = (f32)width, .y = (f32)height };

    // window
    {
        WNDCLASSA window_class = {
            .lpfnWndProc = gfx_win32_window_procedure,
            .hCursor = LoadCursorA(0, IDC_ARROW),
            .lpszClassName = window_name,
        };
        ATOM atom_result = RegisterClassA(&window_class);
        ensure(atom_result != 0);

        bool ok = SetProcessDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);
        ensure(ok);

        DWORD extended_style = 0;
        char *class_name = window_name;
        int x = CW_USEDEFAULT, y = CW_USEDEFAULT, window_width = CW_USEDEFAULT, window_height = CW_USEDEFAULT;
        gfx.window_handle = CreateWindowExA(extended_style, class_name, window_name, WS_OVERLAPPEDWINDOW, x, y, window_width, window_height, 0, 0, 0, 0);
        ensure(gfx.window_handle != 0);
    }

    // D3D11 device
    {
        D3D_FEATURE_LEVEL device_feature_levels[] = { D3D_FEATURE_LEVEL_11_0 };
        u32 device_flags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;
        #if BUILD_DEBUG
            device_flags |= D3D11_CREATE_DEVICE_DEBUG;
        #endif
        HRESULT hresult = D3D11CreateDevice(
            0,
            D3D_DRIVER_TYPE_HARDWARE,
            0,
            device_flags,
            device_feature_levels, array_count(device_feature_levels),
            D3D11_SDK_VERSION,
            &gfx.device,
            0,
            &gfx.device_context
        );
        ensure(hresult == S_OK);

        #if BUILD_DEBUG
        {
            ID3D11InfoQueue* info;
            vcalla(gfx.device, QueryInterface, &IID_ID3D11InfoQueue, (void **)&info);
            vcalla(info, SetBreakOnSeverity, D3D11_MESSAGE_SEVERITY_CORRUPTION, true);
            vcalla(info, SetBreakOnSeverity, D3D11_MESSAGE_SEVERITY_ERROR, true);
            vcall(info, Release);

            IDXGIInfoQueue* dxgi_info;
            win32_ensure_hr(DXGIGetDebugInterface1(0, &IID_IDXGIInfoQueue, (void **)&dxgi_info));
            vcalla(dxgi_info, SetBreakOnSeverity, DXGI_DEBUG_ALL, DXGI_INFO_QUEUE_MESSAGE_SEVERITY_CORRUPTION, true);
            vcalla(dxgi_info, SetBreakOnSeverity, DXGI_DEBUG_ALL, DXGI_INFO_QUEUE_MESSAGE_SEVERITY_ERROR, true);
            vcall(dxgi_info, Release);
        }
        #endif
    }

    // swapchain
    {
        IDXGIDevice *dxgi_device = 0;
        HRESULT hresult = vcalla(gfx.device, QueryInterface, &IID_IDXGIDevice, (void **)&dxgi_device);
        ensure(hresult == S_OK);

        IDXGIAdapter *dxgi_adapter = 0;
        hresult = vcalla(dxgi_device, GetAdapter, &dxgi_adapter);
        ensure(hresult == S_OK);

        IDXGIFactory2 *dxgi_factory = 0;
        hresult = vcalla(dxgi_adapter, GetParent, &IID_IDXGIFactory2, (void **)&dxgi_factory);
        ensure(hresult == S_OK);

        DXGI_SWAP_CHAIN_DESC1 swapchain_description = {
            .Format = DXGI_FORMAT_B8G8R8A8_UNORM_SRGB,
            .SampleDesc = { .Count = 1 },
            .BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT,
            .BufferCount = 2,
            .Scaling = DXGI_SCALING_STRETCH,
            .SwapEffect = DXGI_SWAP_EFFECT_DISCARD,
        };
        hresult = vcalla(dxgi_factory, CreateSwapChainForHwnd,
            (IUnknown *)gfx.device, gfx.window_handle, &swapchain_description, 0, 0, &gfx.swapchain
        );
        ensure(hresult == S_OK);

        hresult = vcalla(dxgi_factory, MakeWindowAssociation, gfx.window_handle, DXGI_MWA_NO_ALT_ENTER);
        ensure(hresult == S_OK);

        vcall(dxgi_factory, Release);
        vcall(dxgi_adapter, Release);
        vcall(dxgi_device, Release);
    }

    // shader
    {
        String passthrough_shader = string("\
            struct Vertex_Shader_Input {\
            	float2 pos: Position;\
            	float4 color: Color;\
            };\
            \
            struct Pixel_Shader_Input {\
            	float4 pos: SV_Position;\
            	float4 color: Color;\
            };\
            \
            Pixel_Shader_Input vertex_shader(Vertex_Shader_Input input) {\
            	Pixel_Shader_Input output;\
            	output.pos = float4(input.pos, 0, 1);\
            	output.color = input.color;\
            	return output;\
            }\
            \
            float4 pixel_shader(Pixel_Shader_Input input): SV_Target {\
            	return input.color;\
            }\
        ");

        ID3DBlob *err_blob = 0;
        u32 flags = D3DCOMPILE_PACK_MATRIX_COLUMN_MAJOR | D3DCOMPILE_ENABLE_STRICTNESS | D3DCOMPILE_WARNINGS_ARE_ERRORS;
        #if BUILD_DEBUG
            flags |= D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
        #else
            flags |= D3DCOMPILE_OPTIMIZATION_LEVEL3;
        #endif

        ID3DBlob *vblob = 0, *pblob = 0;
        win32_ensure_d3d_compile(D3DCompile(passthrough_shader.data, passthrough_shader.count, 0, 0, 0, "vertex_shader", "vs_5_0", flags, 0, &vblob, &err_blob), err_blob);
        win32_ensure_d3d_compile(D3DCompile(passthrough_shader.data, passthrough_shader.count, 0, 0, 0, "pixel_shader", "ps_5_0", flags, 0, &pblob, &err_blob), err_blob);
        win32_ensure_hr(vcalla(gfx.device, CreateVertexShader, vcall(vblob, GetBufferPointer), vcall(vblob, GetBufferSize), 0, &gfx.vertex_shader));
        win32_ensure_hr(vcalla(gfx.device, CreatePixelShader, vcall(pblob, GetBufferPointer), vcall(pblob, GetBufferSize), 0, &gfx.pixel_shader));

        D3D11_INPUT_ELEMENT_DESC desc[] = {
            {
                .SemanticName = "POSITION",
                .Format = DXGI_FORMAT_R32G32_FLOAT,
                .AlignedByteOffset = offsetof(Gfx_Vertex, x),
                .InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA,
            }, {
                .SemanticName = "COLOR",
                .Format = DXGI_FORMAT_R32G32B32A32_FLOAT,
                .AlignedByteOffset = offsetof(Gfx_Vertex, r),
                .InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA,
            },
        };
        win32_ensure_hr(vcalla(gfx.device, CreateInputLayout, desc, array_count(desc), vcall(vblob, GetBufferPointer), vcall(vblob, GetBufferSize), &gfx.input_layout));

        vcall(vblob, Release);
        vcall(pblob, Release);
    }

    HRESULT hresult = ShowWindow(gfx.window_handle, SW_SHOWDEFAULT);
    ensure(hresult == S_OK);

    gfx.frame_info.real_screen_size = gfx_get_real_screen_size(&gfx);

    usize pixels_byte_count = width * height * 4;
    u32 *pixels = arena_make(arena, pixels_byte_count, 1);
    String pixel_bytes = { .data = (u8 *)pixels, .count = pixels_byte_count };
    gfx.texture = (Gfx_Texture){ .bytes = pixel_bytes, .width = width, .height = height };
    {
        assert(gfx.texture.bytes.data != 0);
        assert(gfx.texture.bytes.count > 0);
        assert(gfx.texture.width > 0);
        assert(gfx.texture.height > 0);

        D3D11_TEXTURE2D_DESC texture_description = {
            .Width = width, .Height = height,
            .MipLevels = 1,
            .ArraySize = 1,
            .Format = DXGI_FORMAT_B8G8R8A8_UNORM_SRGB,
            .SampleDesc.Count = 1,
            .Usage = D3D11_USAGE_DYNAMIC, // TODO(felix): investigate
            .BindFlags = D3D11_BIND_SHADER_RESOURCE,
            .CPUAccessFlags = D3D11_CPU_ACCESS_WRITE, // TODO(felix): investigate
        };

        D3D11_SUBRESOURCE_DATA subresource_data = {
            .pSysMem = pixel_bytes.data,
            .SysMemPitch = width * 4,
        };

        hresult = vcalla(gfx.device, CreateTexture2D, &texture_description, &subresource_data, &gfx.texture.texture_2d);
        ensure(hresult == S_OK);

        D3D11_SHADER_RESOURCE_VIEW_DESC shader_resource_view_description = {
            .Format = texture_description.Format,
            .ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D,
            .Texture2D.MipLevels = 1,
        };
        hresult = vcalla(gfx.device, CreateShaderResourceView, (ID3D11Resource *)gfx.texture.texture_2d, &shader_resource_view_description, &gfx.texture.shader_resource_view);
        ensure(hresult == S_OK);

        gfx_update_from_own_bytes(&gfx, &gfx.texture);

        String texture_shader = string("\
            struct Vertex_Shader_Input {\
                float2 position: Position;\
                float2 texture_coordinate: Texcoord0;\
            };\
            \
            struct Pixel_Shader_Input {\
                float4 position: SV_Position;\
                float2 texture_coordinate: Texcoord0;\
            };\
            \
            cbuffer transform_buffer: register(b0) {\
                matrix transform;\
            };\
            Pixel_Shader_Input vertex_shader(Vertex_Shader_Input input) {\
                Pixel_Shader_Input output;\
                output.position = mul(transform, float4(input.position, 0, 1));\
                output.texture_coordinate = input.texture_coordinate;\
                return output;\
            }\
            \
            Texture2D my_texture: register(t0);\
            SamplerState my_sampler: register(s0);\
            float4 pixel_shader(Pixel_Shader_Input input): SV_Target {\
                return my_texture.Sample(my_sampler, input.texture_coordinate);\
            }\
        ");

        ID3DBlob *err_blob = 0;
        u32 flags = D3DCOMPILE_PACK_MATRIX_COLUMN_MAJOR | D3DCOMPILE_ENABLE_STRICTNESS | D3DCOMPILE_WARNINGS_ARE_ERRORS;
        #if BUILD_DEBUG
            flags |= D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
        #else
            flags |= D3DCOMPILE_OPTIMIZATION_LEVEL3;
        #endif

        ID3DBlob *vblob = 0, *pblob = 0;
        win32_ensure_d3d_compile(D3DCompile(texture_shader.data, texture_shader.count, 0, 0, 0, "vertex_shader", "vs_5_0", flags, 0, &vblob, &err_blob), err_blob);
        win32_ensure_d3d_compile(D3DCompile(texture_shader.data, texture_shader.count, 0, 0, 0, "pixel_shader", "ps_5_0", flags, 0, &pblob, &err_blob), err_blob);
        win32_ensure_hr(vcalla(gfx.device, CreateVertexShader, vcall(vblob, GetBufferPointer), vcall(vblob, GetBufferSize), 0, &gfx.texture.vertex_shader));
        win32_ensure_hr(vcalla(gfx.device, CreatePixelShader, vcall(pblob, GetBufferPointer), vcall(pblob, GetBufferSize), 0, &gfx.texture.pixel_shader));

        D3D11_INPUT_ELEMENT_DESC input_element_description[] = {
            {
                .SemanticName = "POSITION",
                .Format = DXGI_FORMAT_R32G32_FLOAT,
                .AlignedByteOffset = offsetof(Gfx_Texture_Vertex, position),
                .InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA,
            }, {
                .SemanticName = "TEXCOORD",
                .Format = DXGI_FORMAT_R32G32_FLOAT,
                .AlignedByteOffset = offsetof(Gfx_Texture_Vertex, texture_coordinate),
                .InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA,
            },
        };
        hresult = vcalla(gfx.device, CreateInputLayout,
            input_element_description,
            array_count(input_element_description),
            vcall(vblob, GetBufferPointer),
            vcall(vblob, GetBufferSize),
            &gfx.texture.input_layout
        );
        ensure(hresult == S_OK);

        vcall(vblob, Release);
        vcall(pblob, Release);

        D3D11_SAMPLER_DESC sampler_description = {
            .Filter = D3D11_FILTER_MIN_MAG_MIP_POINT,
            .AddressU = D3D11_TEXTURE_ADDRESS_CLAMP,
            .AddressV = D3D11_TEXTURE_ADDRESS_CLAMP,
            .AddressW = D3D11_TEXTURE_ADDRESS_CLAMP,
            .ComparisonFunc = D3D11_COMPARISON_NEVER,
            .MinLOD = 0,
            .MaxLOD = D3D11_FLOAT32_MAX,
        };
        hresult = vcalla(gfx.device, CreateSamplerState, &sampler_description, &gfx.texture.sampler_state);
        ensure(hresult == S_OK);

        f32 extent = 1.f;
        Gfx_Texture_Vertex texture_vertices[] = {
            // triangle 1
            { .position = { .x = -extent, .y = -extent }, .texture_coordinate = { .x = 0.f, .y = 1.f } },
            { .position = { .x = -extent, .y =  extent }, .texture_coordinate = { .x = 0.f, .y = 0.f } },
            { .position = { .x =  extent, .y = -extent }, .texture_coordinate = { .x = 1.f, .y = 1.f } },
            // triangle 2,
            { .position = { .x = -extent, .y =  extent }, .texture_coordinate = { .x = 0.f, .y = 0.f } },
            { .position = { .x =  extent, .y =  extent }, .texture_coordinate = { .x = 1.f, .y = 0.f } },
            { .position = { .x =  extent, .y = -extent }, .texture_coordinate = { .x = 1.f, .y = 1.f } },
        };
        D3D11_BUFFER_DESC vertex_buffer_description = {
            .ByteWidth = sizeof(texture_vertices),
            .Usage = D3D11_USAGE_DEFAULT,
            .BindFlags = D3D11_BIND_VERTEX_BUFFER,
        };
        D3D11_SUBRESOURCE_DATA vertex_data = { .pSysMem = texture_vertices };
        hresult = vcalla(gfx.device, CreateBuffer, &vertex_buffer_description, &vertex_data, &gfx.texture.vertex_buffer);
        ensure(hresult == S_OK);

        // transform
        {
            D3D11_BUFFER_DESC transform_buffer_description = {
                .ByteWidth = sizeof(M4),
                .Usage = D3D11_USAGE_DYNAMIC,
                .BindFlags = D3D11_BIND_CONSTANT_BUFFER,
                .CPUAccessFlags = D3D11_CPU_ACCESS_WRITE,
            };
            hresult = vcalla(gfx.device, CreateBuffer, &transform_buffer_description, 0, &gfx.texture.transform_buffer);
            ensure(hresult == S_OK);
        }
    }

    return gfx;
}

static bool gfx_window_should_close(Gfx_Render_Context *gfx) {
    memcpy(gfx->frame_info.key_was_down_last_frame, gfx->frame_info.key_is_down, sizeof(gfx->frame_info.key_is_down));
    memcpy(gfx->frame_info.key_was_up_last_frame, gfx->frame_info.key_is_up, sizeof(gfx->frame_info.key_is_up));
    gfx->frame_info.mouse_left_clicked = false;
    gfx->frame_info.mouse_right_clicked = false;

    V2 window_size = gfx->frame_info.real_window_size;
    V2 prev_window_size = window_size;

    gfx_win32_window_procedure_context__ = gfx;
    for (MSG message = {0}; PeekMessageA(&message, 0, 0, 0, PM_REMOVE);) {
        if (message.message == WM_QUIT) return true;
        TranslateMessage(&message);
        DispatchMessageA(&message);
    }

    bool render_target_view_exists = (gfx->render_target_view != 0);
    bool should_resize_render_target_view = !v2_equal(prev_window_size, window_size) || !render_target_view_exists;
    if (should_resize_render_target_view) {
        if (render_target_view_exists) {
            vcall(gfx->device_context, ClearState);
            ensure_released_and_null(&gfx->render_target_view);
        }

        win32_ensure_hr(vcalla(gfx->swapchain, ResizeBuffers, 0, 0, 0, DXGI_FORMAT_UNKNOWN, 0));

        ID3D11Texture2D *backbuffer = 0;
        win32_ensure_hr(vcalla(gfx->swapchain, GetBuffer, 0, &IID_ID3D11Texture2D, (void **)&backbuffer));
        win32_ensure_hr(vcalla(gfx->device, CreateRenderTargetView, (ID3D11Resource *)backbuffer, 0, &gfx->render_target_view));
        release(backbuffer);
    }

    return false;
}
