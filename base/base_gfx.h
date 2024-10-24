#if OS_WINDOWS
    // link: user32, d3d11, dxguid, dxgi, d3dcompiler
    #include <initguid.h>
    #include <d3d11.h>
    #include <d3dcompiler.h>
    #include <dxgi1_3.h>

    #include <windowsx.h>

    #pragma warning(push)
    #pragma warning(disable: 4005 4200 4616)
    #include "win32/cd2d.h"
    #include "win32/cdwrite.h"
    #pragma warning(pop)

    #if BUILD_DEBUG
        #include <dxgidebug.h>
    #endif
#else
    #error "OS not yet supported"
#endif

structdef(D3D_Device_Info) {
    ID3D11Device *device;
    ID3D11DeviceContext *device_ctx;
};

structdef(D3D_Shader_Info) {
    ID3D11InputLayout *layout;
    ID3D11VertexShader *vertex;
    ID3D11PixelShader *pixel;
};

structdef(Dwrite_Ctx) {
    ID2D1Factory         *d2_factory;
    IDWriteFactory       *factory;
    ID2D1RenderTarget    *render_target;
    IDWriteTextFormat    *text_fmt;
    ID2D1SolidColorBrush *brush;
    IDWriteTextLayout    *text_layout;
    ID2D1StrokeStyle     *stroke_style;
};

structdef(Gfx_Render_Ctx) {
    D3D_Device_Info d3d;
    Dwrite_Ctx dw_ctx;
    ID3D11RenderTargetView *render_target_view;
    IDXGISwapChain1 *swapchain;
};

// TODO(felix): which of these needs to take into account dpi? update relevant functions
structdef(Gfx_Rounded_Rect) {
    V4 colour;
    f32 corner_radius, border_thickness;
    V4 border_colour;
    V4 rect;
};

structdef(Vertex) { f32 x, y, r, g, b, a; };

#define release(obj) { vcall((IUnknown *)(obj), Release); }
#define ensure_released_and_null(obj) { if (*(obj) != 0) { release(*(obj)); *(obj) = 0; } }
#define vcall(struct_ptr, fn_name) (struct_ptr)->lpVtbl->fn_name(struct_ptr)
#define vcalla(struct_ptr, fn_name, ...) (struct_ptr)->lpVtbl->fn_name((struct_ptr), __VA_ARGS__)

static D3D_Device_Info  d3d_device_create(void);
static            void  d3d_render_target_view_resize(Gfx_Render_Ctx *ctx);
static D3D_Shader_Info  d3d_shader_compile(ID3D11Device *device, Str8 hlsl);
static    ID3D11Buffer *d3d_vbuf_create(ID3D11Device *device, usize num_vertices, usize sizeof_vertex);

static Dwrite_Ctx dw_init(void);

static IDXGISwapChain1 *dx_swapchain_create(HWND window_handle, ID3D11Device *device);

static void gfx_draw_rounded_rect(Dwrite_Ctx *dw_ctx, Gfx_Rounded_Rect rounded_rect);
static void gfx_draw_text(Arena *arena, Dwrite_Ctx *dw_ctx, Str8 str, V4 rect);
static   V2 gfx_str_dimensions(Arena *arena, Dwrite_Ctx *dw_ctx, Str8 str);

static    HWND win32_window_create(char *window_name);
static    bool win32_window_show(Gfx_Render_Ctx *ctx);
static LRESULT win32_window_proc(HWND window, u32 message, WPARAM w, LPARAM l);
