// https://github.com/felix-u 2026-01-08
// Public domain. NO WARRANTY - use at your own risk.

#if !defined(CPU_DRAW_H)
#define CPU_DRAW_H


typedef struct {
    unsigned *pixels;
    int stride;
    int width;
    int height;
} cpu_draw_Surface;

#if !defined(CPU_DRAW_MAX_FONT_WIDTH)
    #define CPU_DRAW_MAX_FONT_WIDTH 8
#endif
#if !defined(CPU_DRAW_MAX_FONT_HEIGHT)
    #define CPU_DRAW_MAX_FONT_HEIGHT 12
#endif

typedef struct {
    unsigned char width;
    unsigned char pixels[CPU_DRAW_MAX_FONT_WIDTH * CPU_DRAW_MAX_FONT_HEIGHT];
} cpu_draw_Glyph;

typedef struct {
    int height;
    cpu_draw_Glyph glyphs[96];
} cpu_draw_Font;

#if !defined(CPU_DRAW_FUNCTION)
    #define CPU_DRAW_FUNCTION
#endif

CPU_DRAW_FUNCTION void cpu_draw_line(cpu_draw_Surface surface, int start_x, int start_y, int end_x, int end_y, unsigned color);
CPU_DRAW_FUNCTION void cpu_draw_pixel_if_in_bounds(cpu_draw_Surface surface, int x, int y, unsigned color);
CPU_DRAW_FUNCTION void cpu_draw_rectangle(cpu_draw_Surface surface, int left, int top, int right, int bottom, unsigned color);
CPU_DRAW_FUNCTION void cpu_draw_rectangle_rotated(cpu_draw_Surface surface, int left, int top, int right, int bottom, unsigned color, float radians, int pivot_x, int pivot_y);
CPU_DRAW_FUNCTION void cpu_draw_text(cpu_draw_Surface surface, cpu_draw_Font font, int left, int top, unsigned color, const char *text, int length, int *only_measure_width);
CPU_DRAW_FUNCTION void cpu_draw_triangle(cpu_draw_Surface surface, int ax, int ay, int bx, int by, int cx, int cy, unsigned color);
CPU_DRAW_FUNCTION void cpu_draw_triangle_interpolate(cpu_draw_Surface surface, int ax, int ay, int bx, int by, int cx, int cy, unsigned acolor, unsigned bcolor, unsigned ccolor);

cpu_draw_Font cpu_draw_default_font = {
    .height = 5,
    .glyphs = {
        [' ' - 32] = { .width = 2 },
        ['!' - 32] = { .width = 1, .pixels = {
            1,
            1,
            1,
            0,
            1,
        } },
        ['"' - 32] = { .width = 3, .pixels = {
            1, 0, 1,
            1, 0, 1,
            0, 0, 0,
            0, 0, 0,
            0, 0, 0,
        } },
        ['#' - 32] = { .width = 3, .pixels = {
            1, 0, 1,
            1, 1, 1,
            1, 0, 1,
            1, 1, 1,
            1, 0, 1,
        } },
        ['$' - 32] = { .width = 3, .pixels = {
            0, 1, 0,
            0, 1, 1,
            0, 1, 0,
            1, 1, 0,
            0, 1, 0,
        } },
        ['%' - 32] = { .width = 3, .pixels = {
            1, 0, 1,
            0, 0, 1,
            0, 1, 0,
            1, 0, 0,
            1, 0, 1,
        } },
        ['&' - 32] = { .width = 3, .pixels = {
            0, 1, 0,
            1, 0, 1,
            1, 1, 0,
            1, 0, 1,
            0, 1, 1,
        } },
        ['\'' - 32] = { .width = 1, .pixels = {
            1,
            1,
            0,
            0,
            0,
        } },
        ['(' - 32] = { .width = 2, .pixels = {
            0, 1,
            1, 0,
            1, 0,
            1, 0,
            0, 1,
        } },
        [')' - 32] = { .width = 2, .pixels = {
            1, 0,
            0, 1,
            0, 1,
            0, 1,
            1, 0,
        } },
        ['*' - 32] = { .width = 3, .pixels = {
            0, 0, 0,
            1, 0, 1,
            0, 1, 0,
            1, 0, 1,
            0, 0, 0,
        } },
        ['+' - 32] = { .width = 3, .pixels = {
            0, 0, 0,
            0, 1, 0,
            1, 1, 1,
            0, 1, 0,
            0, 0, 0,
        } },
        [',' - 32] = { .width = 2, .pixels = {
            0, 0,
            0, 0,
            0, 0,
            0, 1,
            1, 0,
        } },
        ['-' - 32] = { .width = 3, .pixels = {
            0, 0, 0,
            0, 0, 0,
            1, 1, 1,
            0, 0, 0,
            0, 0, 0,
        } },
        ['.' - 32] = { .width = 1, .pixels = {
            0,
            0,
            0,
            0,
            1,
        } },
        ['/' - 32] = { .width = 3, .pixels = {
            0, 0, 1,
            0, 0, 1,
            0, 1, 0,
            1, 0, 0,
            1, 0, 0,
        } },
        ['0' - 32] = { .width = 3, .pixels = {
            1, 1, 1,
            1, 0, 1,
            1, 0, 1,
            1, 0, 1,
            1, 1, 1,
        } },
        ['1' - 32] = { .width = 3, .pixels = {
            0, 1, 0,
            1, 1, 0,
            0, 1, 0,
            0, 1, 0,
            0, 1, 0,
        } },
        ['2' - 32] = { .width = 3, .pixels = {
            1, 1, 1,
            1, 0, 1,
            0, 0, 1,
            1, 1, 0,
            1, 1, 1,
        } },
        ['3' - 32] = { .width = 3, .pixels = {
            1, 1, 1,
            0, 0, 1,
            1, 1, 1,
            0, 0, 1,
            1, 1, 1,
        } },
        ['4' - 32] = { .width = 3, .pixels = {
            1, 0, 1,
            1, 0, 1,
            1, 1, 1,
            0, 0, 1,
            0, 0, 1,
        } },
        ['5' - 32] = { .width = 3, .pixels = {
            1, 1, 1,
            1, 0, 0,
            1, 1, 0,
            0, 0, 1,
            1, 1, 0,
        } },
        ['6' - 32] = { .width = 3, .pixels = {
            1, 1, 1,
            1, 0, 0,
            1, 1, 1,
            1, 0, 1,
            1, 1, 1,
        } },
        ['7' - 32] = { .width = 3, .pixels = {
            1, 1, 1,
            0, 0, 1,
            0, 1, 0,
            0, 1, 0,
            0, 1, 0,
        } },
        ['8' - 32] = { .width = 3, .pixels = {
            1, 1, 1,
            1, 0, 1,
            1, 1, 1,
            1, 0, 1,
            1, 1, 1,
        } },
        ['9' - 32] = { .width = 3, .pixels = {
            1, 1, 1,
            1, 0, 1,
            1, 1, 1,
            0, 0, 1,
            1, 1, 1,
        } },
        [':' - 32] = { .width = 1, .pixels = {
            0,
            0,
            1,
            0,
            1,
        } },
        [';' - 32] = { .width = 2, .pixels = {
            0, 0,
            0, 1,
            0, 0,
            0, 1,
            1, 0,
        } },
        ['<' - 32] = { .width = 3, .pixels = {
            0, 0, 1,
            0, 1, 0,
            1, 0, 0,
            0, 1, 0,
            0, 0, 1,
        } },
        ['=' - 32] = { .width = 3, .pixels = {
            0, 0, 0,
            1, 1, 1,
            0, 0, 0,
            1, 1, 1,
            0, 0, 0,
        } },
        ['>' - 32] = { .width = 3, .pixels = {
            1, 0, 0,
            0, 1, 0,
            0, 0, 1,
            0, 1, 0,
            1, 0, 0,
        } },
        ['?' - 32] = { .width = 3, .pixels = {
            1, 1, 0,
            0, 0, 1,
            0, 1, 0,
            0, 0, 0,
            0, 1, 0,
        } },
        ['@' - 32] = { .width = 3, .pixels = {
            1, 1, 1,
            1, 0, 1,
            1, 0, 1,
            1, 0, 0,
            1, 1, 1,
        } },
        ['A' - 32] = { .width = 3, .pixels = {
            0, 1, 0,
            1, 0, 1,
            1, 1, 1,
            1, 0, 1,
            1, 0, 1,
        } },
        ['B' - 32] = { .width = 3, .pixels = {
            1, 1, 0,
            1, 0, 1,
            1, 1, 1,
            1, 0, 1,
            1, 1, 0,
        } },
        ['C' - 32] = { .width = 3, .pixels = {
            0, 1, 1,
            1, 0, 0,
            1, 0, 0,
            1, 0, 0,
            0, 1, 1,
        } },
        ['D' - 32] = { .width = 3, .pixels = {
            1, 1, 0,
            1, 0, 1,
            1, 0, 1,
            1, 0, 1,
            1, 1, 0,
        } },
        ['E' - 32] = { .width = 3, .pixels = {
            1, 1, 1,
            1, 0, 0,
            1, 1, 1,
            1, 0, 0,
            1, 1, 1,
        } },
        ['F' - 32] = { .width = 3, .pixels = {
            1, 1, 1,
            1, 0, 0,
            1, 1, 1,
            1, 0, 0,
            1, 0, 0,
        } },
        ['G' - 32] = { .width = 3, .pixels = {
            0, 1, 1,
            1, 0, 0,
            1, 0, 1,
            1, 0, 1,
            0, 1, 0,
        } },
        ['H' - 32] = { .width = 3, .pixels = {
            1, 0, 1,
            1, 0, 1,
            1, 1, 1,
            1, 0, 1,
            1, 0, 1,
        } },
        ['I' - 32] = { .width = 3, .pixels = {
            1, 1, 1,
            0, 1, 0,
            0, 1, 0,
            0, 1, 0,
            1, 1, 1,
        } },
        ['J' - 32] = { .width = 3, .pixels = {
            1, 1, 1,
            0, 0, 1,
            1, 0, 1,
            1, 0, 1,
            0, 1, 0,
        } },
        ['K' - 32] = { .width = 3, .pixels = {
            1, 0, 1,
            1, 0, 1,
            1, 1, 0,
            1, 0, 1,
            1, 0, 1,
        } },
        ['L' - 32] = { .width = 3, .pixels = {
            1, 0, 0,
            1, 0, 0,
            1, 0, 0,
            1, 0, 0,
            1, 1, 1,
        } },
        ['M' - 32] = { .width = 3, .pixels = {
            1, 0, 1,
            1, 1, 1,
            1, 0, 1,
            1, 0, 1,
            1, 0, 1,
        } },
        ['N' - 32] = { .width = 3, .pixels = {
            1, 0, 1,
            1, 1, 1,
            1, 1, 1,
            1, 0, 1,
            1, 0, 1,
        } },
        ['O' - 32] = { .width = 3, .pixels = {
            0, 1, 0,
            1, 0, 1,
            1, 0, 1,
            1, 0, 1,
            0, 1, 0,
        } },
        ['P' - 32] = { .width = 3, .pixels = {
            1, 1, 0,
            1, 0, 1,
            1, 1, 0,
            1, 0, 0,
            1, 0, 0,
        } },
        ['Q' - 32] = { .width = 3, .pixels = {
            0, 1, 0,
            1, 0, 1,
            1, 0, 1,
            0, 1, 0,
            0, 0, 1,
        } },
        ['R' - 32] = { .width = 3, .pixels = {
            1, 1, 0,
            1, 0, 1,
            1, 1, 0,
            1, 0, 1,
            1, 0, 1,
        } },
        ['S' - 32] = { .width = 3, .pixels = {
            0, 1, 1,
            1, 0, 0,
            0, 1, 0,
            0, 0, 1,
            1, 1, 0,
        } },
        ['T' - 32] = { .width = 3, .pixels = {
            1, 1, 1,
            0, 1, 0,
            0, 1, 0,
            0, 1, 0,
            0, 1, 0,
        } },
        ['U' - 32] = { .width = 3, .pixels = {
            1, 0, 1,
            1, 0, 1,
            1, 0, 1,
            1, 0, 1,
            1, 1, 1,
        } },
        ['V' - 32] = { .width = 3, .pixels = {
            1, 0, 1,
            1, 0, 1,
            1, 0, 1,
            1, 0, 1,
            0, 1, 0,
        } },
        ['W' - 32] = { .width = 3, .pixels = {
            1, 0, 1,
            1, 0, 1,
            1, 0, 1,
            1, 1, 1,
            1, 0, 1,
        } },
        ['X' - 32] = { .width = 3, .pixels = {
            1, 0, 1,
            1, 0, 1,
            0, 1, 0,
            1, 0, 1,
            1, 0, 1,
        } },
        ['Y' - 32] = { .width = 3, .pixels = {
            1, 0, 1,
            1, 0, 1,
            0, 1, 0,
            0, 1, 0,
            0, 1, 0,
        } },
        ['Z' - 32] = { .width = 3, .pixels = {
            1, 1, 1,
            0, 0, 1,
            0, 1, 0,
            1, 0, 0,
            1, 1, 1,
        } },
        ['[' - 32] = { .width = 2, .pixels = {
            1, 1,
            1, 0,
            1, 0,
            1, 0,
            1, 1,
        } },
        ['\\' - 32] = { .width = 3, .pixels = {
            1, 0, 0,
            1, 0, 0,
            0, 1, 0,
            0, 0, 1,
            0, 0, 1,
        } },
        [']' - 32] = { .width = 2, .pixels = {
            1, 1,
            0, 1,
            0, 1,
            0, 1,
            1, 1,
        } },
        ['^' - 32] = { .width = 3, .pixels = {
            0, 1, 0,
            1, 0, 1,
            1, 0, 1,
            0, 0, 0,
            0, 0, 0,
        } },
        ['_' - 32] = { .width = 3, .pixels = {
            0, 0, 0,
            0, 0, 0,
            0, 0, 0,
            0, 0, 0,
            1, 1, 1,
        } },
        ['`' - 32] = { .width = 2, .pixels = {
            1, 0,
            0, 1,
            0, 0,
            0, 0,
            0, 0,
        } },
        ['a' - 32] = { .width = 3, .pixels = {
            0, 0, 0,
            0, 1, 1,
            1, 0, 1,
            1, 0, 1,
            1, 1, 1,
        } },
        ['b' - 32] = { .width = 3, .pixels = {
            1, 0, 0,
            1, 1, 1,
            1, 0, 1,
            1, 0, 1,
            1, 1, 1,
        } },
        ['c' - 32] = { .width = 3, .pixels = {
            0, 0, 0,
            1, 1, 1,
            1, 0, 0,
            1, 0, 0,
            1, 1, 1,
        } },
        ['d' - 32] = { .width = 3, .pixels = {
            0, 0, 1,
            1, 1, 1,
            1, 0, 1,
            1, 0, 1,
            1, 1, 1,
        } },
        ['e' - 32] = { .width = 3, .pixels = {
            0, 0, 0,
            1, 1, 1,
            1, 1, 1,
            1, 0, 0,
            1, 1, 1,
        } },
        ['f' - 32] = { .width = 3, .pixels = {
            0, 1, 1,
            0, 1, 0,
            1, 1, 1,
            0, 1, 0,
            0, 1, 0,
        } },
        ['g' - 32] = { .width = 3, .pixels = {
            0, 0, 0,
            1, 1, 1,
            1, 1, 1,
            0, 0, 1,
            1, 1, 1,
        } },
        ['h' - 32] = { .width = 3, .pixels = {
            1, 0, 0,
            1, 1, 1,
            1, 0, 1,
            1, 0, 1,
            1, 0, 1,
        } },
        ['i' - 32] = { .width = 3, .pixels = {
            0, 0, 0,
            1, 1, 0,
            0, 1, 0,
            0, 1, 0,
            1, 1, 1,
        } },
        ['j' - 32] = { .width = 3, .pixels = {
            0, 0, 0,
            0, 1, 1,
            0, 0, 1,
            1, 0, 1,
            0, 1, 1,
        } },
        ['k' - 32] = { .width = 3, .pixels = {
            1, 0, 0,
            1, 0, 1,
            1, 1, 0,
            1, 1, 0,
            1, 0, 1,
        } },
        ['l' - 32] = { .width = 3, .pixels = {
            1, 1, 0,
            0, 1, 0,
            0, 1, 0,
            0, 1, 0,
            0, 1, 1,
        } },
        ['m' - 32] = { .width = 3, .pixels = {
            0, 0, 0,
            1, 0, 1,
            1, 1, 1,
            1, 0, 1,
            1, 0, 1,
        } },
        ['n' - 32] = { .width = 3, .pixels = {
            0, 0, 0,
            1, 1, 0,
            1, 0, 1,
            1, 0, 1,
            1, 0, 1,
        } },
        ['o' - 32] = { .width = 3, .pixels = {
            0, 0, 0,
            1, 1, 1,
            1, 0, 1,
            1, 0, 1,
            1, 1, 1,
        } },
        ['p' - 32] = { .width = 3, .pixels = {
            0, 0, 0,
            1, 1, 1,
            1, 0, 1,
            1, 1, 1,
            1, 0, 0,
        } },
        ['q' - 32] = { .width = 3, .pixels = {
            0, 0, 0,
            1, 1, 1,
            1, 0, 1,
            1, 1, 1,
            0, 0, 1,
        } },
        ['r' - 32] = { .width = 3, .pixels = {
            0, 0, 0,
            1, 1, 0,
            1, 0, 1,
            1, 0, 0,
            1, 0, 0,
        } },
        ['s' - 32] = { .width = 3, .pixels = {
            0, 0, 0,
            0, 1, 1,
            1, 1, 0,
            0, 0, 1,
            1, 1, 0,
        } },
        ['t' - 32] = { .width = 3, .pixels = {
            0, 1, 0,
            1, 1, 1,
            0, 1, 0,
            0, 1, 0,
            0, 1, 0,
        } },
        ['u' - 32] = { .width = 3, .pixels = {
            0, 0, 0,
            1, 0, 1,
            1, 0, 1,
            1, 0, 1,
            1, 1, 1,
        } },
        ['v' - 32] = { .width = 3, .pixels = {
            0, 0, 0,
            1, 0, 1,
            1, 0, 1,
            1, 0, 1,
            0, 1, 0,
        } },
        ['w' - 32] = { .width = 3, .pixels = {
            0, 0, 0,
            1, 0, 1,
            1, 0, 1,
            1, 1, 1,
            1, 0, 1,
        } },
        ['x' - 32] = { .width = 3, .pixels = {
            0, 0, 0,
            1, 0, 1,
            0, 1, 0,
            0, 1, 0,
            1, 0, 1,
        } },
        ['y' - 32] = { .width = 3, .pixels = {
            0, 0, 0,
            1, 0, 1,
            1, 1, 1,
            0, 0, 1,
            1, 1, 0,
        } },
        ['z' - 32] = { .width = 3, .pixels = {
            0, 0, 0,
            1, 1, 1,
            0, 1, 1,
            1, 0, 0,
            1, 1, 1,
        } },
        ['{' - 32] = { .width = 3, .pixels = {
            0, 1, 1,
            0, 1, 0,
            1, 0, 0,
            0, 1, 0,
            0, 1, 1,
        } },
        ['|' - 32] = { .width = 1, .pixels = {
            1,
            1,
            0,
            1,
            1,
        } },
        ['}' - 32] = { .width = 3, .pixels = {
            1, 1, 0,
            0, 1, 0,
            0, 0, 1,
            0, 1, 0,
            1, 1, 0,
        } },
        ['~' - 32] = { .width = 3, .pixels = {
            0, 0, 0,
            1, 0, 0,
            1, 1, 1,
            0, 0, 1,
            0, 0, 0,
        } },
    },
};


#endif // CPU_DRAW_H


#if defined(CPU_DRAW_IMPLEMENTATION)


#if !defined(CPU_DRAW_SINF)
    #include <math.h>
    #define CPU_DRAW_SINF sinf
    #define CPU_DRAW_COSF cosf
#endif

#define CPU_DRAW__SWAP(a, b) do {\
    int swap__temp__ = *(a); *(a) = *(b); *(b) = swap__temp__; \
} while (0)

#define CPU_DRAW__ABS(x) ((x) < 0 ? -(x) : (x))

#define CPU_DRAW__MIN(a, b) ((a) < (b) ? (a) : (b))
#define CPU_DRAW__MAX(a, b) ((a) > (b) ? (a) : (b))

CPU_DRAW_FUNCTION void cpu_draw_line(cpu_draw_Surface surface, int start_x, int start_y, int end_x, int end_y, unsigned color) {
    // Reference: https://en.wikipedia.org/wiki/Bresenham%27s_line_algorithm

    _Bool steep = CPU_DRAW__ABS(end_y - start_y) >= CPU_DRAW__ABS(end_x - start_x);
    if (steep) {
        _Bool bottom_to_top = start_y > end_y;
        if (bottom_to_top) {
            CPU_DRAW__SWAP(&start_x, &end_x);
            CPU_DRAW__SWAP(&start_y, &end_y);
        }

        int dx = end_x - start_x;
        int dy = end_y - start_y;
        int x_add = 1;

        if (dx < 0) {
            x_add = -1;
            dx = -dx;
        }

        int difference = 2 * dx - dy;
        int x = start_x;

        // TODO(felix): investigate
        // start_y = CPU_DRAW__MAX(0, start_y);
        // end_y = CPU_DRAW__MIN(end_y, surface.height - 1);

        for (int y = start_y; y <= end_y; y += 1) {
            cpu_draw_pixel_if_in_bounds(surface, x, y, color);
            if (difference > 0) {
                x += x_add;
                difference += 2 * (dx - dy);
            } else difference += 2 * dx;
        }
    } else {
        _Bool right_to_left = start_x > end_x;
        if (right_to_left) {
            CPU_DRAW__SWAP(&start_x, &end_x);
            CPU_DRAW__SWAP(&start_y, &end_y);
        }

        int dx = end_x - start_x;
        int dy = end_y - start_y;
        int y_add = 1;

        if (dy < 0) {
            y_add = -1;
            dy = -dy;
        }

        int difference = 2 * dy - dx;
        int y = start_y;

        for (int x = start_x; x <= end_x; x += 1) {
            cpu_draw_pixel_if_in_bounds(surface, x, y, color);
            if (difference > 0) {
                y += y_add;
                difference += 2 * (dy - dx);
            } else difference += 2 * dy;
        }
    }
}

CPU_DRAW_FUNCTION void cpu_draw_pixel_if_in_bounds(cpu_draw_Surface surface, int x, int y, unsigned color) {
    if (0 <= x && x < surface.width && 0 <= y && y < surface.height) surface.pixels[y * surface.stride + x] = color;
}

CPU_DRAW_FUNCTION void cpu_draw_rectangle(cpu_draw_Surface surface, int left, int top, int right, int bottom, unsigned color) {
    int row = top * surface.stride;
    for (int y = top; y < bottom; y += 1, row += surface.stride) {
        for (int x = left; x < right; x += 1) {
            surface.pixels[row + x] = color;
        }
    }
}

typedef struct { int x, y; } cpu_draw__V2;
static inline cpu_draw__V2 cpu_draw__v2_add(cpu_draw__V2 b, cpu_draw__V2 a) {
    cpu_draw__V2 result = { .x = b.x + a.x, .y = b.y + a.y };
    return result;
}
static inline cpu_draw__V2 cpu_draw__v2_rotate(cpu_draw__V2 v, float radians) {
    float sin_angle = CPU_DRAW_SINF(radians);
    float cos_angle = CPU_DRAW_COSF(radians);

    float vx = (float)v.x;
    float vy = (float)v.y;
    float x = vx * cos_angle - vy * sin_angle;
    float y = vx * sin_angle + vy * cos_angle;

    cpu_draw__V2 result = { .x = (int)(x + 0.5f), .y = (int)(y + 0.5f) };
    return result;
}
static inline cpu_draw__V2 cpu_draw__v2_sub(cpu_draw__V2 b, cpu_draw__V2 a) {
    cpu_draw__V2 result = { .x = b.x - a.x, .y = b.y - a.y };
    return result;
}

CPU_DRAW_FUNCTION void cpu_draw_rectangle_rotated(cpu_draw_Surface surface, int left, int top, int right, int bottom, unsigned color, float radians, int pivot_x, int pivot_y) {
    if (radians == 0) {
        cpu_draw_rectangle(surface, left, top, right, bottom, color);
        return;
    }

    enum { TOP_LEFT, BOTTOM_LEFT, BOTTOM_RIGHT, TOP_RIGHT };
    cpu_draw__V2 corners[4] = {
        [TOP_LEFT] = { .x = left, .y = top },
        [BOTTOM_LEFT] = { .x = left, .y = bottom },
        [BOTTOM_RIGHT] = { .x = right, .y = bottom },
        [TOP_RIGHT] = { .x = right, .y = top },
    };

    cpu_draw__V2 pivot = { .x = pivot_x, .y = pivot_y };

    for (int i = 0; i < 4; i += 1) {
        cpu_draw__V2 dist = cpu_draw__v2_sub(corners[i], pivot);
        dist = cpu_draw__v2_rotate(dist, radians);
        corners[i] = cpu_draw__v2_add(pivot, dist);
    }

    cpu_draw__V2
        tl = corners[TOP_LEFT],
        bl = corners[BOTTOM_LEFT],
        br = corners[BOTTOM_RIGHT],
        tr = corners[TOP_RIGHT];

    cpu_draw_triangle(surface, tl.x, tl.y, bl.x, bl.y, br.x, br.y, color);
    cpu_draw_triangle(surface, tl.x, tl.y, br.x, br.y, tr.x, tr.y, color);
}

CPU_DRAW_FUNCTION void cpu_draw_text(cpu_draw_Surface surface, cpu_draw_Font font, int left, int top, unsigned color, const char *text, int length, int *only_measure_width) {
    if (only_measure_width != 0) *only_measure_width = 0;

    int x = left;
    for (int i = 0; i < length; i += 1) {
        char c = text[i];
        cpu_draw_Glyph glyph = font.glyphs[c - 32];

        int spacing = 1; // TODO(felix): configurable

        if (only_measure_width != 0) {
            *only_measure_width += glyph.width + spacing;
        } else {
            for (int row = 0; row < font.height; row += 1) {
                int y = top + row;
                for (int column = 0; column < glyph.width; column += 1) {
                    int local_x = x + column;
                    _Bool in_bounds = (0 <= local_x && local_x < surface.width) && (0 <= y && y < surface.height);
                    if (!in_bounds) continue;

                    unsigned char pixel = glyph.pixels[row * glyph.width + column];
                    if (pixel != 0) surface.pixels[y * surface.stride + local_x] = color;
                }
            }
            x += glyph.width + spacing;
        }
    }
}

static inline float cpu_draw__signed_triangle_area(int ax, int ay, int bx, int by, int cx, int cy) {
    float result = 0.5f * (float)((by - ay) * (bx + ax) + (cy - by) * (cx + bx) + (ay - cy) * (ax + cx));
    return result;
}

CPU_DRAW_FUNCTION void cpu_draw_triangle(cpu_draw_Surface surface, int ax, int ay, int bx, int by, int cx, int cy, unsigned color) {
    int bound_min_x = CPU_DRAW__MIN(CPU_DRAW__MIN(ax, bx), cx);
    int bound_min_y = CPU_DRAW__MIN(CPU_DRAW__MIN(ay, by), cy);
    int bound_max_x = CPU_DRAW__MAX(CPU_DRAW__MAX(ax, bx), cx);
    int bound_max_y = CPU_DRAW__MAX(CPU_DRAW__MAX(ay, by), cy);

    if (bound_max_x < 0 || surface.width <= bound_min_x) return;
    if (bound_max_y < 0 || surface.height <= bound_min_y) return;

    float total_area = cpu_draw__signed_triangle_area(ax, ay, bx, by, cx, cy);
    float reciprocal_total_area = 1.f / total_area;

    bound_min_x = CPU_DRAW__MAX(0, bound_min_x);
    bound_min_y = CPU_DRAW__MAX(0, bound_min_y);
    bound_max_x = CPU_DRAW__MIN(surface.width, bound_max_x);
    bound_max_y = CPU_DRAW__MIN(surface.height, bound_max_y);

    for (int x = bound_min_x; x <= bound_max_x; x += 1) {
        for (int y = bound_min_y; y <= bound_max_y; y += 1) {
            float alpha = cpu_draw__signed_triangle_area(x, y, bx, by, cx, cy) * reciprocal_total_area;
            float beta = cpu_draw__signed_triangle_area(x, y, cx, cy, ax, ay) * reciprocal_total_area;
            float gamma = cpu_draw__signed_triangle_area(x, y, ax, ay, bx, by) * reciprocal_total_area;

            _Bool outside = alpha < 0 || beta < 0 || gamma < 0;
            if (outside) continue;

            surface.pixels[y * surface.stride + x] = color;
        }
    }
}

CPU_DRAW_FUNCTION void cpu_draw_triangle_interpolate(cpu_draw_Surface surface, int ax, int ay, int bx, int by, int cx, int cy, unsigned acolor, unsigned bcolor, unsigned ccolor) {
    if (acolor == bcolor && bcolor == ccolor) {
        cpu_draw_triangle(surface, ax, ay, bx, by, cx, cy, acolor);
        return;
    }

    int bound_min_x = CPU_DRAW__MIN(CPU_DRAW__MIN(ax, bx), cx);
    int bound_min_y = CPU_DRAW__MIN(CPU_DRAW__MIN(ay, by), cy);
    int bound_max_x = CPU_DRAW__MAX(CPU_DRAW__MAX(ax, bx), cx);
    int bound_max_y = CPU_DRAW__MAX(CPU_DRAW__MAX(ay, by), cy);

    if (bound_max_x < 0 || surface.width <= bound_min_x) return;
    if (bound_max_y < 0 || surface.height <= bound_min_y) return;

    float total_area = cpu_draw__signed_triangle_area(ax, ay, bx, by, cx, cy);
    float reciprocal_total_area = 1.f / total_area;

    unsigned colors[3] = { acolor, bcolor, ccolor };
    float channels[3][4];
    for (int i = 0; i < 3; i += 1) {
        channels[i][0] = (float)(colors[i] >> 24);
        channels[i][1] = (float)((colors[i] >> 16) & 0xff);
        channels[i][2] = (float)((colors[i] >> 8) & 0xff);
        channels[i][3] = (float)(colors[i] & 0xff);
    }

    bound_min_x = CPU_DRAW__MAX(0, bound_min_x);
    bound_min_y = CPU_DRAW__MAX(0, bound_min_y);
    bound_max_x = CPU_DRAW__MIN(surface.width, bound_max_x);
    bound_max_y = CPU_DRAW__MIN(surface.height, bound_max_y);

    for (int x = bound_min_x; x <= bound_max_x; x += 1) {
        for (int y = bound_min_y; y <= bound_max_y; y += 1) {
            float alpha = cpu_draw__signed_triangle_area(x, y, bx, by, cx, cy) * reciprocal_total_area;
            float beta = cpu_draw__signed_triangle_area(x, y, cx, cy, ax, ay) * reciprocal_total_area;
            float gamma = cpu_draw__signed_triangle_area(x, y, ax, ay, bx, by) * reciprocal_total_area;

            _Bool outside = alpha < 0 || beta < 0 || gamma < 0;
            if (outside) continue;

            // TODO(felix): is there fast integer-based interpolation we can do?
            unsigned interpolated[4];
            for (int i = 0; i < 4; i += 1) {
                interpolated[i] = (unsigned)(alpha * channels[0][i] + beta * channels[1][i] + gamma * channels[2][i]);
            }
            unsigned color = (interpolated[0] << 24) | (interpolated[1] << 16) | (interpolated[2] << 8) | interpolated[3];

            surface.pixels[y * surface.stride + x] = color;
        }
    }
}


#endif // CPU_DRAW_IMPLEMENTATION
