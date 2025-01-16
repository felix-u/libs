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

structdef(Gfx_Render_Ctx) {
    HWND window_handle;

    ID3D11Device           *device;
    ID3D11DeviceContext    *device_ctx;
    ID3D11RenderTargetView *render_target_view;
    IDXGISwapChain1        *swapchain;
    ID3D11InputLayout      *input_layout;
    ID3D11VertexShader     *vertex_shader;
    ID3D11PixelShader      *pixel_shader;
    ID3D11Buffer           *vbuf;

    ID2D1Factory         *d2_factory;
    IDWriteFactory       *dw_factory;
    ID2D1RenderTarget    *d2_render_target;
    IDWriteTextFormat    *dw_text_fmt;
    ID2D1SolidColorBrush *d2_brush;
    IDWriteTextLayout    *dw_text_layout;
    ID2D1StrokeStyle     *d2_stroke_style;
};

// TODO(felix): this belongs somewhere else
structdef(Vertex) { f32 x, y, r, g, b, a; };

#define release(obj) { vcall((IUnknown *)(obj), Release); }
#define ensure_released_and_null(obj) { if (*(obj) != 0) { release(*(obj)); *(obj) = 0; } }
#define vcall(struct_ptr, fn_name) (struct_ptr)->lpVtbl->fn_name(struct_ptr)
#define vcalla(struct_ptr, fn_name, ...) (struct_ptr)->lpVtbl->fn_name((struct_ptr), __VA_ARGS__)

static LRESULT win32_window_proc(HWND window, u32 message, WPARAM w, LPARAM l);
