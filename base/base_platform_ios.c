#define BASE_PLATFORM_IOS

// TODO(felix): Add the right flags based on BASE_OS once metaprogram parser handles #ifs.

// Actual iPhone
META(compile_and_link_flag "-isysroot")
META(compile_and_link_flag "/Applications/Xcode.app/Contents/Developer/Platforms/iPhoneOS.platform/Developer/SDKs/iPhoneOS.sdk")
META(compile_and_link_flag "-target")
META(compile_and_link_flag "arm64-apple-ios")
#if BASE_OS != BASE_OS_IOS_DEVICE
    #error "This is the wrong block."
#endif

// // iPhone Simulator
// META(compile_and_link_flag "-isysroot")
// META(compile_and_link_flag "/Applications/Xcode.app/Contents/Developer/Platforms/iPhoneSimulator.platform/Developer/SDKs/iPhoneSimulator.sdk")
// META(compile_and_link_flag "-target")
// META(compile_and_link_flag "arm64-apple-ios-simulator")
// #if BASE_OS != BASE_OS_IOS_SIMULATOR
//     #error "This is the wrong block."
// #endif

META(link_flag "-framework") META(link_flag "UIKit")
META(link_flag "-framework") META(link_flag "Foundation")
META(link_flag "-framework") META(link_flag "CoreGraphics")
META(link_flag "-framework") META(link_flag "QuartzCore")
META(link_flag "-framework") META(link_flag "Metal")
META(link_flag "-framework") META(link_flag "CoreText")

// https://developer.apple.com/library/archive/documentation/Cocoa/Conceptual/ObjCRuntimeGuide/Articles/ocrtTypeEncodings.html
// https://developer.apple.com/documentation/objectivec/objective-c-runtime

typedef unsigned char                Boolean;
typedef struct __CFAllocator        *CFAllocatorRef;
typedef struct __CFAttributedString *CFAttributedStringRef;
typedef struct __CFDictionary       *CFDictionaryRef;
typedef long                         CFIndex;
typedef struct __CFString           *CFStringRef;
typedef const void                  *CFTypeRef;
typedef struct CGColor              *CGColorRef;
typedef struct CGColorSpace         *CGColorSpaceRef;
typedef struct CGContext            *CGContextRef;
typedef struct CGDataProvider       *CGDataProviderRef;
typedef struct CGFont               *CGFontRef;
typedef struct objc_class           *Class;
typedef struct __CTFont             *CTFontRef;
typedef struct __CTLine             *CTLineRef;
typedef struct objc_object          *id;
typedef id                           Protocol;
typedef struct objc_selector        *SEL;

typedef id (*IMP)(id, SEL, ...);

typedef enum {
    kCFStringEncodingUTF8 = 0x08000100,
} CFStringEncoding;

typedef enum {
    kCGImageAlphaNone,
    kCGImageAlphaPremultipliedLast,
    kCGImageAlphaPremultipliedFirst,
    kCGImageAlphaLast,
    kCGImageAlphaFirst,
    kCGImageAlphaNoneSkipLast,
    kCGImageAlphaNoneSkipFirst,
    kCGImageAlphaOnly
} CGImageAlphaInfo;

typedef enum {
    MTLLoadActionClear = 2,
} MTLLoadAction;

typedef enum {
    MTLPixelFormatR8Unorm         = 10,
    MTLPixelFormatR8Unorm_sRGB    = 11,
    MTLPixelFormatBGRA8Unorm      = 80,
    MTLPixelFormatBGRA8Unorm_sRGB = 81,
} MTLPixelFormat;

typedef enum {
    MTLPrimitiveTypeTriangle = 3,
} MTLPrimitiveType;

typedef struct { unsigned long x, y, z; } MTLOrigin;
typedef struct { unsigned long width, height, depth; } MTLSize;
typedef struct { MTLOrigin origin; MTLSize size; } MTLRegion;

typedef enum {
    MTLStorageModeShared = 0,
} MTLStorageMode;

#define MTLResourceStorageModeShift 4
typedef enum {
    MTLResourceStorageModeShared = MTLStorageModeShared << MTLResourceStorageModeShift,
} MTLResourceOptions;

typedef enum {
    MTLStoreActionStore = 1,
} MTLStoreAction;

typedef double CFTimeInterval;
typedef double CGFloat;
typedef struct { CGFloat a, b, c, d, tx, ty; } CGAffineTransform;
typedef struct { CGFloat x, y; } CGPoint;
typedef struct { CGFloat width, height; } CGSize;
typedef struct { CGPoint origin; CGSize size; } CGRect;
typedef struct { double red, green, blue, alpha; } MTLClearColor;
typedef struct { CGFloat top, left, bottom, right; } UIEdgeInsets;

extern CFAttributedStringRef CFAttributedStringCreate(CFAllocatorRef, CFStringRef, CFDictionaryRef);
extern CFDictionaryRef       CFDictionaryCreate(CFAllocatorRef, const void **, const void **, CFIndex, const void *, const void *);
extern void                  CFRelease(CFTypeRef);
extern CFStringRef           CFStringCreateWithBytes(CFAllocatorRef, const u8 *, CFIndex, u32, Boolean);
extern CGContextRef          CGBitmapContextCreate(void *, size_t, size_t, size_t, size_t, CGColorSpaceRef, u32);
extern void                 *CGBitmapContextGetData(CGContextRef);
extern CGColorRef            CGColorCreateSRGB(CGFloat r, CGFloat g, CGFloat b, CGFloat a);
extern void                  CGColorRelease(CGColorRef);
extern void                  CGContextScaleCTM(CGContextRef, CGFloat, CGFloat);
extern void                  CGContextSetTextMatrix(CGContextRef, CGAffineTransform);
extern void                  CGContextSetTextPosition(CGContextRef, CGFloat, CGFloat);
extern void                  CGContextTranslateCTM(CGContextRef, CGFloat, CGFloat);
extern CGDataProviderRef     CGDataProviderCreateWithData(void *, const void *, size_t, void *);
extern CGFontRef             CGFontCreateWithDataProvider(CGDataProviderRef);
extern _Bool                 class_addMethod(Class, SEL name, IMP implementation, const char *types);
extern _Bool                 class_addProtocol(Class, Protocol *);
extern CTFontRef             CTFontCreateWithGraphicsFont(CGFontRef, CGFloat, const CGAffineTransform *, void *);
extern CTLineRef             CTLineCreateWithAttributedString(CFAttributedStringRef);
extern void                  CTLineDraw(CTLineRef, CGContextRef);
extern double                CTLineGetTypographicBounds(CTLineRef, CGFloat *, CGFloat *, CGFloat *);
extern id                    MTLCreateSystemDefaultDevice(void);
extern Class                 objc_allocateClassPair(Class superclass, const char *name, size_t extra_bytes);
extern void                  objc_autoreleasePoolPop(void *);
extern void                 *objc_autoreleasePoolPush(void);
extern Class                 objc_getClass(const char *name);
extern id                    objc_msgSend(id self, SEL, ...);
extern void                  objc_registerClassPair(Class);
extern Class                 object_getClass(id);
extern SEL                   sel_registerName(const char *name);

#define CGAffineTransformIdentity ((CGAffineTransform){ 1.0, 0, 0, 1.0, 0, 0 })

extern const CFStringRef kCTFontAttributeName;
extern id                NSDefaultRunLoopMode;

extern int UIApplicationMain(int argc, char *argv[], id principal_class_name, id delegate_class_name);

static Class objc__allocate_class_pair(Class superclass, const char *name, size_t extra_bytes) {
    Class class = objc_allocateClassPair(superclass, name, extra_bytes);
    ensure(class != 0);
    return class;
}

static Class objc__get_class(const char *name) {
    Class class = objc_getClass(name);
    ensure(class != 0);
    return class;
}

static Class object__get_class(id id) {
    Class class = object_getClass(id);
    ensure(class != 0);
    return class;
}

struct Platform_Implementation {
    Platform base;

    Arena frame_scratch;
    Class AppDelegate_class;
    Class TickDriver_class;
    id tick_driver;
    id display_link;
    id view;
    id device;
    id metal_layer;
    id queue;
    id pipeline;
    CGFontRef core_graphics_font;
    id dummy_texture;
    const char *ttf_bytes;
    u64 ttf_size;
};
static Platform_Implementation platform__;

typedef struct {
    f32 position[2];      // 0
    f32 uv[2];            // 8
    f32 half_size[2];     // 16
    f32 corner_radius;    // 24
    f32 border_width;     // 28
    f32 fill_color[4];    // 32
    f32 border_color[4];  // 48
    f32 mode;             // 64
    f32 _padding[3];      // 68 — pad to 80
} Vertex;
_Static_assert(sizeof(Vertex) == 80, "Vertex must match MSL stride");

static void platform__draw_text(Draw_Command *c, V2 *only_measure, id encoder) {
    CTFontRef font = CTFontCreateWithGraphicsFont(platform__.core_graphics_font, (CGFloat)c->text.font_size, 0, 0);

    CFStringRef cf_str = CFStringCreateWithBytes(
        0, (const u8 *)c->text.string.data, (CFIndex)c->text.string.count,
        kCFStringEncodingUTF8, false);
    const void *keys[] = { kCTFontAttributeName };
    const void *vals[] = { font };
    CFDictionaryRef attrs = CFDictionaryCreate(0, keys, vals, 1, 0, 0);
    CFAttributedStringRef attr_str = CFAttributedStringCreate(0, cf_str, attrs);
    CTLineRef line = CTLineCreateWithAttributedString(attr_str);

    CGFloat ascent = 0, descent = 0;
    double width_d = CTLineGetTypographicBounds(line, &ascent, &descent, 0);
    f32 width = (f32)width_d;
    f32 height = (f32)(ascent + descent);

    if (only_measure) {
        *only_measure = (V2){ width, height };
        goto cleanup;
    }

    size_t bitmap_width = (size_t)ceilf(width);
    size_t bitmap_height = (size_t)ceilf(height);
    if (bitmap_width == 0 || bitmap_height == 0) goto cleanup;

    CGContextRef ctx = CGBitmapContextCreate(0, bitmap_width, bitmap_height, 8, bitmap_width, 0, kCGImageAlphaOnly);
    CGContextSetTextMatrix(ctx, CGAffineTransformIdentity);
    CGContextSetTextPosition(ctx, 0, (CGFloat)descent);
    CTLineDraw(line, ctx);
    void *bitmap = CGBitmapContextGetData(ctx);

    // Upload texture
    Class MTLTextureDescriptor = objc__get_class("MTLTextureDescriptor");
    id desc = ((id(*)(Class, SEL, unsigned long, unsigned long, unsigned long, _Bool))(void *)objc_msgSend)(
        MTLTextureDescriptor,
        sel_registerName("texture2DDescriptorWithPixelFormat:width:height:mipmapped:"),
        MTLPixelFormatR8Unorm, (unsigned long)bitmap_width, (unsigned long)bitmap_height, false
    );
    id tex = ((id(*)(id, SEL, id))(void *)objc_msgSend)(platform__.device, sel_registerName("newTextureWithDescriptor:"), desc);
    MTLRegion region = { {0,0,0}, {(unsigned long)bitmap_width, (unsigned long)bitmap_height, 1} };
    ((void(*)(id, SEL, MTLRegion, unsigned long, void *, unsigned long))(void *)objc_msgSend)(
        tex,
        sel_registerName("replaceRegion:mipmapLevel:withBytes:bytesPerRow:"),
        region,
        0UL,
        bitmap,
        (unsigned long)bitmap_width
    );

    V4 fill = rgba_float_from_hex(c->color[Draw_Color_SOLID]);
    Vertex base = { .mode = 1.f };
    memcpy(&base.fill_color, &fill, sizeof base.fill_color);

    Vertex verts[6];
    f32 l = c->position.x, t = c->position.y;
    f32 w = (f32)bitmap_width, h = (f32)bitmap_height;
    #define V(idx, px, py, u, vv) do { \
        verts[idx] = base; \
        verts[idx].position[0] = (px); verts[idx].position[1] = (py); \
        verts[idx].uv[0] = (u);  verts[idx].uv[1] = (vv); \
    } while (0)
    V(0, l,     t,     0, 0);
    V(1, l,     t + h, 0, 1);
    V(2, l + w, t + h, 1, 1);
    V(3, l,     t,     0, 0);
    V(4, l + w, t + h, 1, 1);
    V(5, l + w, t,     1, 0);
    #undef V

    ((void(*)(id, SEL, id, unsigned long))(void *)objc_msgSend)(encoder, sel_registerName("setFragmentTexture:atIndex:"), tex, 0UL);
    ((void(*)(id, SEL, void *, unsigned long, unsigned long))(void *)objc_msgSend)(encoder, sel_registerName("setVertexBytes:length:atIndex:"), verts, sizeof verts, 0UL);
    ((void(*)(id, SEL, unsigned long, unsigned long, unsigned long))(void *)objc_msgSend)(encoder, sel_registerName("drawPrimitives:vertexStart:vertexCount:"), (unsigned long)MTLPrimitiveTypeTriangle, 0UL, 6UL);

    // (CGContext released below; texture released by autorelease pool)
    // Restore dummy texture for following rect draws
    ((void(*)(id, SEL, id, unsigned long))(void *)objc_msgSend)(encoder, sel_registerName("setFragmentTexture:atIndex:"), platform__.dummy_texture, 0UL);

    extern void CGContextRelease(CGContextRef);
    CGContextRelease(ctx);

    cleanup:
    CFRelease(line);
    CFRelease(attr_str);
    CFRelease(attrs);
    CFRelease(cf_str);
    CFRelease(font);
}

static V2 platform_measure_text(Platform *platform, String text, f32 font_size) {
    (void)platform;

    V2 size = {0};
    Draw_Command c = {
        .kind = Draw_Kind_TEXT,
        .text = { .string = text, .font_size = font_size },
    };
    platform__draw_text(&c, &size, 0);

    return size;
}

static const char *platform_view_class_name = "PlatformView";

#define ENC_DID_LAUNCH "B@:@@"
static _Bool platform__did_launch(id self, SEL command, id app, id options) {
    (void)self;
    (void)command;
    (void)app;
    (void)options;

    {
        static Arena persistent_arena;
        persistent_arena = arena_init(4 * 1024 * 1024);
        static Arena frame_arena;
        frame_arena = arena_init(4 * 1024 * 1024);
        platform__.base = (Platform){
            .persistent_arena = &persistent_arena,
            .frame_arena = &frame_arena,
        };

        app_start(&platform__.base);

        platform__.frame_scratch = *platform__.base.frame_arena;
    }

    Class UIScreen = objc__get_class("UIScreen");
    id screen = ((id(*)(Class, SEL))objc_msgSend)(UIScreen, sel_registerName("mainScreen"));
    CGRect bounds = ((CGRect(*)(id, SEL))(void *)objc_msgSend)(screen, sel_registerName("bounds"));

    // NOTE(felix): This will have to be queried per-frame if we want to support external displays. For now it's fine to set it once here.
    platform__.base.frame.dpi_scale = (f32)((CGFloat(*)(id, SEL))(void *)objc_msgSend)(screen, sel_registerName("scale"));

    Class UIWindow = objc__get_class("UIWindow");
    id window = ((id(*)(Class, SEL))objc_msgSend)(UIWindow, sel_registerName("alloc"));
    window = ((id(*)(id, SEL, CGRect))objc_msgSend)(window, sel_registerName("initWithFrame:"), bounds);

    Class UIViewController = objc__get_class("UIViewController");
    id view_controller = ((id(*)(Class, SEL))objc_msgSend)(UIViewController, sel_registerName("alloc"));
    view_controller = ((id(*)(id, SEL))objc_msgSend)(view_controller, sel_registerName("init"));

    Class PlatformView = objc__get_class(platform_view_class_name);
    platform__.view = ((id(*)(Class, SEL))objc_msgSend)(PlatformView, sel_registerName("alloc"));
    platform__.view = ((id(*)(id, SEL, CGRect))objc_msgSend)(platform__.view, sel_registerName("initWithFrame:"), bounds);
    ((void(*)(id, SEL, id))(void *)objc_msgSend)(view_controller, sel_registerName("setView:"), platform__.view);

    // TODO(felix): Remove.
    if (false) {
        CGFloat rgba[4] = {0};
        rgba_double_from_hex(platform__.base.clear_color, rgba);
        CGColorRef color = CGColorCreateSRGB(rgba[0], rgba[1], rgba[2], rgba[3]);

        id layer = ((id(*)(id, SEL))objc_msgSend)(platform__.view, sel_registerName("layer"));
        ((void(*)(id, SEL, CGColorRef))(void *)objc_msgSend)(layer, sel_registerName("setBackgroundColor:"), color);

        CGColorRelease(color);
    }

    // TODO(felix): Various touchups, enums, etc.
    {
        platform__.device = MTLCreateSystemDefaultDevice();
        ensure(platform__.device != 0);

        platform__.metal_layer = ((id(*)(id, SEL))objc_msgSend)(platform__.view, sel_registerName("layer"));
        ((void(*)(id, SEL, id))(void *)objc_msgSend)(platform__.metal_layer, sel_registerName("setDevice:"), platform__.device);
        // TODO(felix): Switch to Unorm_sRGB.
        ((void(*)(id, SEL, unsigned long))(void *)objc_msgSend)(platform__.metal_layer, sel_registerName("setPixelFormat:"), MTLPixelFormatBGRA8Unorm);

        ensure(platform__.base.frame.dpi_scale > 0);
        ((void(*)(id, SEL, CGFloat))(void *)objc_msgSend)(platform__.metal_layer, sel_registerName("setContentsScale:"), (CGFloat)platform__.base.frame.dpi_scale);

        platform__.queue = ((id(*)(id, SEL))objc_msgSend)(platform__.device, sel_registerName("newCommandQueue"));

        assert(platform__.ttf_bytes != 0);
        assert(platform__.ttf_size > 0);
        CGDataProviderRef provider = CGDataProviderCreateWithData(0, platform__.ttf_bytes, platform__.ttf_size, 0);
        platform__.core_graphics_font = CGFontCreateWithDataProvider(provider);
        CFRelease((CFTypeRef)provider);
        ensure(platform__.core_graphics_font != 0);

        static const char metal_shader_source[] = "#include <metal_stdlib>\n" STRINGIFY(
            using namespace metal;

            struct Vertex_In {
                float2 position;
                float2 uv;
                float2 half_size;
                float  corner_radius;
                float  border_width;
                float4 fill_color;
                float4 border_color;
                float  mode;
            };

            struct Uniforms { float2 viewport_size; };

            struct VS_Out {
                float4 position [[position]];
                float2 uv;
                float2 half_size;
                float  corner_radius;
                float  border_width;
                float4 fill_color;
                float4 border_color;
                float  mode;
            };

            constexpr sampler tex_sampler(
                coord::normalized,
                address::clamp_to_edge,
                filter::linear);

            vertex VS_Out vs_main(
                constant Vertex_In *verts [[buffer(0)]],
                constant Uniforms  &u     [[buffer(1)]],
                uint vid                  [[vertex_id]]
            ) {
                Vertex_In in = verts[vid];
                float2 ndc = float2(
                     2.0 * in.position.x / u.viewport_size.x - 1.0,
                     1.0 - 2.0 * in.position.y / u.viewport_size.y
                );
                VS_Out o;
                o.position      = float4(ndc, 0.0, 1.0);
                o.uv            = in.uv;
                o.half_size     = in.half_size;
                o.corner_radius = in.corner_radius;
                o.border_width  = in.border_width;
                o.fill_color    = in.fill_color;
                o.border_color  = in.border_color;
                o.mode          = in.mode;
                return o;
            }

            float sdf_box(float2 p, float2 b, float r) {
                float2 q = abs(p) - b + r;
                return length(max(q, 0.0)) + min(max(q.x, q.y), 0.0) - r;
            }

            fragment float4 fs_main(
                VS_Out in [[stage_in]],
                texture2d<float> atlas [[texture(0)]]
            ) {
                if (in.mode < 0.5) {
                    float2 p = in.uv * in.half_size;
                    float d = sdf_box(p, in.half_size, in.corner_radius);
                    float aa = fwidth(d);
                    float coverage = 1.0 - smoothstep(-aa, aa, d);
                    float4 color = in.fill_color;
                    if (in.border_width > 0.0) {
                        float t = smoothstep(-in.border_width - aa, -in.border_width + aa, d);
                        color = mix(in.fill_color, in.border_color, t);
                    }
                    return float4(color.rgb, color.a * coverage);
                }
                float mask = atlas.sample(tex_sampler, in.uv).r;
                return float4(in.fill_color.rgb, in.fill_color.a * mask);
            }
        );

        Class NSString = objc__get_class("NSString");
        id source = ((id(*)(Class, SEL))objc_msgSend)(NSString, sel_registerName("alloc"));
        source = ((id(*)(id, SEL, const char *))objc_msgSend)(source, sel_registerName("initWithUTF8String:"), metal_shader_source);
        id error = 0;
        id library = ((id(*)(id, SEL, id, id, id *))(void *)objc_msgSend)(platform__.device, sel_registerName("newLibraryWithSource:options:error:"), source, 0, &error);
        ensure(library != 0);

        id vs_name = ((id(*)(Class, SEL, const char *))objc_msgSend)(NSString, sel_registerName("stringWithUTF8String:"), "vs_main");
        id fs_name = ((id(*)(Class, SEL, const char *))objc_msgSend)(NSString, sel_registerName("stringWithUTF8String:"), "fs_main");
        id vs_function = ((id(*)(id, SEL, id))(void *)objc_msgSend)(library, sel_registerName("newFunctionWithName:"), vs_name);
        id fs_function = ((id(*)(id, SEL, id))(void *)objc_msgSend)(library, sel_registerName("newFunctionWithName:"), fs_name);
        ensure(vs_function != 0);
        ensure(fs_function != 0);

        Class MTLRenderPipelineDescriptor = objc__get_class("MTLRenderPipelineDescriptor");
        id pipeline_descriptor = ((id(*)(Class, SEL))objc_msgSend)(MTLRenderPipelineDescriptor, sel_registerName("alloc"));
        pipeline_descriptor = ((id(*)(id, SEL))objc_msgSend)(pipeline_descriptor, sel_registerName("init"));
        ((void(*)(id, SEL, id))(void *)objc_msgSend)(pipeline_descriptor, sel_registerName("setVertexFunction:"), vs_function);
        ((void(*)(id, SEL, id))(void *)objc_msgSend)(pipeline_descriptor, sel_registerName("setFragmentFunction:"), fs_function);

        id color_attachments = ((id(*)(id, SEL))objc_msgSend)(pipeline_descriptor, sel_registerName("colorAttachments"));
        id color_attachment_0 = ((id(*)(id, SEL, unsigned long))(void *)objc_msgSend)(color_attachments, sel_registerName("objectAtIndexedSubscript:"), 0UL);
        // TODO(felix): Switch to Unorm_sRGB.
        ((void(*)(id, SEL, unsigned long))(void *)objc_msgSend)(color_attachment_0, sel_registerName("setPixelFormat:"), MTLPixelFormatBGRA8Unorm);
        ((void(*)(id, SEL, _Bool))(void *)objc_msgSend)(color_attachment_0, sel_registerName("setBlendingEnabled:"), true);
        // straight-alpha source-over
        ((void(*)(id, SEL, unsigned long))(void *)objc_msgSend)(color_attachment_0, sel_registerName("setSourceRGBBlendFactor:"), 4UL);          // SourceAlpha
        ((void(*)(id, SEL, unsigned long))(void *)objc_msgSend)(color_attachment_0, sel_registerName("setDestinationRGBBlendFactor:"), 5UL);     // OneMinusSourceAlpha
        ((void(*)(id, SEL, unsigned long))(void *)objc_msgSend)(color_attachment_0, sel_registerName("setSourceAlphaBlendFactor:"), 1UL);        // One
        ((void(*)(id, SEL, unsigned long))(void *)objc_msgSend)(color_attachment_0, sel_registerName("setDestinationAlphaBlendFactor:"), 5UL);   // OneMinusSourceAlpha

        platform__.pipeline = ((id(*)(id, SEL, id, id *))(void *)objc_msgSend)(platform__.device, sel_registerName("newRenderPipelineStateWithDescriptor:error:"), pipeline_descriptor, &error);
        ensure(platform__.pipeline != 0);

        // NOTE(felix): 1x1 white dummy texture is always bound so that non-text draws have a valid texture.
        // TODO(felix): See if this is really needed.
        Class MTLTextureDescriptor = objc__get_class("MTLTextureDescriptor");
        id dummy_descriptor = ((id(*)(Class, SEL, unsigned long, unsigned long, unsigned long, _Bool))(void *)objc_msgSend)(MTLTextureDescriptor, sel_registerName("texture2DDescriptorWithPixelFormat:width:height:mipmapped:"), (unsigned long)MTLPixelFormatR8Unorm, 1UL, 1UL, false);
        platform__.dummy_texture = ((id(*)(id, SEL, id))(void *)objc_msgSend)(platform__.device, sel_registerName("newTextureWithDescriptor:"), dummy_descriptor);
        {
            u8 white = 255;
            MTLRegion region = { { 0, 0, 0 }, { 1, 1, 1 } };
            ((void(*)(id, SEL, MTLRegion, unsigned long, void *, unsigned long))(void *)objc_msgSend)(platform__.dummy_texture, sel_registerName("replaceRegion:mipmapLevel:withBytes:bytesPerRow:"), region, 0UL, &white, 1UL);
        }
    }

    ((void(*)(id, SEL, id))(void *)objc_msgSend)(window, sel_registerName("setRootViewController:"), view_controller);
    ((void(*)(id, SEL))(void *)objc_msgSend)(window, sel_registerName("makeKeyAndVisible"));

    platform__.tick_driver = ((id(*)(Class, SEL))objc_msgSend)(platform__.TickDriver_class, sel_registerName("alloc"));
    platform__.tick_driver = ((id(*)(id, SEL))objc_msgSend)(platform__.tick_driver, sel_registerName("init"));

    Class CADisplayLink = objc__get_class("CADisplayLink");
    SEL sel_tick = sel_registerName("tick:");
    platform__.display_link = ((id(*)(Class, SEL, id, SEL))objc_msgSend)(CADisplayLink, sel_registerName("displayLinkWithTarget:selector:"), platform__.tick_driver, sel_tick);

    Class NSRunLoop = objc__get_class("NSRunLoop");
    id main_runloop = ((id(*)(Class, SEL))objc_msgSend)(NSRunLoop, sel_registerName("mainRunLoop"));
    ((void(*)(id, SEL, id, id))(void *)objc_msgSend)(platform__.display_link, sel_registerName("addToRunLoop:forMode:"), main_runloop, NSDefaultRunLoopMode);

    return true;
}

#define ENC_TICK "v@:@"
static void platform__tick(id self, SEL command, id link) {
    (void)self;
    (void)command;
    (void)link;

    *platform__.base.frame_arena = platform__.frame_scratch;
    Array_Draw_Command *draw_commands = &platform__.base.draw_commands;
    *draw_commands = (Array_Draw_Command){0};

    App_Frame_Info *frame = &platform__.base.frame;

    // TODO(felix): Move some of this to time.h.
    static CFTimeInterval last_timestamp;
    CFTimeInterval timestamp = ((CFTimeInterval(*)(id, SEL))(void *)objc_msgSend)(link, sel_registerName("timestamp"));
    frame->seconds_since_last_frame = (last_timestamp == 0.0) ? (1.f / 60.f) : (f32)(timestamp - last_timestamp);
    last_timestamp = timestamp;

    CGRect bounds = ((CGRect(*)(id, SEL))(void *)objc_msgSend)(platform__.view, sel_registerName("bounds"));

    assert(frame->dpi_scale > 0);
    frame->window_size = v2_scale((V2){ .x = (f32)bounds.size.width, .y = (f32)bounds.size.height }, frame->dpi_scale);

    UIEdgeInsets insets = ((UIEdgeInsets(*)(id, SEL))(void *)objc_msgSend)(platform__.view, sel_registerName("safeAreaInsets"));
    frame->safe_area_inset_left   = frame->dpi_scale * (f32)insets.left;
    frame->safe_area_inset_top    = frame->dpi_scale * (f32)insets.top;
    frame->safe_area_inset_right  = frame->dpi_scale * (f32)insets.right;
    frame->safe_area_inset_bottom = frame->dpi_scale * (f32)insets.bottom;

    app_update_and_render(&platform__.base);

    {
        // TODO(felix): This whole thing feels hacky. Do we really need to count all the rectangles before-hand?
        u64 rectangle_count = 0;
        for (u64 i = 0; i < draw_commands->count; i += 1) {
            Draw_Command *c = &draw_commands->data[i];
            rectangle_count += (u64)(c->kind == Draw_Kind_RECTANGLE);
        }

        u64 rectangle_vertex_count = rectangle_count * 6;
        Vertex *rectangle_vertices = arena_make(platform__.base.frame_arena, rectangle_vertex_count, Vertex);

        u64 vertex_index = 0;
        for (u64 i = 0; i < draw_commands->count; i += 1) {
            Draw_Command *c = &draw_commands->data[i];
            if (c->kind != Draw_Kind_RECTANGLE) continue;

            // TODO(felix): Support clipping.
            assert(memcmp(&c->clip, &(V4){0}, sizeof c->clip) == 0);

            f32 left = c->position.x, top = c->position.y;
            f32 width = c->rectangle.size.x, height = c->rectangle.size.y;
            f32 half_width = width * 0.5f, half_height = height * 0.5f;

            V4 color_top_left = rgba_float_from_hex(c->color[Draw_Color_TOP_LEFT]);
            V4 color_bottom_left = color_top_left;
            V4 color_bottom_right = color_top_left;
            V4 color_top_right = color_top_left;
            if (c->gradient) {
                color_bottom_left = rgba_float_from_hex(c->color[Draw_Color_BOTTOM_LEFT]);
                color_bottom_right = rgba_float_from_hex(c->color[Draw_Color_BOTTOM_RIGHT]);
                color_top_right = rgba_float_from_hex(c->color[Draw_Color_TOP_RIGHT]);
            }

            Vertex base = {
                .half_size = { half_width, half_height },
                .corner_radius = c->rectangle.border_radius,
                .border_width = c->rectangle.border_width,
            };
            V4 color_border = rgba_float_from_hex(c->rectangle.border_color);
            memcpy(&base.border_color, &color_border, sizeof base.border_color);

            // TODO(felix): Deobfuscate.
            #define V(px, py, u, vv, color) do { \
                Vertex temp = base; \
                temp.position[0] = (px); temp.position[1] = (py); \
                temp.uv[0] = (u); temp.uv[1] = (vv); \
                memcpy(&temp.fill_color, &(color), sizeof temp.fill_color); \
                rectangle_vertices[vertex_index++] = temp; \
            } while (0)
            V(left,         top,          -1.f, -1.f, color_top_left);     // top left
            V(left,         top + height, -1.f,  1.f, color_bottom_left);  // bottom left
            V(left + width, top + height,  1.f,  1.f, color_bottom_right); // bottom right
            V(left,         top,          -1.f, -1.f, color_top_left);     // top left
            V(left + width, top + height,  1.f,  1.f, color_bottom_right); // bottom right
            V(left + width, top,           1.f, -1.f, color_top_right);    // top right
            #undef V
        }

        void *pool = objc_autoreleasePoolPush();

        id drawable = ((id(*)(id, SEL))objc_msgSend)(platform__.metal_layer, sel_registerName("nextDrawable"));
        if (drawable == 0) {
            objc_autoreleasePoolPop(pool);
            goto frame_end;
        }
        id drawable_tex = ((id(*)(id, SEL))objc_msgSend)(drawable, sel_registerName("texture"));

        Class MTLRenderPassDescriptor = objc__get_class("MTLRenderPassDescriptor");
        id render_pass_descriptor = ((id(*)(Class, SEL))objc_msgSend)(MTLRenderPassDescriptor, sel_registerName("renderPassDescriptor"));
        id color_attachments = ((id(*)(id, SEL))objc_msgSend)(render_pass_descriptor, sel_registerName("colorAttachments"));
        id color_attachment_0 = ((id(*)(id, SEL, unsigned long))(void *)objc_msgSend)(color_attachments, sel_registerName("objectAtIndexedSubscript:"), 0UL);
        ((void(*)(id, SEL, id))(void *)objc_msgSend)(color_attachment_0, sel_registerName("setTexture:"), drawable_tex);
        ((void(*)(id, SEL, unsigned long))(void *)objc_msgSend)(color_attachment_0, sel_registerName("setLoadAction:"), MTLLoadActionClear);
        ((void(*)(id, SEL, unsigned long))(void *)objc_msgSend)(color_attachment_0, sel_registerName("setStoreAction:"), MTLStoreActionStore);
        CGFloat clear_rgba[4];
        rgba_double_from_hex(platform__.base.clear_color, clear_rgba);
        MTLClearColor clear = { clear_rgba[0], clear_rgba[1], clear_rgba[2], clear_rgba[3] };
        ((void(*)(id, SEL, MTLClearColor))(void *)objc_msgSend)(color_attachment_0, sel_registerName("setClearColor:"), clear);

        id command_buffer = ((id(*)(id, SEL))objc_msgSend)(platform__.queue, sel_registerName("commandBuffer"));
        id render_command_encoder = ((id(*)(id, SEL, id))objc_msgSend)(command_buffer, sel_registerName("renderCommandEncoderWithDescriptor:"), render_pass_descriptor);

        ((void(*)(id, SEL, id))(void *)objc_msgSend)(render_command_encoder, sel_registerName("setRenderPipelineState:"), platform__.pipeline);

        // id vertex_buffer = ((id(*)(id, SEL, void *, unsigned long, unsigned long))(void *)objc_msgSend)(platform__.device, sel_registerName("newBufferWithBytes:length:options:"), vertices, vertex_count * sizeof *vertices, MTLResourceStorageModeShared);
        // ((void(*)(id, SEL, id, unsigned long, unsigned long))(void *)objc_msgSend)(render_command_encoder, sel_registerName("setVertexBuffer:offset:atIndex:"), vertex_buffer, 0UL, 0UL);

        typedef struct {
            f32 viewport_size[2];
        } SDF_Uniforms;
        SDF_Uniforms uniforms = { .viewport_size = { frame->window_size.x, frame->window_size.y } };
        ((void(*)(id, SEL, void *, unsigned long, unsigned long))(void *)objc_msgSend)(render_command_encoder, sel_registerName("setVertexBytes:length:atIndex:"), &uniforms, sizeof uniforms, 1UL);

        // NOTE(felix): Binding dummy texture because shader signature apparently requires a valid texture.
        ((void(*)(id, SEL, id, unsigned long))(void *)objc_msgSend)(render_command_encoder, sel_registerName("setFragmentTexture:atIndex:"), platform__.dummy_texture, 0UL);

        id rectangle_buffer = 0;
        if (rectangle_vertex_count > 0) {
            rectangle_buffer = ((id(*)(id, SEL, void *, unsigned long, unsigned long))(void *)objc_msgSend)(platform__.device, sel_registerName("newBufferWithBytes:length:options:"), rectangle_vertices, rectangle_vertex_count * sizeof *rectangle_vertices, (unsigned long)MTLResourceStorageModeShared);
            ((void(*)(id, SEL, id, unsigned long, unsigned long))(void *)objc_msgSend)(render_command_encoder, sel_registerName("setVertexBuffer:offset:atIndex:"), rectangle_buffer, 0UL, 0UL);
        }

        u64 batch_start = 0;
        u64 batch_count = 0;
        for (u64 i = 0; i < draw_commands->count; i++) {
            Draw_Command *c = &draw_commands->data[i];
            switch (c->kind) {
                case Draw_Kind_NIL: break;
                case Draw_Kind_RECTANGLE: {
                    batch_count += 6;
                } break;
                case Draw_Kind_TEXT: {
                    if (batch_count > 0) {
                        ((void(*)(id, SEL, unsigned long, unsigned long, unsigned long))(void *)objc_msgSend)(
                            render_command_encoder, sel_registerName("drawPrimitives:vertexStart:vertexCount:"),
                            (unsigned long)MTLPrimitiveTypeTriangle, batch_start, batch_count);
                        batch_start += batch_count;
                        batch_count = 0;
                    }
                    // Text path re-binds rectangle_buffer for following rect batches
                    platform__draw_text(c, 0, render_command_encoder);
                    if (rectangle_buffer != 0) {
                        ((void(*)(id, SEL, id, unsigned long, unsigned long))(void *)objc_msgSend)(
                            render_command_encoder, sel_registerName("setVertexBuffer:offset:atIndex:"),
                            rectangle_buffer, 0UL, 0UL);
                    }
                } break;
                default: unreachable;
            }
        }

        if (batch_count > 0) {
            ((void(*)(id, SEL, unsigned long, unsigned long, unsigned long))(void *)objc_msgSend)(render_command_encoder, sel_registerName("drawPrimitives:vertexStart:vertexCount:"), (unsigned long)MTLPrimitiveTypeTriangle, batch_start, batch_count);
        }

        ((void(*)(id, SEL))(void *)objc_msgSend)(render_command_encoder, sel_registerName("endEncoding"));
        ((void(*)(id, SEL, id))(void *)objc_msgSend)(command_buffer, sel_registerName("presentDrawable:"), drawable);
        ((void(*)(id, SEL))(void *)objc_msgSend)(command_buffer, sel_registerName("commit"));

        objc_autoreleasePoolPop(pool);
    }
    frame_end:;

    app_frame_info_reset_end_of_frame(frame);

    if (platform__.base.should_quit) {
        ((void(*)(id, SEL))(void *)objc_msgSend)(platform__.display_link, sel_registerName("invalidate"));
    }
}

static void platform__touches_began(id self, SEL command, id touches, id event) {
    (void)self;
    (void)command;
    (void)event;

    id touch = ((id(*)(id, SEL))(void *)objc_msgSend)(touches, sel_registerName("anyObject"));
    CGPoint point = ((CGPoint(*)(id, SEL, id))(void *)objc_msgSend)(touch, sel_registerName("locationInView:"), platform__.view);

    App_Frame_Info *frame = &platform__.base.frame;
    assert(frame->dpi_scale > 0);
    frame->mouse_position = v2_scale((V2){ .x = (f32)point.x, .y = (f32)point.y }, frame->dpi_scale);
    frame->mouse_clicked[App_Mouse_LEFT] = true;
    frame->mouse_down[App_Mouse_LEFT] = true;
}

static void platform__touches_moved(id self, SEL command, id touches, id event) {
    (void)self;
    (void)command;
    (void)event;

    id touch = ((id(*)(id, SEL))(void *)objc_msgSend)(touches, sel_registerName("anyObject"));
    CGPoint point = ((CGPoint(*)(id, SEL, id))(void *)objc_msgSend)(touch, sel_registerName("locationInView:"), platform__.view);

    App_Frame_Info *frame = &platform__.base.frame;
    assert(frame->dpi_scale > 0);
    frame->mouse_position = v2_scale((V2){ .x = (f32)point.x, .y = (f32)point.y }, frame->dpi_scale);
}

static void platform__touches_ended(id self, SEL command, id touches, id event) {
    (void)self;
    (void)command;
    (void)touches;
    (void)event;

    platform__.base.frame.mouse_down[App_Mouse_LEFT] = false;
}

static Class platform__layer_class(id self, SEL command) {
    (void)self;
    (void)command;
    return objc__get_class("CAMetalLayer");
}

static void program(void) {
    Class UIResponder = objc__get_class("UIResponder");
    platform__.AppDelegate_class = objc__allocate_class_pair(UIResponder, "AppDelegate", 0);
    class_addMethod(platform__.AppDelegate_class, sel_registerName("application:didFinishLaunchingWithOptions:"), (IMP)(void *)platform__did_launch, ENC_DID_LAUNCH);
    objc_registerClassPair(platform__.AppDelegate_class);

    Class ViewBase = objc__get_class("UIView");
    Class ViewClass = objc__allocate_class_pair(ViewBase, platform_view_class_name, 0);
    class_addMethod(ViewClass, sel_registerName("touchesBegan:withEvent:"),     (IMP)(void *)platform__touches_began, "v@:@@");
    class_addMethod(ViewClass, sel_registerName("touchesMoved:withEvent:"),     (IMP)(void *)platform__touches_moved, "v@:@@");
    class_addMethod(ViewClass, sel_registerName("touchesEnded:withEvent:"),     (IMP)(void *)platform__touches_ended, "v@:@@");
    class_addMethod(ViewClass, sel_registerName("touchesCancelled:withEvent:"), (IMP)(void *)platform__touches_ended, "v@:@@");
    objc_registerClassPair(ViewClass);

    Class view_meta = object__get_class((id)ViewClass);
    class_addMethod(view_meta, sel_registerName("layerClass"), (IMP)(void *)platform__layer_class, "#@:");

    Class NSObject = objc__get_class("NSObject");
    platform__.TickDriver_class = objc__allocate_class_pair(NSObject, "TickDriver", 0);
    class_addMethod(platform__.TickDriver_class, sel_registerName("tick:"), (IMP)(void*)platform__tick, ENC_TICK);
    objc_registerClassPair(platform__.TickDriver_class);

    Class NSString = objc__get_class("NSString");
    id delegate_name = ((id(*)(Class, SEL, const char *))objc_msgSend)(NSString, sel_registerName("stringWithUTF8String:"), "AppDelegate");

    UIApplicationMain(main_argc__, main_argv__, 0, delegate_name);
    unreachable;
}
