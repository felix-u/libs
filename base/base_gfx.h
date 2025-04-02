#if OS_WINDOWS
    // link: user32, d3d11, dxguid, dxgi, d3dcompiler
    #include <initguid.h>
    #include <d3d11.h>
    #include <d3dcompiler.h>
    #include <dxgi1_3.h>

    #include <windowsx.h>

    // TODO(felix): remove
    // #pragma warning(push)
    // #pragma warning(disable: 4005 4200 4616)
    // #include "win32/cd2d.h"
    // #include "win32/cdwrite.h"
    // #pragma warning(pop)

    #if BUILD_DEBUG
        #include <dxgidebug.h>
    #endif
#else
    #error "OS not yet supported"
#endif

#define gfx_max_font_width 8
#define gfx_max_font_height 16
structdef(Gfx_Font) {
    u8 width, height;
    u8 glyphs[gfx_max_font_width * gfx_max_font_height][128]; // NOTE(felix): worth looking into whether we want to support > 128. IBM codepage 437 might be cool!
};

enumdef(Gfx_Key, u8) {
    gfx_key_shift                = VK_SHIFT,
    gfx_key_control              = VK_CONTROL,
    gfx_key_space                = VK_SPACE,
    gfx_key_left                 = VK_LEFT,
    gfx_key_down                 = VK_DOWN,
    gfx_key_up                   = VK_UP,
    gfx_key_right                = VK_RIGHT,
    gfx_key_a                    = 'A',
    gfx_key_b                    = 'B',
    gfx_key_c                    = 'C',
    gfx_key_d                    = 'D',
    gfx_key_s                    = 'S',
    gfx_key_w                    = 'W',
    gfx_key_f1                   = VK_F1,
    gfx_key_f2                   = VK_F2,
    gfx_key_f3                   = VK_F3,
    gfx_key_f4                   = VK_F4,
    gfx_key_f5                   = VK_F5,
    gfx_key_f6                   = VK_F6,
    gfx_key_f7                   = VK_F7,
    gfx_key_f8                   = VK_F8,
    gfx_key_f9                   = VK_F9,
    gfx_key_f10                  = VK_F10,
    gfx_key_comma                = VK_OEM_COMMA,
    gfx_key_dot                  = VK_OEM_PERIOD,
    gfx_key_left_square_bracket  = VK_OEM_4,
    gfx_key_right_square_bracket = VK_OEM_6,
    gfx_key_count,
};

structdef(Gfx_Frame_Info) {
    V2 real_window_size;
    V2 real_screen_size;
    HMONITOR monitor_handle;
    V2 virtual_window_size;
    f32 dpi_scale;
    V2 real_mouse_position;
    bool mouse_left_down, mouse_right_down;
    bool mouse_left_clicked, mouse_right_clicked;
    Gfx_Key key_is_down[256];
    Gfx_Key key_is_up[256];
    Gfx_Key key_was_down_last_frame[256];
    Gfx_Key key_was_up_last_frame[256];
    f32 seconds;
};

structdef(Gfx_Texture) {
    String bytes;
    u32 width, height;
    ID3D11InputLayout        *input_layout;
    ID3D11SamplerState       *sampler_state;
    ID3D11Buffer             *vertex_buffer;
    ID3D11VertexShader       *vertex_shader;
    ID3D11PixelShader        *pixel_shader;
    ID3D11ShaderResourceView *shader_resource_view;
    ID3D11Buffer             *transform_buffer;
    ID3D11Texture2D          *texture_2d;
};

structdef(Gfx_Render_Context) {
    HWND window_handle;

    ID3D11Device           *device;
    ID3D11DeviceContext    *device_context;
    IDXGISwapChain1        *swapchain;
    ID3D11Buffer           *vertex_buffer;
    ID3D11VertexShader     *vertex_shader;
    ID3D11PixelShader      *pixel_shader;
    ID3D11InputLayout      *input_layout;
    ID3D11RenderTargetView *render_target_view;
    D3D11_MAPPED_SUBRESOURCE mapped_subresource;

    Gfx_Texture texture;
    Gfx_Font font;

    Gfx_Frame_Info frame_info;
};

uniondef(Gfx_Rectangle) {
    struct { V2 position, size; };
    struct { f32 x, y, width, height; };
};

structdef(Gfx_Texture_Vertex) { V2 position, texture_coordinate; };

// TODO(felix): remove or do something with this
structdef(Gfx_Vertex) { f32 x, y, r, g, b, a; };

#define release(obj) vcall((IUnknown *)(obj), Release);
#define ensure_released_and_null(obj) statement_macro( if (*(obj) != 0) { release(*(obj)); *(obj) = 0; } )
#define vcall(struct_ptr, fn_name) (struct_ptr)->lpVtbl->fn_name(struct_ptr)
#define vcalla(struct_ptr, fn_name, ...) (struct_ptr)->lpVtbl->fn_name((struct_ptr), __VA_ARGS__)

#define gfx_key_is_down(gfx, key) (gfx)->frame_info.key_is_down[key]
#define gfx_key_is_up(gfx, key) (gfx)->frame_info.key_is_up[key]
#define gfx_key_is_pressed(gfx, key) (gfx_key_is_down(gfx, key) && !(gfx)->frame_info.key_was_down_last_frame[key])

static                 V2 gfx_get_real_screen_size(Gfx_Render_Context *gfx);
static               void gfx_render_begin(Gfx_Render_Context *gfx);
static               void gfx_render_end(Gfx_Render_Context *gfx);
static inline         u32 gfx_rgba8_from_v4(V4 v);
static               void gfx_clear(Gfx_Texture *texture, u32 rgba);
static               void gfx_draw_line(Gfx_Texture *texture, V2 start_position, V2 end_position, f32 thickness, u32 rgba);
static               void gfx_draw_rectangle(Gfx_Texture *texture, i32 top, i32 left, i32 width, i32 height, u32 rgba);
static               void gfx_draw_text(Gfx_Texture *texture, Gfx_Font font, String string, i32 x, i32 y, u32 rgba);
static inline        void gfx_set_pixel(Gfx_Texture *texture, i32 x, i32 y, u32 rgba);
static               void gfx_update_from_own_bytes(Gfx_Render_Context *gfx, Gfx_Texture *texture);
static            LRESULT gfx_win32_window_procedure(HWND window, u32 message, WPARAM w, LPARAM l);
static Gfx_Render_Context gfx_window_create(Arena *arena, char *window_name, u32 width, u32 height);
static               bool gfx_window_should_close(Gfx_Render_Context *gfx);
