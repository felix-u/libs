// TODO(felix): is vsync enabled by default? if not, let's enable it
// TODO(felix): get delta time

// TODO(felix): these should be in a struct
static f32 screen_width, screen_height;
static f32 mouse_x, mouse_y;
static bool mouse_left_down, mouse_right_down;
static bool mouse_left_clicked, mouse_right_clicked;
static void input_reset(void) {
    mouse_left_clicked = false;
    mouse_right_clicked = false;
}

static D3D_Device_Info d3d_device_create(void) {
    D3D_Device_Info d3d = {0};

    D3D_FEATURE_LEVEL device_feature_levels[] = { D3D_FEATURE_LEVEL_11_0 };
    int device_flags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;
    #if BUILD_DEBUG
        device_flags |= D3D11_CREATE_DEVICE_DEBUG;
    #endif
    win32_assert_hr(D3D11CreateDevice(
        0,
        D3D_DRIVER_TYPE_HARDWARE,
        0,
        device_flags,
        device_feature_levels, array_count(device_feature_levels),
        D3D11_SDK_VERSION,
        &d3d.device,
        0,
        &d3d.device_ctx
    ));

    #if BUILD_DEBUG
    {
        ID3D11InfoQueue* info;
        vcalla(d3d.device, QueryInterface, &IID_ID3D11InfoQueue, (void **)&info);
        vcalla(info, SetBreakOnSeverity, D3D11_MESSAGE_SEVERITY_CORRUPTION, true);
        vcalla(info, SetBreakOnSeverity, D3D11_MESSAGE_SEVERITY_ERROR, true);
        vcall(info, Release);

        IDXGIInfoQueue* dxgi_info;
        win32_assert_hr(DXGIGetDebugInterface1(0, &IID_IDXGIInfoQueue, (void **)&dxgi_info));
        vcalla(dxgi_info, SetBreakOnSeverity, DXGI_DEBUG_ALL, DXGI_INFO_QUEUE_MESSAGE_SEVERITY_CORRUPTION, true);
        vcalla(dxgi_info, SetBreakOnSeverity, DXGI_DEBUG_ALL, DXGI_INFO_QUEUE_MESSAGE_SEVERITY_ERROR, true);
        vcall(dxgi_info, Release);
    }
    #endif

    return d3d;
}

static void d3d_render_target_view_resize(D3D_Device_Info *d3d, IDXGISwapChain1 **swapchain, ID3D11RenderTargetView **render_target_view, Dwrite_Ctx *dw_ctx) {
    if (render_target_view != 0) {
        vcall(d3d->device_ctx, ClearState);
        ensure_released_and_null(render_target_view);
    }

    ensure_released_and_null(&dw_ctx->brush);
    ensure_released_and_null(&dw_ctx->render_target);

    win32_assert_hr(vcalla(*swapchain, ResizeBuffers, 0, 0, 0, DXGI_FORMAT_UNKNOWN, 0));

    ID3D11Texture2D *backbuffer = 0;
    win32_assert_hr(vcalla(*swapchain, GetBuffer, 0, &IID_ID3D11Texture2D, (void **)&backbuffer));
    win32_assert_hr(vcalla(d3d->device, CreateRenderTargetView, (ID3D11Resource *)backbuffer, 0, render_target_view));

    IDXGISurface *surface = 0;
    vcalla(backbuffer, QueryInterface, &IID_IDXGISurface, (void **)&surface);

    {
        D2D1_RENDER_TARGET_PROPERTIES properties = {
            .type = D2D1_RENDER_TARGET_TYPE_DEFAULT,
            .pixelFormat = { .format = DXGI_FORMAT_B8G8R8A8_UNORM_SRGB, .alphaMode = D2D1_ALPHA_MODE_IGNORE },
            .dpiX = 0, .dpiY = 0,
            .usage = D2D1_RENDER_TARGET_USAGE_NONE,
            .minLevel = D2D1_FEATURE_LEVEL_DEFAULT,
        };
        win32_assert_hr(ID2D1Factory_CreateDxgiSurfaceRenderTarget(dw_ctx->d2_factory, surface, &properties, &dw_ctx->render_target));
    }

    D2D_COLOR_F draw_colour = { .r = 0, .g = 0, .b = 0, .a = 1.f };
    win32_assert_hr(ID2D1RenderTarget_CreateSolidColorBrush(dw_ctx->render_target, &draw_colour, 0, &dw_ctx->brush));

    release(surface);
    release(backbuffer);
}

static D3D_Shader_Info d3d_shader_compile(ID3D11Device *device, Str8 hlsl) {
    D3D_Shader_Info shader = {0};

    ID3DBlob *err_blob = 0;
    int flags = D3DCOMPILE_PACK_MATRIX_COLUMN_MAJOR | D3DCOMPILE_ENABLE_STRICTNESS | D3DCOMPILE_WARNINGS_ARE_ERRORS;
    #if BUILD_DEBUG
        flags |= D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
    #else
        flags |= D3DCOMPILE_OPTIMIZATION_LEVEL3;
    #endif

    ID3DBlob *vblob = 0, *pblob = 0;
    win32_assert_d3d_compile(D3DCompile(hlsl.ptr, hlsl.len, 0, 0, 0, "vs", "vs_5_0", flags, 0, &vblob, &err_blob), err_blob);
    win32_assert_d3d_compile(D3DCompile(hlsl.ptr, hlsl.len, 0, 0, 0, "ps", "ps_5_0", flags, 0, &pblob, &err_blob), err_blob);
    win32_assert_hr(vcalla(device, CreateVertexShader, vcall(vblob, GetBufferPointer), vcall(vblob, GetBufferSize), 0, &shader.vertex));
    win32_assert_hr(vcalla(device, CreatePixelShader, vcall(pblob, GetBufferPointer), vcall(pblob, GetBufferSize), 0, &shader.pixel));

    D3D11_INPUT_ELEMENT_DESC desc[] = {
        {
            .SemanticName = "POSITION",
            .Format = DXGI_FORMAT_R32G32_FLOAT,
            .AlignedByteOffset = offsetof(Vertex, x),
            .InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA,
        }, {
            .SemanticName = "COLOR",
            .Format = DXGI_FORMAT_R32G32B32A32_FLOAT,
            .AlignedByteOffset = offsetof(Vertex, r),
            .InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA,
        },
    };
    win32_assert_hr(vcalla(device, CreateInputLayout, desc, array_count(desc), vcall(vblob, GetBufferPointer), vcall(vblob, GetBufferSize), &shader.layout));

    vcall(vblob, Release);
    vcall(pblob, Release);

    return shader;
}

static ID3D11Buffer *d3d_vbuf_create(ID3D11Device *device, usize num_vertices, usize sizeof_vertex) {
    D3D11_BUFFER_DESC desc = {
        .ByteWidth = (int)(num_vertices * sizeof_vertex),
        .Usage = D3D11_USAGE_DYNAMIC,
        .BindFlags = D3D11_BIND_VERTEX_BUFFER,
        .CPUAccessFlags = D3D11_CPU_ACCESS_WRITE,
    };
    ID3D11Buffer *vbuf = 0;
    win32_assert_hr(vcalla(device, CreateBuffer, &desc, 0, &vbuf));
    return vbuf;
}

static Dwrite_Ctx dw_init(void) {
    Dwrite_Ctx ctx = {0};
    D2D1_FACTORY_OPTIONS options = { .debugLevel = D2D1_DEBUG_LEVEL_NONE };
    win32_assert_hr(D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &IID_ID2D1Factory, &options, (void **)&ctx.d2_factory));
    win32_assert_hr(DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, &IID_IDWriteFactory, (void **)&ctx.factory));

    // NOTE(felix): for DirectWrite we use Martins' cd2d.h and cdwrite.h, which don't seem to expose vtables, so we use MS's COBJMACROS instead of our vcall/vcalla macros

    {
        u16 *system_font = L"";
        f32 font_size = 16.f;

        f32 dpi_scaling_1x = 96.f;
        // TODO(felix): figure out if we should be using per-monitor scale instead of GetDpiForSystem()
        f32 scale_factor = (f32)GetDpiForSystem() / dpi_scaling_1x;
        win32_assert_hr(IDWriteFactory_CreateTextFormat(ctx.factory,
            system_font, 0,
            DWRITE_FONT_WEIGHT_NORMAL, DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL,
            font_size * scale_factor, L"en-us", &ctx.text_fmt
        ));
    }

    // TODO(felix): add parameter to draw_text() for the following alignments:
    // DWRITE_TEXT_ALIGNMENT_LEADING = left
    // DWRITE_TEXT_ALIGNMENT_TRAILING = right
    // DWRITE_TEXT_ALIGNMENT_CENTER = centre
    // DWRITE_TEXT_ALIGNMENT_JUSTIFIED = spread_to_fill (???)
    // TODO(felix): maybe also add parameters for paragraph alignment and word wrapping

    win32_assert_hr(IDWriteTextFormat_SetTextAlignment(ctx.text_fmt, DWRITE_TEXT_ALIGNMENT_LEADING));
    win32_assert_hr(IDWriteTextFormat_SetParagraphAlignment(ctx.text_fmt, DWRITE_PARAGRAPH_ALIGNMENT_CENTER));
    win32_assert_hr(IDWriteTextFormat_SetWordWrapping(ctx.text_fmt, DWRITE_WORD_WRAPPING_NO_WRAP));

    win32_assert_hr(ID2D1Factory_CreateStrokeStyle(ctx.d2_factory, &(D2D1_STROKE_STYLE_PROPERTIES){
        .startCap = D2D1_CAP_STYLE_FLAT,
        .endCap = D2D1_CAP_STYLE_FLAT,
        .dashCap = D2D1_CAP_STYLE_FLAT,
        .lineJoin = D2D1_LINE_JOIN_MITER,
        .miterLimit = 1.f,
        .dashStyle = D2D1_DASH_STYLE_SOLID,
        .dashOffset = 0,
    }, 0, 0, &ctx.stroke_style));

    return ctx;
}

static IDXGISwapChain1 *dx_swapchain_create(HWND window_handle, ID3D11Device *device) {
    IDXGIDevice *dxgi_device = 0;
    win32_assert_hr(vcalla(device, QueryInterface, &IID_IDXGIDevice, (void **)&dxgi_device));

    IDXGIAdapter *dxgi_adapter = 0;
    win32_assert_hr(vcalla(dxgi_device, GetAdapter, &dxgi_adapter));

    IDXGIFactory2 *factory = 0;
    win32_assert_hr(vcalla(dxgi_adapter, GetParent, &IID_IDXGIFactory2, (void **)&factory));

    DXGI_SWAP_CHAIN_DESC1 desc = {
        .Format = DXGI_FORMAT_B8G8R8A8_UNORM_SRGB,
        .SampleDesc = { .Count = 1 },
        .BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT,
        .BufferCount = 2,
        .Scaling = DXGI_SCALING_STRETCH,
        .SwapEffect = DXGI_SWAP_EFFECT_DISCARD,
    };
    IDXGISwapChain1 *swapchain = 0;
    win32_assert_hr(vcalla(factory, CreateSwapChainForHwnd, (IUnknown *)device, window_handle, &desc, 0, 0, &swapchain));

    win32_assert_hr(vcalla(factory, MakeWindowAssociation, window_handle, DXGI_MWA_NO_ALT_ENTER));

    vcall(factory, Release);
    vcall(dxgi_adapter, Release);
    vcall(dxgi_device, Release);

    return swapchain;
}

static void gfx_draw_rounded_rect(Dwrite_Ctx *dw_ctx, Gfx_Rounded_Rect rounded_rect) {
    D2D1_RECT_F d2_layout_rect = *(D2D1_RECT_F *)(&rounded_rect.rect);
    D2D1_ROUNDED_RECT d2_rounded_rect = {
        .rect = d2_layout_rect,
        .radiusX = rounded_rect.corner_radius, .radiusY = rounded_rect.corner_radius,
    };

    // TODO(felix): cache this!
    ID2D1SolidColorBrush *bg_brush = 0;
    win32_assert_hr(ID2D1RenderTarget_CreateSolidColorBrush(dw_ctx->render_target, (D2D1_COLOR_F *)&rounded_rect.colour, 0, &bg_brush));
    ID2D1RenderTarget_FillRoundedRectangle(dw_ctx->render_target, &d2_rounded_rect, (ID2D1Brush *)bg_brush);

    // TODO(felix): cache this!
    ID2D1SolidColorBrush *border_brush = 0;
    win32_assert_hr(ID2D1RenderTarget_CreateSolidColorBrush(dw_ctx->render_target, (D2D_COLOR_F *)&rounded_rect.border_colour, 0, &border_brush));

    ID2D1RenderTarget_DrawRoundedRectangle(dw_ctx->render_target,
        &d2_rounded_rect, (ID2D1Brush *)border_brush, rounded_rect.border_thickness, dw_ctx->stroke_style
    );

    ensure_released_and_null(&border_brush);
    ensure_released_and_null(&bg_brush);
}

static void gfx_draw_text(Arena *arena, Dwrite_Ctx *dw_ctx, Str8 str, V4 rect) {
    D2D1_RECT_F layout_rect = *(D2D1_RECT_F *)(&rect);
    Str16 wstr = str16_from_str8(arena, str);
    ID2D1RenderTarget_DrawText(dw_ctx->render_target,
        wstr.ptr, (u32)wstr.len, dw_ctx->text_fmt, &layout_rect,
        (ID2D1Brush *)dw_ctx->brush, D2D1_DRAW_TEXT_OPTIONS_ENABLE_COLOR_FONT, DWRITE_MEASURING_MODE_NATURAL
    );
}

static V2 gfx_str_dimensions(Arena *arena, Dwrite_Ctx *dw_ctx, Str8 str) {
    Str16 wstr = str16_from_str8(arena, str);
    // TODO(felix): cache this! we want to call these functions as little as possible
    IDWriteFactory_CreateTextLayout(dw_ctx->factory, wstr.ptr, (u32)wstr.len, dw_ctx->text_fmt, screen_width, screen_height, &dw_ctx->text_layout);
    DWRITE_TEXT_METRICS metrics = {0};
    win32_assert_hr(IDWriteTextLayout_GetMetrics(dw_ctx->text_layout, &metrics));
    ensure_released_and_null(&dw_ctx->text_layout);
    return v2(metrics.width, metrics.height);
}

static HWND win32_window_create(char *window_name) {
    WNDCLASSA window_class = {
        .lpfnWndProc = win32_window_proc,
        .hCursor = LoadCursorA(0, IDC_ARROW),
        .lpszClassName = window_name,
    };
    win32_assert_not_0(RegisterClassA(&window_class));

    win32_assert_not_0(SetProcessDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2));

    DWORD extended_style = 0;
    char *class_name = window_name;
    int x = CW_USEDEFAULT, y = CW_USEDEFAULT, width = CW_USEDEFAULT, height = CW_USEDEFAULT;

    HWND window_handle = CreateWindowExA(extended_style, class_name, window_name, WS_OVERLAPPEDWINDOW, x, y, width, height, 0, 0, 0, 0);
    win32_assert_not_0(window_handle);
    return window_handle;
}

static bool win32_window_show(void) {
    bool stay_running = true;
    for (MSG message = {0}; PeekMessageA(&message, 0, 0, 0, PM_REMOVE);) {
        if (message.message == WM_QUIT) stay_running = false;
        TranslateMessage(&message);
        DispatchMessageA(&message);
    }
    return stay_running;
}

static LRESULT win32_window_proc(HWND window, u32 message, WPARAM w, LPARAM l) {
    switch (message) {
        case WM_CLOSE: DestroyWindow(window); break;
        case WM_DESTROY: PostQuitMessage(0); break;
        case WM_GETMINMAXINFO: { // FIXME(felix): not working. The goal is to not allow the window to become smaller than a certain size
            MINMAXINFO *info = (MINMAXINFO *)l;
            int min_width = 100, min_height = 100;
            info->ptMinTrackSize.x = min_width;
            info->ptMinTrackSize.y = min_height;
        } break;
        case WM_LBUTTONDOWN: mouse_left_clicked = true; mouse_left_down = true; break;
        case WM_LBUTTONUP: mouse_left_clicked = false; mouse_left_down = false; break;
        case WM_MOUSEMOVE: {
            // NOTE(felix): we include windowsx.h just for these two lparam macros. Can we remove that and do something else?
            mouse_x = (f32)GET_X_LPARAM(l);
            mouse_y = (f32)GET_Y_LPARAM(l);
        } break;
        case WM_RBUTTONDOWN: mouse_right_clicked = true; mouse_right_down = true; break;
        case WM_RBUTTONUP: mouse_right_clicked = false; mouse_right_down = false; break;
        case WM_SIZE: {
            screen_width = (f32)LOWORD(l);
            screen_height = (f32)HIWORD(l);
        } break;
        case WM_SIZING: {
            // TODO(felix): handle redrawing here so that we're not blocked while resizing
            // NOTE(felix): apparently there might be some problems with this and the better way to do it is for the thread with window events to be different than the application thread that submits draw calls and updates the program
        } break;
    }
    return DefWindowProcA(window, message, w, l);
}

// NOTE(felix): was previously after target view resizing in main loop
// Minimal usage of D3D11 with D2D. Draws 3 coloured squares
/* {
    D3D11_MAPPED_SUBRESOURCE mapped = {0};
    win32_assert_hr(vcalla(device_ctx, Map, (ID3D11Resource *)vbuf, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped));
    Vertex *vertex = mapped.pData;

    f32 w = (f32)width, h = (f32)height;
    f32 qsize = 400;
    f32 pos[][2] = { { 100.f, 300.f }, { 300.f, 500.f }, { 600.f, 300.f } };
    f32 col[][4] = { { 1.f, 0, 0, 1.f }, { 0, 1.f, 0, 1.f }, { 0, 0, 1.f, 1.f } };
    f32 corner[][2] = { { 0, 0 }, { 0, 1.f }, { 1.f, 1.f }, { 1.f, 0 } };

    for (usize q = 0; q < 3; q += 1) {
        f32 ax = (pos[q][0] + qsize * corner[0][0]) / w * 2.f - 1.f;
        f32 ay = (pos[q][1] + qsize * corner[0][1]) / h * 2.f - 1.f;
        f32 bx = (pos[q][0] + qsize * corner[1][0]) / w * 2.f - 1.f;
        f32 by = (pos[q][1] + qsize * corner[1][1]) / h * 2.f - 1.f;
        f32 cx = (pos[q][0] + qsize * corner[2][0]) / w * 2.f - 1.f;
        f32 cy = (pos[q][1] + qsize * corner[2][1]) / h * 2.f - 1.f;
        f32 dx = (pos[q][0] + qsize * corner[3][0]) / w * 2.f - 1.f;
        f32 dy = (pos[q][1] + qsize * corner[3][1]) / h * 2.f - 1.f;

        vertex[6 * q + 0] = (Vertex){ .x = ax, .y = -ay, .r = col[q][0], .g = col[q][1], .b = col[q][2], .a = col[q][3] };
        vertex[6 * q + 1] = (Vertex){ .x = cx, .y = -cy, .r = col[q][0], .g = col[q][1], .b = col[q][2], .a = col[q][3] };
        vertex[6 * q + 2] = (Vertex){ .x = bx, .y = -by, .r = col[q][0], .g = col[q][1], .b = col[q][2], .a = col[q][3] };
        vertex[6 * q + 3] = (Vertex){ .x = cx, .y = -cy, .r = col[q][0], .g = col[q][1], .b = col[q][2], .a = col[q][3] };
        vertex[6 * q + 4] = (Vertex){ .x = ax, .y = -ay, .r = col[q][0], .g = col[q][1], .b = col[q][2], .a = col[q][3] };
        vertex[6 * q + 5] = (Vertex){ .x = dx, .y = -dy, .r = col[q][0], .g = col[q][1], .b = col[q][2], .a = col[q][3] };
    }
    vcalla(device_ctx, Unmap, (ID3D11Resource *)vbuf, 0);
} */
