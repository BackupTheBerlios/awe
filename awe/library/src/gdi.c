#include "gdi.h"
#include "allegro/internal/aintern.h"


/*****************************************************************************
    INTERNALS
 *****************************************************************************/


//side enum
enum _SIDE {
    _LEFT,
    _TOP,
    _RIGHT,
    _BOTTOM
};


//var swap
#define _SWAP(TYPE, V1, V2) {\
    TYPE temp = V1;\
    V1 = V2;\
    V2 = temp;\
}


//draws a fat pixel
static void _fat_putpixel(BITMAP *bmp, int x, int y, int c)
{
    putpixel(bmp, x, y, c);
    putpixel(bmp, x - 1, y, c);
    putpixel(bmp, x, y - 1, c);
    putpixel(bmp, x + 1, y, c);
    putpixel(bmp, x, y + 1, c);
}


//actual drawing of a 3d rect
static INLINE void _draw_3d_rect(BITMAP *bmp, int x1, int y1, int x2, int y2, int top_left_color, int bottom_right_color, int width)
{
    for(; width > 0; width--, x1++, y1++, x2--, y2--) {
        vline(bmp, x1, y1, y2, top_left_color);
        hline(bmp, x1, y1, x2, top_left_color);
        vline(bmp, x2, y1, y2, bottom_right_color);
        hline(bmp, x1, y2, x2, bottom_right_color);
    }
}


//blit fill
static INLINE void _fill(BITMAP *bmp, BITMAP *src, int x, int y, int w, int h)
{
    int x1 = x;
    int y1 = y;
    int x2 = x + w;
    int y2 = y + h;
    if (!src) return;
    do{
        x1 = x;
        do{
            masked_blit(src, bmp, 0, 0, x1, y1, MIN(x2 - x1, MIN(w, src->w)), MIN(y2 - y1, MIN(h, src->h)));
            x1 += src->w;
        }while(x1 < x2 - 1);
        y1 += src->h;
    }while(y1 < y2 - 1);
}


//creates a NULL sub-bitmap if w or h is 0
static BITMAP *_create_sub_bitmap(BITMAP *bmp, int x, int y, int w, int h)
{
    if (w <= 0 || h <= 0) return 0;
    return create_sub_bitmap(bmp, x, y, w, h);
}


//stretch blit
static INLINE void _stretch(BITMAP *dst, BITMAP *src, int x, int y, int w, int h)
{
    if (!src) return;
    masked_stretch_blit(src, dst, 0, 0, src->w, src->h, x, y, w, h);
}


/*****************************************************************************
    PUBLIC
 *****************************************************************************/


//prepares a canvas for drawing
void awe_set_canvas(AWE_CANVAS *canvas, BITMAP *bmp, AWE_RECT *area)
{
    canvas->bitmap = bmp;
    canvas->area = *area;
    canvas->clip = *area;
    canvas->x_org = 0;
    canvas->y_org = 0;
    set_clip(bmp, area->left, area->top, area->right, area->bottom);
}


//returns the point of origin of the canvas
void awe_get_canvas_origin(AWE_CANVAS *canvas, int *x, int *y)
{
    *x = canvas->x_org;
    *y = canvas->y_org;
}


//sets the canva's point of origin (actually, the offset)
void awe_set_canvas_origin(AWE_CANVAS *canvas, int x, int y)
{
    canvas->x_org = x;
    canvas->y_org = y;
}


//returns the canvas' current clip
void awe_get_canvas_clip(AWE_CANVAS *canvas, AWE_RECT *r)
{
    *r = canvas->clip;
    AWE_RECT_SHIFT(*r, -AWE_CANVAS_BASE_X(canvas), -AWE_CANVAS_BASE_Y(canvas));
}


//sets the clipping of the canvas
int awe_set_canvas_clip(AWE_CANVAS *canvas, AWE_RECT *clip)
{
    AWE_RECT r = *clip;

    AWE_RECT_SHIFT(r, AWE_CANVAS_BASE_X(canvas), AWE_CANVAS_BASE_Y(canvas));
    if (!AWE_RECT_OVERLAP(r, canvas->area)) return 0;
    AWE_RECT_INTERSECTION(canvas->clip, canvas->area, r);
    set_clip(canvas->bitmap,
             canvas->clip.left, 
             canvas->clip.top, 
             canvas->clip.right, 
             canvas->clip.bottom);
    return 1;
}


//draws a pixel
void awe_draw_pixel(const AWE_CANVAS *canvas, int x, int y, int color)
{
    putpixel(canvas->bitmap, AWE_CANVAS_BASE_X(canvas) + x, AWE_CANVAS_BASE_Y(canvas) + y, color);
}


//draws a horizontal line
void awe_draw_hline(const AWE_CANVAS *canvas, int x1, int y, int x2, int color)
{
    hline(canvas->bitmap, AWE_CANVAS_BASE_X(canvas) + x1, AWE_CANVAS_BASE_Y(canvas) + y, AWE_CANVAS_BASE_X(canvas) + x2, color);
}


//draws a vertical line
void awe_draw_vline(const AWE_CANVAS *canvas, int x, int y1, int y2, int color)
{
    vline(canvas->bitmap, AWE_CANVAS_BASE_X(canvas) + x, AWE_CANVAS_BASE_Y(canvas) + y1, AWE_CANVAS_BASE_Y(canvas) + y2, color);
}


//draws a line
void awe_draw_line(const AWE_CANVAS *canvas, int x1, int y1, int x2, int y2, int color)
{
    line(canvas->bitmap, AWE_CANVAS_BASE_X(canvas) + x1, AWE_CANVAS_BASE_Y(canvas) + y1, AWE_CANVAS_BASE_X(canvas) + x2, AWE_CANVAS_BASE_Y(canvas) + y2, color);
}


//draws a rectangle
void awe_draw_rect(const AWE_CANVAS *canvas, int x1, int y1, int x2, int y2, int color)
{
    rect(canvas->bitmap, AWE_CANVAS_BASE_X(canvas) + x1, AWE_CANVAS_BASE_Y(canvas) + y1, AWE_CANVAS_BASE_X(canvas) + x2, AWE_CANVAS_BASE_Y(canvas) + y2, color);
}


//draws a rectangle with position, size
void awe_draw_rect_s(const AWE_CANVAS *canvas, int x, int y, int w, int h, int color)
{
    awe_draw_rect(canvas, x, y, x + w - 1, y + h - 1, color);
}


//draws a circle
void awe_draw_circle(const AWE_CANVAS *canvas, int x, int y, int radius, int color)
{
    circle(canvas->bitmap, AWE_CANVAS_BASE_X(canvas) + x, AWE_CANVAS_BASE_Y(canvas) + y, radius, color);
}


//draws an ellipse
void awe_draw_ellipse(const AWE_CANVAS *canvas, int x, int y, int hor_radius, int ver_radius, int color)
{
    ellipse(canvas->bitmap, AWE_CANVAS_BASE_X(canvas) + x, AWE_CANVAS_BASE_Y(canvas) + y, hor_radius, ver_radius, color);
}


//draws an arc
void awe_draw_arc(const AWE_CANVAS *canvas, int x, int y, fixed start_angle, fixed end_angle, int radius, int color)
{
    arc(canvas->bitmap, AWE_CANVAS_BASE_X(canvas) + x, AWE_CANVAS_BASE_Y(canvas) + y, start_angle, end_angle, radius, color);
}


//draws a spline
void awe_draw_spline(const AWE_CANVAS *canvas, const int points[8], int color)
{
    int tr_points[8] = {
        AWE_CANVAS_BASE_X(canvas) + points[0], AWE_CANVAS_BASE_Y(canvas) + points[1],
        AWE_CANVAS_BASE_X(canvas) + points[2], AWE_CANVAS_BASE_Y(canvas) + points[3],
        AWE_CANVAS_BASE_X(canvas) + points[4], AWE_CANVAS_BASE_Y(canvas) + points[5],
        AWE_CANVAS_BASE_X(canvas) + points[6], AWE_CANVAS_BASE_Y(canvas) + points[7],
    };
    spline(canvas->bitmap, tr_points, color);
}


//fills a rectangle
void awe_fill_rect(const AWE_CANVAS *canvas, int x1, int y1, int x2, int y2, int color)
{
    rectfill(canvas->bitmap, AWE_CANVAS_BASE_X(canvas) + x1, AWE_CANVAS_BASE_Y(canvas) + y1, AWE_CANVAS_BASE_X(canvas) + x2, AWE_CANVAS_BASE_Y(canvas) + y2, color);
}


//fills a rectangle with position, size
void awe_fill_rect_s(const AWE_CANVAS *canvas, int x, int y, int w, int h, int color)
{
    awe_fill_rect(canvas, x, y, x + w - 1, y + h - 1, color);
}


//fills a circle
void awe_fill_circle(const AWE_CANVAS *canvas, int x, int y, int radius, int color)
{
    circlefill(canvas->bitmap, AWE_CANVAS_BASE_X(canvas) + x, AWE_CANVAS_BASE_Y(canvas) + y, radius, color);
}


//fills an ellipse
void awe_fill_ellipse(const AWE_CANVAS *canvas, int x, int y, int hor_radius, int ver_radius, int color)
{
    ellipsefill(canvas->bitmap, AWE_CANVAS_BASE_X(canvas) + x, AWE_CANVAS_BASE_Y(canvas) + y, hor_radius, ver_radius, color);
}


//fills a gradient
void awe_fill_gradient(const AWE_CANVAS *canvas, int x1, int y1, int x2, int y2, int color1, int color2, int color3, int color4)
{
    x1 = itofix(x1 + AWE_CANVAS_BASE_X(canvas));
    y1 = itofix(y1 + AWE_CANVAS_BASE_Y(canvas));
    x2 = itofix(x2 + AWE_CANVAS_BASE_X(canvas) + 1);
    y2 = itofix(y2 + AWE_CANVAS_BASE_Y(canvas) + 1); {
    V3D v1 = {x1, y1, 0, 0, 0, color1};
    V3D v2 = {x1, y2, 0, 0, 0, color2};
    V3D v3 = {x2, y2, 0, 0, 0, color3};
    V3D v4 = {x2, y1, 0, 0, 0, color4};
    quad3d(canvas->bitmap, POLYTYPE_GCOL, 0, &v1, &v2, &v3, &v4);
    }
}


//fills a gradient with position, size
void awe_fill_gradient_s(const AWE_CANVAS *canvas, int x, int y, int w, int h, int color1, int color2, int color3, int color4)
{
    awe_fill_gradient(canvas, x, y, x + w - 1, y + h - 1, color1, color2, color3, color4);
}


//fills a horizontal gradient
void awe_fill_gradient_hor(const AWE_CANVAS *canvas, int x1, int y1, int x2, int y2, int color1, int color2)
{
    awe_fill_gradient(canvas, x1, y1, x2, y2, color1, color1, color2, color2);
}


//fills a horizontal gradient with position and size
void awe_fill_gradient_hor_s(const AWE_CANVAS *canvas, int x, int y, int w, int h, int color1, int color2)
{
    awe_fill_gradient_hor(canvas, x, y, x + w - 1, y + h - 1, color1, color2);
}


//fills a vertical gradient
void awe_fill_gradient_ver(const AWE_CANVAS *canvas, int x1, int y1, int x2, int y2, int color1, int color2)
{
    awe_fill_gradient(canvas, x1, y1, x2, y2, color1, color2, color2, color1);
}


//fills a vertical gradient with positoin and size
void awe_fill_gradient_ver_s(const AWE_CANVAS *canvas, int x, int y, int w, int h, int color1, int color2)
{
    awe_fill_gradient_ver(canvas, x, y, x + w - 1, y + h - 1, color1, color2);
}


//draws a 3d rect
void awe_draw_3d_rect(const AWE_CANVAS *canvas, int x1, int y1, int x2, int y2, int top_left_color, int bottom_right_color, int width)
{
    x1 += AWE_CANVAS_BASE_X(canvas);
    y1 += AWE_CANVAS_BASE_Y(canvas);
    x2 += AWE_CANVAS_BASE_X(canvas);
    y2 += AWE_CANVAS_BASE_Y(canvas);
    _draw_3d_rect(canvas->bitmap, x1, y1, x2, y2, top_left_color, bottom_right_color, width);
}


//draws a 3d rect with position and size
void awe_draw_3d_rect_s(const AWE_CANVAS *canvas, int x, int y, int w, int h, int top_left_color, int bottom_right_color, int width)
{
    awe_draw_3d_rect(canvas, x, y, x + w - 1, y + h - 1, top_left_color, bottom_right_color, width);
}


//draws a 3d slider handle
void awe_draw_3d_slider_down(const AWE_CANVAS *canvas, int x1, int y1, int x2, int y2, int top_left_color, int bottom_right_color, int width)
{
    BITMAP *bmp = canvas->bitmap;
    int i;
    x1 += AWE_CANVAS_BASE_X(canvas);
    y1 += AWE_CANVAS_BASE_Y(canvas);
    x2 += AWE_CANVAS_BASE_X(canvas);
    y2 += AWE_CANVAS_BASE_Y(canvas);
    for(i = 0; i < width; ++i) {
        vline(bmp, x1 + i, y1 + i, y2 - 5, top_left_color);
        hline(bmp, x1 + i, y1 + i, x2 - i, top_left_color);
        vline(bmp, x2 - i, y1 + i, y2 - 5, bottom_right_color);
        line(bmp, x1 + i, y2 - 5, x1 + ((x2 - x1) / 2), y2 - i, top_left_color);
        line(bmp, x2 - i, y2 - 5, x1 + ((x2 - x1) / 2), y2 - i, bottom_right_color);
    }
}


//draws a 3d slider handle with position and size
void awe_draw_3d_slider_down_s(const AWE_CANVAS *canvas, int x, int y, int w, int h, int top_left_color, int bottom_right_color, int width)
{
    awe_draw_3d_slider_down(canvas, x, y, x + w - 1, y + h - 1, top_left_color, bottom_right_color, width);
}


//draws a 3d slider handle
void awe_draw_3d_slider_up(const AWE_CANVAS *canvas, int x1, int y1, int x2, int y2, int top_left_color, int bottom_right_color, int width)
{
    BITMAP *bmp = canvas->bitmap;
    int i;
    x1 += AWE_CANVAS_BASE_X(canvas);
    y1 += AWE_CANVAS_BASE_Y(canvas);
    x2 += AWE_CANVAS_BASE_X(canvas);
    y2 += AWE_CANVAS_BASE_Y(canvas);
    for(i = 0; i < width; ++i) {
        vline(bmp, x1 + i, y1 + 5, y2 - i, top_left_color);
        hline(bmp, x1 + i, y2 - i, x2 - i, bottom_right_color);
        vline(bmp, x2 - i, y1 + 5, y2 - i, bottom_right_color);
        line(bmp, x1 + i, y1 + 5, x1 + ((x2 - x1) / 2), y1 + i, top_left_color);
        line(bmp, x2 - i, y1 + 5, x1 + ((x2 - x1) / 2), y1 + i, bottom_right_color);
    }
}


//draws a 3d slider handle with position and size
void awe_draw_3d_slider_up_s(const AWE_CANVAS *canvas, int x, int y, int w, int h, int top_left_color, int bottom_right_color, int width)
{
    awe_draw_3d_slider_up(canvas, x, y, x + w - 1, y + h - 1, top_left_color, bottom_right_color, width);
}


//draws a 3d slider handle
void awe_draw_3d_slider_left(const AWE_CANVAS *canvas, int x1, int y1, int x2, int y2, int top_left_color, int bottom_right_color, int width)
{
    BITMAP *bmp = canvas->bitmap;
    int i;
    x1 += AWE_CANVAS_BASE_X(canvas);
    y1 += AWE_CANVAS_BASE_Y(canvas);
    x2 += AWE_CANVAS_BASE_X(canvas);
    y2 += AWE_CANVAS_BASE_Y(canvas);
    for(i = 0; i < width; ++i) {
        hline(bmp, x1 + 5, y1 + i, x2 - i, top_left_color);
        vline(bmp, x2 - i, y1 + i, y2 - i, bottom_right_color);
        hline(bmp, x1 + 5, y2 - i, x2 - i, bottom_right_color);
        line(bmp, x1 + 5, y1 + i, x1 + i, y1 + ((y2 - y1) / 2), top_left_color);
        line(bmp, x1 + 5, y2 - i, x1 + i, y1 + ((y2 - y1) / 2), bottom_right_color);
    }
}


//draws a 3d slider handle with position and size
void awe_draw_3d_slider_left_s(const AWE_CANVAS *canvas, int x, int y, int w, int h, int top_left_color, int bottom_right_color, int width)
{
    awe_draw_3d_slider_left(canvas, x, y, x + w - 1, y + h - 1, top_left_color, bottom_right_color, width);
}


//draws a 3d slider handle
void awe_draw_3d_slider_right(const AWE_CANVAS *canvas, int x1, int y1, int x2, int y2, int top_left_color, int bottom_right_color, int width)
{
    BITMAP *bmp = canvas->bitmap;
    int i;
    x1 += AWE_CANVAS_BASE_X(canvas);
    y1 += AWE_CANVAS_BASE_Y(canvas);
    x2 += AWE_CANVAS_BASE_X(canvas);
    y2 += AWE_CANVAS_BASE_Y(canvas);
    for(i = 0; i < width; ++i) {
        vline(bmp, x1 + i, y1 + i, y2 - i, top_left_color);
        hline(bmp, x1 + i, y1 + i, x2 - 5, top_left_color);
        hline(bmp, x1 + i, y2 - i, x2 - 5, bottom_right_color);
        line(bmp, x2 - 5, y1 + i, x2 - i, y1 + ((y2 - y1) / 2), top_left_color);
        line(bmp, x2 - 5, y2 - i, x2 - i, y1 + ((y2 - y1) / 2), bottom_right_color);
    }
}


//draws a 3d slider handle with position and size
void awe_draw_3d_slider_right_s(const AWE_CANVAS *canvas, int x, int y, int w, int h, int top_left_color, int bottom_right_color, int width)
{
    awe_draw_3d_slider_right(canvas, x, y, x + w - 1, y + h - 1, top_left_color, bottom_right_color, width);
}


//draws a 3d circle
void awe_draw_3d_circle(const AWE_CANVAS *canvas, int x, int y, int radius, int top_left_color, int bottom_right_color, int width)
{
    x += AWE_CANVAS_BASE_X(canvas);
    y += AWE_CANVAS_BASE_Y(canvas);
    for(; width > 0; width--, radius--) {
        do_arc(canvas->bitmap, x, y, itofix(32), itofix(160), radius, top_left_color, _fat_putpixel);
        do_arc(canvas->bitmap, x, y, itofix(160), itofix(32), radius, bottom_right_color, _fat_putpixel);
    }
}


//draws a 3d frame
void awe_draw_3d_frame(const AWE_CANVAS *canvas, int x1, int y1, int x2, int y2, int color1, int color2, int color3, int color4, AWE_FRAME_TYPE type)
{
    x1 += AWE_CANVAS_BASE_X(canvas);
    y1 += AWE_CANVAS_BASE_Y(canvas);
    x2 += AWE_CANVAS_BASE_X(canvas);
    y2 += AWE_CANVAS_BASE_Y(canvas);
    switch (type) {
        case AWE_FRAME_SUNKEN:
            _draw_3d_rect(canvas->bitmap, x1, y1, x2, y2, color3, color2, 1);
            _draw_3d_rect(canvas->bitmap, x1 + 1, y1 + 1, x2 - 1, y2 - 1, color4, color1, 1);
            break;

        case AWE_FRAME_RAISED:
            _draw_3d_rect(canvas->bitmap, x1, y1, x2, y2, color1, color4, 1);
            _draw_3d_rect(canvas->bitmap, x1 + 1, y1 + 1, x2 - 1, y2 - 1, color2, color3, 1);
            break;

        case AWE_FRAME_ETCHED_IN:
            rect(canvas->bitmap, x1, y1, x2 - 1, y2 - 1, color2);
            rect(canvas->bitmap, x1 + 1, y1 + 1, x2, y2, color1);
            break;

        case AWE_FRAME_ETCHED_OUT:
            rect(canvas->bitmap, x1 + 1, y1 + 1, x2, y2, color2);
            rect(canvas->bitmap, x1, y1, x2 - 1, y2 - 1, color1);
            break;

        default:
            rect(canvas->bitmap, x1, y1, x2, y2, color1);
    }
}


//draws a 3d frame with position and size
void awe_draw_3d_frame_s(const AWE_CANVAS *canvas, int x, int y, int w, int h, int color1, int color2, int color3, int color4, AWE_FRAME_TYPE type)
{
    awe_draw_3d_frame(canvas, x, y, x + w - 1, y + h - 1, color1, color2, color3, color4, type);
}


//draws a rectangular pattern
void awe_draw_rect_pattern(const AWE_CANVAS *canvas, int x1, int y1, int x2, int y2, int color, unsigned pt)
{
    int i;

    //offset coords
    x1 += AWE_CANVAS_BASE_X(canvas);
    y1 += AWE_CANVAS_BASE_Y(canvas);
    x2 += AWE_CANVAS_BASE_X(canvas);
    y2 += AWE_CANVAS_BASE_Y(canvas);

    //swap coords if needed
    if (x1 > x2) _SWAP(int, x1, x2);
    if (y1 > y2) _SWAP(int, y1, y2);

    //top line
    for(i = x1; i < x2; i++) {
        if (pt & 1) putpixel(canvas->bitmap, i, y1, color);
        pt = (pt >> 1) | (pt << 31);
    }

    //right line
    for(i = y1; i < y2; i++) {
        if (pt & 1) putpixel(canvas->bitmap, x2, i, color);
        pt = (pt >> 1) | (pt << 31);
    }

    //bottom line
    for(i = x2; i > x1; i--) {
        if (pt & 1) putpixel(canvas->bitmap, i, y2, color);
        pt = (pt >> 1) | (pt << 31);
    }

    //left line
    for(i = y2; i > y1; i--) {
        if (pt & 1) putpixel(canvas->bitmap, x1, i, color);
        pt = (pt >> 1) | (pt << 31);
    }
}


//draws a rectangular pattern from position and size
void awe_draw_rect_pattern_s(const AWE_CANVAS *canvas, int x, int y, int w, int h, int color, unsigned pt)
{
    awe_draw_rect_pattern(canvas, x, y, x + w - 1, y + h - 1, color, pt);
}


//draws text of specific length
int awe_draw_text_len(const AWE_CANVAS *canvas, const FONT *font, const char *text, int len, int x, int y, int fg_color, int bg_color, const char **text_r)
{
    x += AWE_CANVAS_BASE_X(canvas);
    y += AWE_CANVAS_BASE_Y(canvas);
    for(; len > 0; len--) {
        int ch = ugetc(text);
        if (!ch) break;
        x += font->vtable->render_char(font, ch, fg_color, bg_color, canvas->bitmap, x, y);
        text += ucwidth(ch);
    }
    if (text_r) *text_r = text;
    return x - AWE_CANVAS_BASE_X(canvas);
}


//draws text until coordinate is reached
int awe_draw_text_pix(const AWE_CANVAS *canvas, const FONT *font, const char *text, int x2, int x, int y, int fg_color, int bg_color, const char **text_r)
{
    int next_x;

    x += AWE_CANVAS_BASE_X(canvas);
    y += AWE_CANVAS_BASE_Y(canvas);
    x2 += AWE_CANVAS_BASE_X(canvas);
    for(;;) {
        int ch = ugetc(text);
        if (!ch) break;
        next_x = x + font->vtable->char_length(font, ch);
        if (next_x > x2) break;
        font->vtable->render_char(font, ch, fg_color, bg_color, canvas->bitmap, x, y);
        x = next_x;
        text += ucwidth(ch);
    }
    if (text_r) *text_r = text;
    return x - AWE_CANVAS_BASE_X(canvas);
}


//draws text 
int awe_draw_text(const AWE_CANVAS *canvas, const FONT *font, const char *text, int x, int y, int fg_color, int bg_color)
{
    return awe_draw_text_len(canvas, font, text, INT_MAX, x, y, fg_color, bg_color, 0);
}


//draws gui text up to specific length
int awe_draw_gui_text_len(const AWE_CANVAS *canvas, const FONT *font, const char *text, int len, int x, int y, int fg_color, int bg_color, const char **text_r)
{
    int ac = 0, ch, xx;

    //offset coords
    x += AWE_CANVAS_BASE_X(canvas);
    y += AWE_CANVAS_BASE_Y(canvas);

    //draw first part until first single '&'
    for(;;) {
        //get character
        ch = ugetc(text);

        //end of text found
        if (!ch) break;

        //draw characters
        if (ch != '&') {
            //single '&' found; draw next character underlined
            if (ac == 1) {
                xx = x + font->vtable->char_length(font, ch);
                hline(canvas->bitmap, x, y + text_height(font) + gui_font_baseline, xx - 1, fg_color);
                ac = 0;
            }

            //more than 1 '&' found; draw them
            else if (ac > 1) {
                for(; ac > 1; ac--) {
                    x += font->vtable->render_char(font, '&', fg_color, bg_color, canvas->bitmap, x, y);
                    len--;
                    if (len <= 0) break;
                }
                if (len <= 0) break;
                ac = 0;
            }

            //draw char
            x += font->vtable->render_char(font, ch, fg_color, bg_color, canvas->bitmap, x, y);
            len--;
            if (len <= 0) {
                text += ucwidth(ch);
                break;
            }
        }

        //else count '&'
        else {
            ac++;
        }

        //next text
        text += ucwidth(ch);
    }

    if (text_r) *text_r = text;
    return x - AWE_CANVAS_BASE_X(canvas);
}


//draws gui text until coordinate is reached
int awe_draw_gui_text_pix(const AWE_CANVAS *canvas, const FONT *font, const char *text, int x2, int x, int y, int fg_color, int bg_color, const char **text_r)
{
    int ac = 0, ch, xx;

    //offset coords
    x += AWE_CANVAS_BASE_X(canvas);
    y += AWE_CANVAS_BASE_Y(canvas);
    x2 += AWE_CANVAS_BASE_X(canvas);

    //draw first part until first single '&'
    for(;;) {
        //get character
        ch = ugetc(text);

        //end of text found
        if (!ch) break;

        //draw characters
        if (ch != '&') {
            //single '&' found; draw next character underlined
            if (ac == 1) {
                xx = x + font->vtable->char_length(font, ch);
                if (xx > x2) {
                    text -= ucwidth(ch);
                    break;
                }
                hline(canvas->bitmap, x, y + text_height(font) + gui_font_baseline, xx - 1, fg_color);
                ac = 0;
            }

            //more than 1 '&' found; draw them
            else if (ac > 1) {
                for(; ac > 1; ac--) {
                    xx = x + font->vtable->char_length(font, '&');
                    if (xx > x2) break;
                    x += font->vtable->render_char(font, '&', fg_color, bg_color, canvas->bitmap, x, y);
                }
                if (xx > x2) break;
                ac = 0;
            }

            //draw char
            xx = x + font->vtable->char_length(font, '&');
            if (xx > x2) break;
            x += font->vtable->render_char(font, ch, fg_color, bg_color, canvas->bitmap, x, y);
        }

        //else count '&'
        else {
            ac++;
        }

        //next text
        text += ucwidth(ch);
    }

    if (text_r) *text_r = text;
    return x - AWE_CANVAS_BASE_X(canvas);
}


//draws gui text
int awe_draw_gui_text(const AWE_CANVAS *canvas, const FONT *font, const char *text, int x, int y, int fg_color, int bg_color)
{
    return awe_draw_gui_text_len(canvas, font, text, INT_MAX, x, y, fg_color, bg_color, 0);
}


//bit-blit
void awe_blit_bitmap(BITMAP *src, const AWE_CANVAS *canvas, int src_x, int src_y, int dst_x, int dst_y, int width, int height)
{
    blit(src, canvas->bitmap, src_x, src_y, dst_x + AWE_CANVAS_BASE_X(canvas), dst_y + AWE_CANVAS_BASE_Y(canvas), width, height);
}


//masked bit-blit
void awe_blit_masked_bitmap(BITMAP *src, const AWE_CANVAS *canvas, int src_x, int src_y, int dst_x, int dst_y, int width, int height)
{
    masked_blit(src, canvas->bitmap, src_x, src_y, dst_x + AWE_CANVAS_BASE_X(canvas), dst_y + AWE_CANVAS_BASE_Y(canvas), width, height);
}


//stretch bit-blit
void awe_blit_stretch_bitmap(BITMAP *src, const AWE_CANVAS *canvas, int src_x, int src_y, int src_width, int src_height, int dst_x, int dst_y, int dst_width, int dst_height)
{
    stretch_blit(src, canvas->bitmap, src_x, src_y, src_width, src_height, dst_x + AWE_CANVAS_BASE_X(canvas), dst_y + AWE_CANVAS_BASE_Y(canvas), dst_width, dst_height);
}


//stretch masked bit-blit
void awe_blit_masked_stretch_bitmap(BITMAP *src, const AWE_CANVAS *canvas, int src_x, int src_y, int src_width, int src_height, int dst_x, int dst_y, int dst_width, int dst_height)
{
    masked_stretch_blit(src, canvas->bitmap, src_x, src_y, src_width, src_height, dst_x + AWE_CANVAS_BASE_X(canvas), dst_y + AWE_CANVAS_BASE_Y(canvas), dst_width, dst_height);
}


//monochrome bit-blit
void awe_blit_mono_bitmap(BITMAP *src, const AWE_CANVAS *canvas, int src_x, int src_y, int dst_x, int dst_y, int width, int height, int color)
{
    int i, j, p, m = bitmap_mask_color(src);

    dst_x += AWE_CANVAS_BASE_X(canvas);
    dst_y += AWE_CANVAS_BASE_Y(canvas);
    for(j = src_y; j < src_y + height; j++) {
        for(i = src_x; i < src_x + width; i++) {
            p = getpixel(src, i, j);
            if (p != m) putpixel(canvas->bitmap, i + dst_x, j + dst_y, color);
        }
    }
}


//3d bit-blit
void awe_blit_3d_bitmap(BITMAP *src, const AWE_CANVAS *canvas, int src_x, int src_y, int dst_x, int dst_y, int width, int height, int top_left_color, int bottom_right_color)
{
    int i, j, p, m = bitmap_mask_color(src);

    dst_x += AWE_CANVAS_BASE_X(canvas);
    dst_y += AWE_CANVAS_BASE_Y(canvas);
    for(j = src_y; j < src_y + height; j++) {
        for(i = src_x; i < src_x + width; i++) {
            p = getpixel(src, i, j);
            if (p != m) putpixel(canvas->bitmap, i + dst_x, j + dst_y, top_left_color);
            else {
                p = getpixel(src, i - 1, j - 1);
                if (p > 0 && p != m) putpixel(canvas->bitmap, i + dst_x, j + dst_y, bottom_right_color);
            }
        }
    }
}


//draws a left arrow icon
void awe_draw_left_arrow_icon(const AWE_CANVAS *canvas, int x, int y, int radius, int color)
{
    x += AWE_CANVAS_BASE_X(canvas);
    y += AWE_CANVAS_BASE_Y(canvas); {
    int quarter_size = radius >> 1;
    int x1 = x - quarter_size + 1;
    int x2 = x1 + radius - 1;
    int i;
    hline(canvas->bitmap, x1, y, x2, color);
    for(i = 1; i < radius; i++) {
        hline(canvas->bitmap, x1 + i, y - i, x2, color);
        hline(canvas->bitmap, x1 + i, y + i, x2, color);        
    }
    }    
}


//draws a right arrow icon
void awe_draw_right_arrow_icon(const AWE_CANVAS *canvas, int x, int y, int radius, int color)
{
    x += AWE_CANVAS_BASE_X(canvas);
    y += AWE_CANVAS_BASE_Y(canvas); {
    int quarter_size = radius >> 1;
    int x1 = x - quarter_size + 1;
    int x2 = x1 + radius - 1;
    int i;
    hline(canvas->bitmap, x1, y, x2, color);
    for(i = 1; i < radius; i++) {
        hline(canvas->bitmap, x1, y - i, x2 - i, color);
        hline(canvas->bitmap, x1, y + i, x2 - i, color);        
    }
    }
}


//draws an up arrow icon
void awe_draw_up_arrow_icon(const AWE_CANVAS *canvas, int x, int y, int radius, int color)
{
    x += AWE_CANVAS_BASE_X(canvas);
    y += AWE_CANVAS_BASE_Y(canvas); {
    int quarter_size = radius >> 1;
    int y1 = y - quarter_size;
    int y2 = y1 + radius - 1;
    int i;
    vline(canvas->bitmap, x, y1, y2, color);
    for(i = 1; i < radius; i++) {
        vline(canvas->bitmap, x - i, y1 + i, y2, color);
        vline(canvas->bitmap, x + i, y1 + i, y2, color);
    }
    }    
}


//draws a down arrow icon
void awe_draw_down_arrow_icon(const AWE_CANVAS *canvas, int x, int y, int radius, int color)
{
    x += AWE_CANVAS_BASE_X(canvas);
    y += AWE_CANVAS_BASE_Y(canvas); {
    int quarter_size = radius >> 1;
    int y1 = y - quarter_size + 1;
    int y2 = y1 + radius - 1;
    int i;
    vline(canvas->bitmap, x, y1, y2, color);
    for(i = 1; i < radius; i++) {
        vline(canvas->bitmap, x - i, y1, y2 - i, color);
        vline(canvas->bitmap, x + i, y1, y2 - i, color);
    }
    }    
}


//draws a tick icon
void awe_draw_tick_icon(const AWE_CANVAS *canvas, int x, int y, int radius, int color, int width)
{
    x += AWE_CANVAS_BASE_X(canvas);
    y += AWE_CANVAS_BASE_Y(canvas);
    if (width == 0) width = MAX(radius / 4, 2); {
    int size = radius << 1;
    int third_size = size / 3;
    int two_thirds_size = size - third_size;
    int lx = x - radius;
    int mx = lx + third_size;
    int my = y + radius;
    int ly = my - third_size;
    int rx = mx + two_thirds_size;
    int ry = my - two_thirds_size;
    for(; width > 0; width--, my--, ly--, ry--) {
        line(canvas->bitmap, mx, my, lx, ly, color);
        line(canvas->bitmap, mx, my, rx, ry, color);
    }
    }
}


//draws a close icon
void awe_draw_close_icon(const AWE_CANVAS *canvas, int x, int y, int radius, int color, int width)
{
    x += AWE_CANVAS_BASE_X(canvas);
    y += AWE_CANVAS_BASE_Y(canvas);
    if (width == 0) width = MAX(radius / 4, 2); {
    int x1 = x - radius;
    int y1 = y - radius;
    int x2 = x + radius;
    int y2 = y + radius;
    int i;
    for(i = 0; i < width; i++) {
        line(canvas->bitmap, x1 + i, y1, x2, y2 - i, color);
        line(canvas->bitmap, x1, y1 + i, x2 - i, y2, color);
        line(canvas->bitmap, x2 - i, y1, x1, y2 - i, color);
        line(canvas->bitmap, x2, y1 + i, x1 + i, y2, color);
    }
    }
}


//draws a minimize icon
void awe_draw_minimize_icon(const AWE_CANVAS *canvas, int x, int y, int radius, int color, int width)
{
    x += AWE_CANVAS_BASE_X(canvas);
    y += AWE_CANVAS_BASE_Y(canvas);
    if (width == 0) width = MAX(radius / 4, 2); {
    int length = 4 * radius / 3;
    int x1 = x - radius;
    int y2 = y + radius;
    int y1 = y2 - width + 1;
    int x2 = x1 + length + 1;
    rectfill(canvas->bitmap, x1, y1, x2, y2, color);
    }
}


//draws a maximize icon
void awe_draw_maximize_icon(const AWE_CANVAS *canvas, int x, int y, int radius, int color, int width)
{
    x += AWE_CANVAS_BASE_X(canvas);
    y += AWE_CANVAS_BASE_Y(canvas);
    if (width == 0) width = MAX(radius / 4, 2); {
    int x1 = x - radius;
    int y1 = y - radius;
    int x2 = x + radius;
    int y2 = y + radius;
    int yy1 = y1 + width - 1;
    int yy2 = y2 - width + 1;
    rectfill(canvas->bitmap, x1, y1, x2, yy1, color);
    rectfill(canvas->bitmap, x1, yy1, x2, yy1 + width - 1, color);
    rectfill(canvas->bitmap, x1, yy1, x1 + width - 1, yy2, color);
    rectfill(canvas->bitmap, x2 - width + 1, yy1, x2, yy2, color);
    rectfill(canvas->bitmap, x1, yy2, x2, y2, color);
    }
}


//draws a restore icon
void awe_draw_restore_icon(const AWE_CANVAS *canvas, int x, int y, int radius, int color, int width)
{
    x += AWE_CANVAS_BASE_X(canvas);
    y += AWE_CANVAS_BASE_Y(canvas);
    if (width == 0) width = MAX(radius / 4, 2); {
    int half_width = MAX(width >> 1, 1);
    int width2 = width * 2;
    int width3 = width * 3;
    int x1 = x - radius;
    int y1 = y - radius;
    int x2 = x + radius;
    int y2 = y + radius;
    rectfill(canvas->bitmap, x1, y1 + width2, x2 - width2, y1 + width3 + half_width - 1, color);
    rectfill(canvas->bitmap, x1, y1 + width2, x1 + width - 1, y2 - width, color);
    rectfill(canvas->bitmap, x2 - width3 + 1, y1 + width3, x2 - width2, y2 - width, color);
    rectfill(canvas->bitmap, x1, y2 - width + 1, x2 - width2, y2, color);
    rectfill(canvas->bitmap, x1 + width2, y1, x2, y1 + width + half_width - 1, color);
    rectfill(canvas->bitmap, x2 - width + 1, y1, x2, y2 - width2, color);
    rectfill(canvas->bitmap, x2 - width2 + half_width, y2 - width3 + 1, x2, y2 - width2, color);
    }
}


//creates a texture
AWE_TEXTURE *awe_create_texture(BITMAP *src, int x, int y, int w, int h, int left_side, int top_side, int right_side, int bottom_side)
{
    AWE_TEXTURE *tex;

    //allocate
    tex = calloc(1, sizeof(AWE_TEXTURE));
    if (!tex) return 0;

    //set sides
    tex->side[_LEFT] = MAX(left_side, 0);
    tex->side[_TOP] = MAX(top_side, 0);
    tex->side[_RIGHT] = MAX(right_side, 0);
    tex->side[_BOTTOM] = MAX(bottom_side, 0);

    //create sub-bitmaps
    tex->bitmap[0] = _create_sub_bitmap(src, x, y, tex->side[_LEFT], tex->side[_TOP]);
    tex->bitmap[1] = _create_sub_bitmap(src, x + tex->side[_LEFT], y, w - tex->side[_LEFT] - tex->side[_RIGHT], tex->side[_TOP]);
    tex->bitmap[2] = _create_sub_bitmap(src, x + w - tex->side[_RIGHT], y, tex->side[_RIGHT], tex->side[_TOP]);
    tex->bitmap[3] = _create_sub_bitmap(src, x, y + tex->side[_TOP], tex->side[_LEFT], h - tex->side[_TOP] - tex->side[_BOTTOM]);
    tex->bitmap[4] = _create_sub_bitmap(src, x + tex->side[_LEFT], y + tex->side[_TOP], w - tex->side[_LEFT] - tex->side[_RIGHT], h - tex->side[_TOP] - tex->side[_BOTTOM]);
    tex->bitmap[5] = _create_sub_bitmap(src, x + w - tex->side[_RIGHT], y + tex->side[_TOP], tex->side[_RIGHT], h - tex->side[_TOP] - tex->side[_BOTTOM]);
    tex->bitmap[6] = _create_sub_bitmap(src, x, y + h - tex->side[_BOTTOM], tex->side[_LEFT], tex->side[_BOTTOM]);
    tex->bitmap[7] = _create_sub_bitmap(src, x + tex->side[_LEFT], y + h - tex->side[_BOTTOM], w - tex->side[_LEFT] - tex->side[_RIGHT], tex->side[_BOTTOM]);
    tex->bitmap[8] = _create_sub_bitmap(src, x + w - tex->side[_RIGHT], y + h - tex->side[_BOTTOM], tex->side[_RIGHT], tex->side[_BOTTOM]);

    return tex;
}


//destroys a texture
void awe_destroy_texture(AWE_TEXTURE *texture)
{
    int i;

    for(i = 0; i < 9; i++) destroy_bitmap(texture->bitmap[i]);
    free(texture);
}


//draws a texture
void awe_draw_texture(const AWE_CANVAS *canvas, const AWE_TEXTURE *tex, int x1, int y1, int x2, int y2)
{
    int w, h;

    if (x1 > x2) _SWAP(int, x1, x2);
    if (y1 > y2) _SWAP(int, y1, y2);
    x1 += AWE_CANVAS_BASE_X(canvas);
    y1 += AWE_CANVAS_BASE_Y(canvas);
    x2 += AWE_CANVAS_BASE_X(canvas);
    y2 += AWE_CANVAS_BASE_Y(canvas);
    w = x2 - x1 + 1 - tex->side[_LEFT] - tex->side[_RIGHT];
    h = y2 - y1 + 1 - tex->side[_TOP] - tex->side[_BOTTOM];

    if (tex->bitmap[0])
        masked_blit(tex->bitmap[0], canvas->bitmap, 0, 0, x1, y1, tex->side[_LEFT], tex->side[_TOP]);
    _fill(canvas->bitmap, tex->bitmap[1], x1 + tex->side[_LEFT], y1, w, tex->side[_TOP]);
    if (tex->bitmap[2])
        masked_blit(tex->bitmap[2], canvas->bitmap, 0, 0, x2 - tex->side[_RIGHT] + 1, y1, tex->side[_RIGHT], tex->side[_TOP]);
    _fill(canvas->bitmap, tex->bitmap[3], x1, y1 + tex->side[_TOP], tex->side[_LEFT], h);
    _fill(canvas->bitmap, tex->bitmap[4], x1 + tex->side[_LEFT], y1 + tex->side[_TOP], w, h);
    _fill(canvas->bitmap, tex->bitmap[5], x2 - tex->side[_RIGHT] + 1, y1 + tex->side[_TOP], tex->side[_RIGHT], h);
    if (tex->bitmap[6])
        masked_blit(tex->bitmap[6], canvas->bitmap, 0, 0, x1, y2 - tex->side[_BOTTOM] + 1, tex->side[_LEFT], tex->side[_BOTTOM]);
    _fill(canvas->bitmap, tex->bitmap[7], x1 + tex->side[_LEFT], y2 - tex->side[_BOTTOM] + 1, w, tex->side[_BOTTOM]);
    if (tex->bitmap[8])
        masked_blit(tex->bitmap[8], canvas->bitmap, 0, 0, x2 - tex->side[_RIGHT] + 1, y2 - tex->side[_BOTTOM] + 1, tex->side[_RIGHT], tex->side[_BOTTOM]);
}


//draws a texture with size and position
void awe_draw_texture_s(const AWE_CANVAS *canvas, const AWE_TEXTURE *tex, int x, int y, int w, int h)
{
    awe_draw_texture(canvas, tex, x, y, x + w - 1, y + h - 1);
}


//draws a texture horizontally
void awe_draw_texture_hor(const AWE_CANVAS *canvas, const AWE_TEXTURE *tex, int x1, int y1, int x2, int y2)
{
    AWE_RECT old_clip;
    AWE_RECT new_clip;
    int w, h, x3;

    if (!tex->bitmap[4]) return;
    if (x1 > x2) _SWAP(int, x1, x2);
    if (y1 > y2) _SWAP(int, y1, y2);
    x1 += AWE_CANVAS_BASE_X(canvas);
    y1 += AWE_CANVAS_BASE_Y(canvas);
    x2 += AWE_CANVAS_BASE_X(canvas);
    y2 += AWE_CANVAS_BASE_Y(canvas);
    x3 = x1;
    w = x2 - x1 + 1 - tex->side[_LEFT] - tex->side[_RIGHT];
    h = y2 - y1 + 1 - tex->side[_TOP] - tex->side[_BOTTOM];

    if (tex->bitmap[0])
        masked_blit(tex->bitmap[0], canvas->bitmap, 0, 0, x1, y1, tex->side[_LEFT], tex->side[_TOP]);
    if (tex->bitmap[1])
        _fill(canvas->bitmap, tex->bitmap[1], x1 + tex->side[_LEFT], y1, w, tex->side[_TOP]);
    if (tex->bitmap[2])
        masked_blit(tex->bitmap[2], canvas->bitmap, 0, 0, x2 - tex->side[_RIGHT] + 1, y1, tex->side[_RIGHT], tex->side[_TOP]);

    //draw left border
    if (tex->bitmap[3]) {
        _stretch(canvas->bitmap, tex->bitmap[3], x3, y1 + tex->side[_TOP], tex->side[_LEFT], h);
        x3 += tex->bitmap[3]->w;
    }

    awe_get_canvas_clip((AWE_CANVAS*)canvas, &old_clip);
    new_clip.left = x1;
    new_clip.top = y1 + tex->side[_TOP];
    new_clip.right = x2;
    new_clip.bottom = y2 - tex->side[_BOTTOM];    
    awe_set_canvas_clip((AWE_CANVAS*)canvas, &new_clip);

    do{
        _stretch(canvas->bitmap, tex->bitmap[4], x3, y1 + tex->side[_TOP], tex->bitmap[4]->w, h);//x1 + tex->side[_LEFT], y3, w, tex->bitmap[4]->h);
        x3 += tex->bitmap[4]->h;
    }while(x3 < x2);

    awe_set_canvas_clip((AWE_CANVAS*)canvas, &old_clip);

    //draw right border
    if (tex->bitmap[5]) {
        _stretch(canvas->bitmap, tex->bitmap[5], x2 - tex->side[_RIGHT] + 1, y1 + tex->side[_TOP], tex->side[_RIGHT], h);
    }

    if (tex->bitmap[6])
        masked_blit(tex->bitmap[6], canvas->bitmap, 0, 0, x1, y2 - tex->side[_BOTTOM] + 1, tex->side[_LEFT], tex->side[_BOTTOM]);
    if (tex->bitmap[7])    
        _fill(canvas->bitmap, tex->bitmap[7], x1 + tex->side[_LEFT], y2 - tex->side[_BOTTOM] + 1, w, tex->side[_BOTTOM]);
    if (tex->bitmap[8])
        masked_blit(tex->bitmap[8], canvas->bitmap, 0, 0, x2 - tex->side[_RIGHT] + 1, y2 - tex->side[_BOTTOM] + 1, tex->side[_RIGHT], tex->side[_BOTTOM]);
}


//draws a texture horizontally with size and position
void awe_draw_texture_hor_s(const AWE_CANVAS *canvas, const AWE_TEXTURE *tex, int x, int y, int w, int h)
{
    awe_draw_texture_hor(canvas, tex, x, y, x + w - 1, y + h - 1);
}


//draws a texture vertically
void awe_draw_texture_ver(const AWE_CANVAS *canvas, const AWE_TEXTURE *tex, int x1, int y1, int x2, int y2)
{
    AWE_RECT old_clip;
    AWE_RECT new_clip;
    int w, h, y3;
    if (!tex->bitmap[4]) return;
    if (x1 > x2) _SWAP(int, x1, x2);
    if (y1 > y2) _SWAP(int, y1, y2);
    x1 += AWE_CANVAS_BASE_X(canvas);
    y1 += AWE_CANVAS_BASE_Y(canvas);
    x2 += AWE_CANVAS_BASE_X(canvas);
    y2 += AWE_CANVAS_BASE_Y(canvas);
    y3 = y1;
    w = x2 - x1 + 1 - tex->side[_LEFT] - tex->side[_RIGHT];
    h = y2 - y1 + 1 - tex->side[_TOP] - tex->side[_BOTTOM];

    if (tex->bitmap[0])
        masked_blit(tex->bitmap[0], canvas->bitmap, 0, 0, x1, y1, tex->side[_LEFT], tex->side[_TOP]);
    if (tex->bitmap[3])
        _fill(canvas->bitmap, tex->bitmap[3], x1, y1 + tex->side[_TOP], tex->side[_LEFT], h);
    if (tex->bitmap[6])
        masked_blit(tex->bitmap[6], canvas->bitmap, 0, 0, x1, y2 - tex->side[_BOTTOM] + 1, tex->side[_LEFT], tex->side[_BOTTOM]);

    //draw top border
    if (tex->bitmap[1]) {
        _stretch(canvas->bitmap, tex->bitmap[1], x1 + tex->side[_LEFT], y1, w, tex->side[_TOP]);
        y3 += tex->bitmap[1]->h;
    }

    awe_get_canvas_clip((AWE_CANVAS*)canvas, &old_clip);
    new_clip.left = x1;
    new_clip.top = y1 + tex->side[_TOP];
    new_clip.right = x2;
    new_clip.bottom = y2 - tex->side[_BOTTOM];    
    awe_set_canvas_clip((AWE_CANVAS*)canvas, &new_clip);

    do{
        _stretch(canvas->bitmap, tex->bitmap[4], x1 + tex->side[_LEFT], y3, w, tex->bitmap[4]->h);
        y3 += tex->bitmap[4]->h;
    }while(y3 < y2);

    awe_set_canvas_clip((AWE_CANVAS*)canvas, &old_clip);

    //draw bottom border
    if (tex->bitmap[7]) {
        _stretch(canvas->bitmap, tex->bitmap[7], x1 + tex->side[_LEFT], y2 - tex->side[_BOTTOM] + 1, w, tex->side[_BOTTOM]);
    }

    if (tex->bitmap[2])
        masked_blit(tex->bitmap[2], canvas->bitmap, 0, 0, x2 - tex->side[_RIGHT] + 1, y1, tex->side[_RIGHT], tex->side[_TOP]);
    if (tex->bitmap[5])    
        _fill(canvas->bitmap, tex->bitmap[5], x2 - tex->side[_RIGHT] + 1, y1 + tex->side[_TOP], tex->side[_RIGHT], h);
    if (tex->bitmap[8])
        masked_blit(tex->bitmap[8], canvas->bitmap, 0, 0, x2 - tex->side[_RIGHT] + 1, y2 - tex->side[_BOTTOM] + 1, tex->side[_RIGHT], tex->side[_BOTTOM]);
}


//draws a texture vertically with size and position
void awe_draw_texture_ver_s(const AWE_CANVAS *canvas, const AWE_TEXTURE *tex, int x, int y, int w, int h)
{
    awe_draw_texture_ver(canvas, tex, x, y, x + w - 1, y + h - 1);
}


//draws a texture vertically
void awe_draw_texture_hor_ver(const AWE_CANVAS *canvas, const AWE_TEXTURE *tex, int x1, int y1, int x2, int y2)
{
    AWE_RECT old_clip;
    AWE_RECT new_clip;
    int w, h, y3;
    if (!tex->bitmap[4]) return;
    if (x1 > x2) _SWAP(int, x1, x2);
    if (y1 > y2) _SWAP(int, y1, y2);
    x1 += AWE_CANVAS_BASE_X(canvas);
    y1 += AWE_CANVAS_BASE_Y(canvas);
    x2 += AWE_CANVAS_BASE_X(canvas);
    y2 += AWE_CANVAS_BASE_Y(canvas);
    y3 = y1;
    w = x2 - x1 + 1 - tex->side[_LEFT] - tex->side[_RIGHT];
    h = y2 - y1 + 1 - tex->side[_TOP] - tex->side[_BOTTOM];

    if (tex->bitmap[0])
        masked_blit(tex->bitmap[0], canvas->bitmap, 0, 0, x1, y1, tex->side[_LEFT], tex->side[_TOP]);
    if (tex->bitmap[3])
        _stretch(canvas->bitmap, tex->bitmap[3], x1, y1 + tex->side[_TOP], tex->side[_LEFT], h);
    if (tex->bitmap[6])
        masked_blit(tex->bitmap[6], canvas->bitmap, 0, 0, x1, y2 - tex->side[_BOTTOM] + 1, tex->side[_LEFT], tex->side[_BOTTOM]);

    //draw top border
    if (tex->bitmap[1]) {
        _stretch(canvas->bitmap, tex->bitmap[1], x1 + tex->side[_LEFT], y1, w, tex->side[_TOP]);
        y3 += tex->bitmap[1]->h;
    }

    awe_get_canvas_clip((AWE_CANVAS*)canvas, &old_clip);
    new_clip.left = x1;
    new_clip.top = y1 + tex->side[_TOP];
    new_clip.right = x2;
    new_clip.bottom = y2 - tex->side[_BOTTOM];    
    awe_set_canvas_clip((AWE_CANVAS*)canvas, &new_clip);

    _stretch(canvas->bitmap, tex->bitmap[4], x1 + tex->side[_LEFT], y3, w, h);

    awe_set_canvas_clip((AWE_CANVAS*)canvas, &old_clip);

    //draw bottom border
    if (tex->bitmap[7]) {
        _stretch(canvas->bitmap, tex->bitmap[7], x1 + tex->side[_LEFT], y2 - tex->side[_BOTTOM] + 1, w, tex->side[_BOTTOM]);
    }

    if (tex->bitmap[2])
        masked_blit(tex->bitmap[2], canvas->bitmap, 0, 0, x2 - tex->side[_RIGHT] + 1, y1, tex->side[_RIGHT], tex->side[_TOP]);
    if (tex->bitmap[5])    
        _stretch(canvas->bitmap, tex->bitmap[5], x2 - tex->side[_RIGHT] + 1, y1 + tex->side[_TOP], tex->side[_RIGHT], h);
    if (tex->bitmap[8])
        masked_blit(tex->bitmap[8], canvas->bitmap, 0, 0, x2 - tex->side[_RIGHT] + 1, y2 - tex->side[_BOTTOM] + 1, tex->side[_RIGHT], tex->side[_BOTTOM]);
}


//draws a texture vertically with size and position
void awe_draw_texture_hor_ver_s(const AWE_CANVAS *canvas, const AWE_TEXTURE *tex, int x, int y, int w, int h)
{
    awe_draw_texture_hor_ver(canvas, tex, x, y, x + w - 1, y + h - 1);
}


//draws a texture based on type
void awe_draw_texture_type(const AWE_CANVAS *canvas, const AWE_TEXTURE *tex, int x1, int y1, int x2, int y2, AWE_TEXTURE_TYPE h_type, AWE_TEXTURE_TYPE v_type)
{
    int type = h_type << 1;
    type |= v_type;
    switch(type){
        case 0:
            awe_draw_texture(canvas, tex, x1, y1, x2, y2);
        break;
        case 1:
            awe_draw_texture_hor(canvas, tex, x1, y1, x2, y2);
        break;
        case 2:
            awe_draw_texture_ver(canvas, tex, x1, y1, x2, y2);
        break;
        case 3:
            awe_draw_texture_hor_ver(canvas, tex, x1, y1, x2, y2);
        break;
    }
}


//draws a texture based on type with size and position
void awe_draw_texture_type_s(const AWE_CANVAS *canvas, const AWE_TEXTURE *tex, int x, int y, int w, int h, AWE_TEXTURE_TYPE h_type, AWE_TEXTURE_TYPE v_type)
{
    awe_draw_texture_type(canvas, tex, x, y, x + w - 1, y + h - 1, h_type, v_type);
}


//draws a shadow 
void awe_draw_shadow(const AWE_CANVAS *canvas, int x1, int y1, int x2, int y2, int start_trans, int end_trans, int hor)
{
    double trans_delta, trans;
    int i, black;

    start_trans = MID(0, start_trans, 255);
    end_trans = MID(0, end_trans, 255);
    if (x1 > x2) _SWAP(int, x1, x2);
    if (y1 > y2) _SWAP(int, y1, y2);
    x1 += AWE_CANVAS_BASE_X(canvas);
    y1 += AWE_CANVAS_BASE_Y(canvas);
    x2 += AWE_CANVAS_BASE_X(canvas);
    y2 += AWE_CANVAS_BASE_Y(canvas);
    black = makecol_depth(bitmap_color_depth(canvas->bitmap), 0, 0, 0);
    drawing_mode(DRAW_MODE_TRANS, 0, 0, 0);
    acquire_bitmap(canvas->bitmap);
    if (hor) {
        double w = x2 - x1 + 1;
        trans_delta = ((double)(end_trans - start_trans + 1)) / w;
        trans = start_trans;
        for(i = x1; i <= x2; i++) {
            set_trans_blender(0, 0, 0, (int)trans);
            vline(canvas->bitmap, i, y1, y2, black);
            trans += trans_delta;
            if (trans < 0) trans = 0; else if (trans > 255) trans = 255;
        }
    }
    else {
        double h = y2 - y1 + 1;
        trans_delta = ((double)(end_trans - start_trans + 1)) / h;
        trans = start_trans;
        for(i = y1; i <= y2; ++i) {
            set_trans_blender(0, 0, 0, (int)trans);
            hline(canvas->bitmap, x1, i, x2, black);
            trans += trans_delta;
            if (trans < 0) trans = 0; else if (trans > 255) trans = 255;
        }
    }
    release_bitmap(canvas->bitmap);
}


//draws a shadow using position and size
void awe_draw_shadow_s(const AWE_CANVAS *canvas, int x, int y, int w, int h, int start_trans, int end_trans, int hor)
{
    awe_draw_shadow(canvas, x, y, x + w - 1, y + h - 1, start_trans, end_trans, hor);
}


//draws a bottom right shadow 
void awe_draw_bottom_right_shadow(const AWE_CANVAS *canvas, int x1, int y1, int x2, int y2, int start_trans, int end_trans, int width)
{
    double trans_delta, trans;
    int black;

    if (width <= 0) return;
    start_trans = MID(0, start_trans, 255);
    end_trans = MID(0, end_trans, 255);
    if (x1 > x2) _SWAP(int, x1, x2);
    if (y1 > y2) _SWAP(int, y1, y2);
    x1 += AWE_CANVAS_BASE_X(canvas);
    y1 += AWE_CANVAS_BASE_Y(canvas);
    x2 += AWE_CANVAS_BASE_X(canvas);
    y2 += AWE_CANVAS_BASE_Y(canvas);
    black = makecol_depth(bitmap_color_depth(canvas->bitmap), 0, 0, 0);
    drawing_mode(DRAW_MODE_TRANS, 0, 0, 0);
    acquire_bitmap(canvas->bitmap);
    trans_delta = ((double)(end_trans - start_trans + 1)) / ((double)width);
    trans = start_trans;
    for(--width; width >= 0; --width) {
        set_trans_blender(0, 0, 0, (int)trans);
        vline(canvas->bitmap, x2 - width, y1, y2 - width, black);
        hline(canvas->bitmap, x1, y2 - width, x2 - width - 1, black);
        trans += trans_delta;
        if (trans < 0) trans = 0; else if (trans > 255) trans = 255;
    }
    release_bitmap(canvas->bitmap);
}


//draws a shadow using position and size
void awe_draw_bottom_right_shadow_s(const AWE_CANVAS *canvas, int x, int y, int w, int h, int start_trans, int end_trans, int width)
{
    awe_draw_bottom_right_shadow(canvas, x, y, x + w - 1, y + h - 1, start_trans, end_trans, width);
}
