#ifndef AWE_GDI_H
#define AWE_GDI_H


#include "allegro.h"


#ifdef __cplusplus
   extern "C" {
#endif


/**@name GDI
    The purpose of the GDI module is to provide a set of routines that are
    used for drawing from inside widgets. This layer is necessary because
    Allegro does not provide drawing with an offset. Drawing is done with a
    help of the AWE_CANVAS object, which represents a drawing surface. This
    module provides the regular allegro functions as well as additional 
    functions for drawing more complex shapes. It also provides skin texture
    management and drawing functions.
 */
/*@{*/


///type of frame to draw
enum AWE_FRAME_TYPE {
    ///flat frame
    AWE_FRAME_FLAT,

    ///sunken 3d frame
    AWE_FRAME_SUNKEN,

    ///raised 3d frame
    AWE_FRAME_RAISED,

    ///etched in frame
    AWE_FRAME_ETCHED_IN,

    ///etched out frame
    AWE_FRAME_ETCHED_OUT
};
typedef enum AWE_FRAME_TYPE AWE_FRAME_TYPE;


///solid line pattern
#define AWE_PATTERN_SOLID         0xFFFFFFFFU    //_____


//dot-space-dot-space line pattern
#define AWE_PATTERN_DOT_DOT       0x55555555U    //.....


//dash-space-dash-space line pattern
#define AWE_PATTERN_DASH_DASH     0x33333333U    //-----


/** macro for calculating the horizontal screen-relative offset of a canvas
    @param C pointer to canvas
    @return horizontal coordinate that is the current canvas offset, relative
            to the destination bitmaps' top-left corner
 */
#define AWE_CANVAS_BASE_X(C)      ((C)->x_org + (C)->x_ofs)


/** macro for calculating the vertical screen-relative offset of a canvas
    @param C pointer to canvas
    @return vertical coordinate that is the current canvas offset, relative
            to the destination bitmaps' top-left corner
 */
#define AWE_CANVAS_BASE_Y(C)      ((C)->y_org + (C)->y_ofs)


/** canvas object used for drawing.
 */
struct AWE_CANVAS {
    ///target bitmap
    BITMAP *bitmap;

    ///horizontal point of origin
    int x_org;

    ///vertical point of origin
    int y_org;

    ///horizontal offset from point of origin
    int x_ofs;

    ///vertical offset from point of origin
    int y_ofs;
};
typedef struct AWE_CANVAS AWE_CANVAS;


/** object used for skinning
 */
struct AWE_TEXTURE {
    int side[4];
    BITMAP *bitmap[9];
};
typedef struct AWE_TEXTURE AWE_TEXTURE;


/** sets the canva's 'point of origin' (actually, the canvas offset from point
    of origin)
    @param canvas canvas object to modify
    @param x new horizontal point of origin
    @param y new vertical point of origin
 */
void awe_set_canvas_origin(AWE_CANVAS *canvas, int x, int y);


/** draws a pixel
    @param canvas destination canvas
    @param x horizontal coordinate
    @param y vertical coordinate
    @param color pixel value
 */
void awe_draw_pixel(AL_CONST AWE_CANVAS *canvas, int x, int y, int color);


/** draws a horizontal line
    @param canvas destination canvas
    @param x1 left coordinate
    @param y vertical coordinate
    @param x2 right coordinate
    @param color pixel value
 */
void awe_draw_hline(AL_CONST AWE_CANVAS *canvas, int x1, int y, int x2, int color);


/** draws a vertical line
    @param canvas destination canvas
    @param x horizontal coordinate
    @param y1 top coordinate
    @param y2 bottom coordinate
    @param color pixel value
 */
void awe_draw_vline(AL_CONST AWE_CANVAS *canvas, int x, int y1, int y2, int color);


/** draws a line
    @param canvas destination canvas
    @param x1 left coordinate
    @param y1 top coordinate
    @param x2 right coordinate
    @param y2 bottom coordinate
    @param color pixel value
 */
void awe_draw_line(AL_CONST AWE_CANVAS *canvas, int x1, int y1, int x2, int y2, int color);


/** draws a rectangle
    @param canvas destination canvas
    @param x1 left coordinate
    @param y1 top coordinate
    @param x2 right coordinate
    @param y2 bottom coordinate
    @param color pixel value
 */
void awe_draw_rect(AL_CONST AWE_CANVAS *canvas, int x1, int y1, int x2, int y2, int color);


/** draws a rectangle with position and size
    @param canvas destination canvas
    @param x left coordinate
    @param y top coordinate
    @param w width
    @param h height
    @param color pixel value
 */
void awe_draw_rect_s(AL_CONST AWE_CANVAS *canvas, int x, int y, int w, int h, int color);


/** draws a circle
    @param canvas destination canvas
    @param x horizontal center
    @param y vertical center
    @param radius radius of circle
    @param color pixel value
 */
void awe_draw_circle(AL_CONST AWE_CANVAS *canvas, int x, int y, int radius, int color);


/** draws an ellipse
    @param canvas destination canvas
    @param x horizontal center
    @param y vertical center
    @param hor_radius horizontal radius of ellipse
    @param ver_radius vertical radius of ellipse
    @param color pixel value
 */
void awe_draw_ellipse(AL_CONST AWE_CANVAS *canvas, int x, int y, int hor_radius, int ver_radius, int color);


/** draws an arc
    @param canvas destination canvas
    @param x horizontal center
    @param y vertical center
    @param start_angle start angle, in allegro's 'radians' (0 to 256) as fixed
    @param end_angle end angle, in allegro's 'radians' (0 to 256) as fixed
    @param radius radius of circle
    @param color pixel value
 */
void awe_draw_arc(AL_CONST AWE_CANVAS *canvas, int x, int y, fixed start_angle, fixed end_angle, int radius, int color);


/** draws a spline
    @param canvas destination canvas
    @param points the spline's control points
    @param color pixel value
 */
void awe_draw_spline(AL_CONST AWE_CANVAS *canvas, AL_CONST int points[8], int color);


/** fills a rectangle
    @param canvas destination canvas
    @param x1 left coordinate
    @param y1 top coordinate
    @param x2 right coordinate
    @param y2 bottom coordinate
    @param color pixel value
 */
void awe_fill_rect(AL_CONST AWE_CANVAS *canvas, int x1, int y1, int x2, int y2, int color);


/** fills a rectangle with position and size
    @param canvas destination canvas
    @param x left coordinate
    @param y top coordinate
    @param w width
    @param h height
    @param color pixel value
 */
void awe_fill_rect_s(AL_CONST AWE_CANVAS *canvas, int x, int y, int w, int h, int color);


/** fills a circle
    @param canvas destination canvas
    @param x horizontal center
    @param y vertical center
    @param radius radius of circle
    @param color pixel value
 */
void awe_fill_circle(AL_CONST AWE_CANVAS *canvas, int x, int y, int radius, int color);


/** fills an ellipse
    @param canvas destination canvas
    @param x horizontal center
    @param y vertical center
    @param hor_radius horizontal radius of ellipse
    @param ver_radius vertical radius of ellipse
    @param color pixel value
 */
void awe_fill_ellipse(AL_CONST AWE_CANVAS *canvas, int x, int y, int hor_radius, int ver_radius, int color);


/** rectangular gradient fill (using 'quad3d')
    @param canvas destination canvas
    @param x1 left coordinate
    @param y1 top coordinate
    @param x2 right coordinate
    @param y2 bottom coordinate
    @param color1 top-left color
    @param color2 bottom-left color
    @param color3 bottom-right color
    @param color4 top-right color
 */
void awe_fill_gradient(AL_CONST AWE_CANVAS *canvas, int x1, int y1, int x2, int y2, int color1, int color2, int color3, int color4);


/** rectangular gradient fill (using 'quad3d') using position and size
    @param canvas destination canvas
    @param x left coordinate
    @param y top coordinate
    @param w width
    @param h height
    @param color1 top-left color
    @param color2 bottom-left color
    @param color3 bottom-right color
    @param color4 top-right color
 */
void awe_fill_gradient_s(AL_CONST AWE_CANVAS *canvas, int x, int y, int w, int h, int color1, int color2, int color3, int color4);


/** horizontal gradient fill (using 'quad3d')
    @param canvas destination canvas
    @param x1 left coordinate
    @param y1 top coordinate
    @param x2 right coordinate
    @param y2 bottom coordinate
    @param color1 left color
    @param color2 right color
 */
void awe_fill_gradient_hor(AL_CONST AWE_CANVAS *canvas, int x1, int y1, int x2, int y2, int color1, int color2);


/** horizontal gradient fill (using 'quad3d') using position and size
    @param canvas destination canvas
    @param x left coordinate
    @param y top coordinate
    @param w width
    @param h height
    @param color1 left color
    @param color2 right color
 */
void awe_fill_gradient_hor_s(AL_CONST AWE_CANVAS *canvas, int x, int y, int w, int h, int color1, int color2);


/** vertical gradient fill (using 'quad3d')
    @param canvas destination canvas
    @param x1 left coordinate
    @param y1 top coordinate
    @param x2 right coordinate
    @param y2 bottom coordinate
    @param color1 top color
    @param color2 bottom color
 */
void awe_fill_gradient_ver(AL_CONST AWE_CANVAS *canvas, int x1, int y1, int x2, int y2, int color1, int color2);


/** vertical gradient fill (using 'quad3d') using position and size
    @param canvas destination canvas
    @param x left coordinate
    @param y top coordinate
    @param w width
    @param h height
    @param color1 top color
    @param color2 bottom color
 */
void awe_fill_gradient_ver_s(AL_CONST AWE_CANVAS *canvas, int x, int y, int w, int h, int color1, int color2);


/** draws a 3d rectangle
    @param canvas destination canvas
    @param x1 left coordinate
    @param y1 top coordinate
    @param x2 right coordinate
    @param y2 bottom coordinate
    @param top_left_color top left color
    @param bottom_right_color bottom right color
    @param width thickness of rectangle in pixels
 */
void awe_draw_3d_rect(AL_CONST AWE_CANVAS *canvas, int x1, int y1, int x2, int y2, int top_left_color, int bottom_right_color, int width);


/** draws a 3d rectangle using position and size
    @param canvas destination canvas
    @param x left coordinate
    @param y top coordinate
    @param w width
    @param h height
    @param top_left_color top left color
    @param bottom_right_color bottom right color
    @param width thickness of rectangle in pixels
 */
void awe_draw_3d_rect_s(AL_CONST AWE_CANVAS *canvas, int x, int y, int w, int h, int top_left_color, int bottom_right_color, int width);


/** draws a 3d circle
    @param canvas destination canvas
    @param x horizontal center
    @param y vertical center
    @param radius radius of circle
    @param top_left_color top left color
    @param bottom_right_color bottom right color
    @param width thickness of circle in pixels
 */
void awe_draw_3d_circle(AL_CONST AWE_CANVAS *canvas, int x, int y, int radius, int top_left_color, int bottom_right_color, int width);


/** draws a 3d 2-pixel frame
    @param canvas destination canvas
    @param x1 left coordinate
    @param y1 top coordinate
    @param x2 right coordinate
    @param y2 bottom coordinate
    @param color1 outter top-left color; also color for using when the frame is flat
    @param color2 inner top-left color
    @param color3 inner bottom-right color
    @param color4 outter bottom-right color
    @param type frame type
 */
void awe_draw_3d_frame(AL_CONST AWE_CANVAS *canvas, int x1, int y1, int x2, int y2, int color1, int color2, int color3, int color4, AWE_FRAME_TYPE type);


/** draws a 3d 2-pixel frame using position and size
    @param canvas destination canvas
    @param x left coordinate
    @param y top coordinate
    @param w width
    @param h height
    @param color1 outter top-left color; also color for using when the frame is flat
    @param color2 inner top-left color
    @param color3 inner bottom-right color
    @param color4 outter bottom-right color
    @param type frame type
 */
void awe_draw_3d_frame_s(AL_CONST AWE_CANVAS *canvas, int x, int y, int w, int h, int color1, int color2, int color3, int color4, AWE_FRAME_TYPE type);


/** draws a rectangle using a line pattern; used for drawing input focus etc
    @param canvas destination canvas
    @param x1 left coordinate
    @param y1 top coordinate
    @param x2 right coordinate
    @param y2 bottom coordinate
    @param color pixel value
    @param pt bitmask of the line: a pixel is drawn for each bit that is 1
 */
void awe_draw_rect_pattern(AL_CONST AWE_CANVAS *canvas, int x1, int y1, int x2, int y2, int color, unsigned pt);


/** draws a rectangle using a line pattern and position, size 
    @param canvas destination canvas
    @param x left coordinate
    @param y top coordinate
    @param w width
    @param h height
    @param color pixel value
    @param pt bitmask of the line: a pixel is drawn for each bit that is 1
 */
void awe_draw_rect_pattern_s(AL_CONST AWE_CANVAS *canvas, int x, int y, int w, int h, int color, unsigned pt);


/** draws an Allegro string of a specific length
    @param canvas destination canvas
    @param font font to use for the text output
    @param text NULL-terminated Allegro string to draw
    @param len number of characters to draw; pass INT_MAX for the whole string
    @param x left coordinate
    @param y top coordinate
    @param fg_color foreground color
    @param bg_color background color; use a value less than 0 for non-opaque drawing
    @param text_r optional result pointer to the string where the drawing stopped
    @return right text coordinate
 */
int awe_draw_text_len(AL_CONST AWE_CANVAS *canvas, AL_CONST FONT *font, AL_CONST char *text, int len, int x, int y, int fg_color, int bg_color, AL_CONST char **text_r);


/** draws an Allegro string until a specific coordinate is met
    @param canvas destination canvas
    @param font font to use for the text output
    @param text NULL-terminated Allegro string to draw
    @param x2 right coordinate that is the limit of text
    @param x left coordinate
    @param y top coordinate
    @param fg_color foreground color
    @param bg_color background color; use a value less than 0 for non-opaque drawing
    @param text_r optional result pointer to the string where the drawing stopped
    @return right text coordinate
 */
int awe_draw_text_pix(AL_CONST AWE_CANVAS *canvas, AL_CONST FONT *font, AL_CONST char *text, int x2, int x, int y, int fg_color, int bg_color, AL_CONST char **text_r);


/** draws an Allegro string
    @param canvas destination canvas
    @param font font to use for the text output
    @param text NULL-terminated Allegro string to draw
    @param x left coordinate
    @param y top coordinate
    @param fg_color foreground color
    @param bg_color background color; use a value less than 0 for non-opaque drawing
    @param text_r optional result pointer to the string where the drawing stopped
    @return right text coordinate
 */
int awe_draw_text(AL_CONST AWE_CANVAS *canvas, AL_CONST FONT *font, AL_CONST char *text, int x, int y, int fg_color, int bg_color);


/** draws an Allegro string of a specific length, underlying the first character
    after the first single '&'
    @param canvas destination canvas
    @param font font to use for the text output
    @param text NULL-terminated Allegro string to draw
    @param len number of characters to draw; pass INT_MAX for the whole string
    @param x left coordinate
    @param y top coordinate
    @param fg_color foreground color
    @param bg_color background color; use a value less than 0 for non-opaque drawing
    @param text_r optional result pointer to the string where the drawing stopped
    @return right text coordinate
 */
int awe_draw_gui_text_len(AL_CONST AWE_CANVAS *canvas, AL_CONST FONT *font, AL_CONST char *text, int len, int x, int y, int fg_color, int bg_color, AL_CONST char **text_r);


/** draws an Allegro string until a specific coordinate is met, underlying the
    first character after the first single '&'
    @param canvas destination canvas
    @param font font to use for the text output
    @param text NULL-terminated Allegro string to draw
    @param x2 right coordinate that is the limit of text
    @param x left coordinate
    @param y top coordinate
    @param fg_color foreground color
    @param bg_color background color; use a value less than 0 for non-opaque drawing
    @param text_r optional result pointer to the string where the drawing stopped
    @return right text coordinate
 */
int awe_draw_gui_text_pix(AL_CONST AWE_CANVAS *canvas, AL_CONST FONT *font, AL_CONST char *text, int x2, int x, int y, int fg_color, int bg_color, AL_CONST char **text_r);


/** draws an Allegro string, underlying the first character after the first
    single '&'
    @param canvas destination canvas
    @param font font to use for the text output
    @param text NULL-terminated Allegro string to draw
    @param x left coordinate
    @param y top coordinate
    @param fg_color foreground color
    @param bg_color background color; use a value less than 0 for non-opaque drawing
    @param text_r optional result pointer to the string where the drawing stopped
    @return right text coordinate
 */
int awe_draw_gui_text(AL_CONST AWE_CANVAS *canvas, AL_CONST FONT *font, AL_CONST char *text, int x, int y, int fg_color, int bg_color);


/** blits a bitmap onto a canvas
    @param src source bitmap
    @param canvas destination canvas
    @param src_x left coordinate inside the source bitmap
    @param src_y top coordinate inside the source bitmap
    @param dst_x left coordinate inside the destination canvas
    @param dst_y top coordinate inside the destination canvas
    @param width width of area to blit
    @param height height of area to blit
 */
void awe_blit_bitmap(BITMAP *src, AL_CONST AWE_CANVAS *canvas, int src_x, int src_y, int dst_x, int dst_y, int width, int height);


/** masked blitting of a bitmap onto a canvas
    @param src source bitmap
    @param canvas destination canvas
    @param src_x left coordinate inside the source bitmap
    @param src_y top coordinate inside the source bitmap
    @param dst_x left coordinate inside the destination canvas
    @param dst_y top coordinate inside the destination canvas
    @param width width of area to blit
    @param height height of area to blit
 */
void awe_blit_masked_bitmap(BITMAP *src, AL_CONST AWE_CANVAS *canvas, int src_x, int src_y, int dst_x, int dst_y, int width, int height);


/** scaled blitting of a bitmap onto a canvas
    @param src source bitmap
    @param canvas destination canvas
    @param src_x left coordinate inside the source bitmap
    @param src_y top coordinate inside the source bitmap
    @param src_width width of area of source bitmap to blit
    @param src_height height of area of source bitmap to blit
    @param dst_x left coordinate inside the destination canvas
    @param dst_y top coordinate inside the destination canvas
    @param dst_width width of area of destination bitmap to blit
    @param dst_height height of area of destination bitmap to blit
 */
void awe_blit_stretch_bitmap(BITMAP *src, AL_CONST AWE_CANVAS *canvas, int src_x, int src_y, int src_width, int src_height, int dst_x, int dst_y, int dst_width, int dst_height);


/** scaled masked blitting of a bitmap onto a canvas
    @param src source bitmap
    @param canvas destination canvas
    @param src_x left coordinate inside the source bitmap
    @param src_y top coordinate inside the source bitmap
    @param src_width width of area of source bitmap to blit
    @param src_height height of area of source bitmap to blit
    @param dst_x left coordinate inside the destination canvas
    @param dst_y top coordinate inside the destination canvas
    @param dst_width width of area of destination bitmap to blit
    @param dst_height height of area of destination bitmap to blit
 */
void awe_blit_masked_stretch_bitmap(BITMAP *src, AL_CONST AWE_CANVAS *canvas, int src_x, int src_y, int src_width, int src_height, int dst_x, int dst_y, int dst_width, int dst_height);


/** draws a bitmap's non-masked pixels with a specific color
    @param src source bitmap
    @param canvas destination canvas
    @param src_x left coordinate inside the source bitmap
    @param src_y top coordinate inside the source bitmap
    @param dst_x left coordinate inside the destination canvas
    @param dst_y top coordinate inside the destination canvas
    @param width width of area to blit
    @param height height of area to blit
    @param color pixel value
 */
void awe_blit_mono_bitmap(BITMAP *src, AL_CONST AWE_CANVAS *canvas, int src_x, int src_y, int dst_x, int dst_y, int width, int height, int color);


/** draws a bitmap's non-masked pixels with a specific color; bottom-right
    edges are drawn with another color
    @param src source bitmap
    @param canvas destination canvas
    @param src_x left coordinate inside the source bitmap
    @param src_y top coordinate inside the source bitmap
    @param dst_x left coordinate inside the destination canvas
    @param dst_y top coordinate inside the destination canvas
    @param width width of area to blit
    @param height height of area to blit
    @param top_left_color top left color
    @param bottom_right_color bottom right color
 */
void awe_blit_3d_bitmap(BITMAP *src, AL_CONST AWE_CANVAS *canvas, int src_x, int src_y, int dst_x, int dst_y, int width, int height, int top_left_color, int bottom_right_color);


/** draws a left arrow
    @param canvas destination canvas
    @param x horizontal coordinate
    @param y vertical coordinate
    @param radius radius of shape
    @param color pixel value
 */
void awe_draw_left_arrow_icon(AL_CONST AWE_CANVAS *canvas, int x, int y, int radius, int color);


/** draws a right arrow
    @param canvas destination canvas
    @param x horizontal coordinate
    @param y vertical coordinate
    @param radius radius of shape
    @param color pixel value
 */
void awe_draw_right_arrow_icon(AL_CONST AWE_CANVAS *canvas, int x, int y, int radius, int color);


/** draws an up arrow
    @param canvas destination canvas
    @param x horizontal coordinate
    @param y vertical coordinate
    @param radius radius of shape
    @param color pixel value
 */
void awe_draw_up_arrow_icon(AL_CONST AWE_CANVAS *canvas, int x, int y, int radius, int color);


/** draws a down arrow
    @param canvas destination canvas
    @param x horizontal coordinate
    @param y vertical coordinate
    @param radius radius of shape
    @param color pixel value
 */
void awe_draw_down_arrow_icon(AL_CONST AWE_CANVAS *canvas, int x, int y, int radius, int color);


/** draws a tick (a check mark)
    @param canvas destination canvas
    @param x horizontal coordinate
    @param y vertical coordinate
    @param radius radius of shape
    @param color pixel value
    @param width thickness of icon in pixels
 */
void awe_draw_tick_icon(AL_CONST AWE_CANVAS *canvas, int x, int y, int radius, int color, int width);


/** draws a close icon (an 'x')
    @param canvas destination canvas
    @param x horizontal coordinate
    @param y vertical coordinate
    @param radius radius of shape
    @param color pixel value
    @param width thickness of icon in pixels
 */
void awe_draw_close_icon(AL_CONST AWE_CANVAS *canvas, int x, int y, int radius, int color, int width);


/** draws a minimize icon
    @param canvas destination canvas
    @param x horizontal coordinate
    @param y vertical coordinate
    @param radius radius of shape
    @param color pixel value
    @param width thickness of icon in pixels
 */
void awe_draw_minimize_icon(AL_CONST AWE_CANVAS *canvas, int x, int y, int radius, int color, int width);


/** draws a maximize icon
    @param canvas destination canvas
    @param x horizontal coordinate
    @param y vertical coordinate
    @param radius radius of shape
    @param color pixel value
    @param width thickness of icon in pixels
 */
void awe_draw_maximize_icon(AL_CONST AWE_CANVAS *canvas, int x, int y, int radius, int color, int width);


/** draws a restore icon
    @param canvas destination canvas
    @param x horizontal coordinate
    @param y vertical coordinate
    @param radius radius of shape
    @param color pixel value
    @param width thickness of icon in pixels
 */
void awe_draw_restore_icon(AL_CONST AWE_CANVAS *canvas, int x, int y, int radius, int color, int width);


/** creates a texture used for drawing skinned surfaces
    @param src source bitmap; must be around while the texture is being used
    @param x left coordinate inside the source bitmap
    @param y top coordinate inside the source bitmap
    @param w width of texture area inside the source bitmap; has same constraints
             as allegro's patterned drawing
    @param h height of texture area inside the source bitmap
    @param left_side size of left border
    @param top_side size of top border
    @param right_side size of right border
    @param bottom_side size of bottom border
 */
AWE_TEXTURE *awe_create_texture(BITMAP *src, int x, int y, int w, int h, int left_side, int top_side, int right_side, int bottom_side);


/** destroys a texture
    @param texture texture to destroy
 */
void awe_destroy_texture(AWE_TEXTURE *texture);


/** draws a texture on a canvas
    @param canvas destination canvas
    @param tex texture to draw
    @param x1 left coordinate
    @param y1 top coordinate
    @param x2 right coordinate
    @param y2 bottom coordinate
 */
void awe_draw_texture(AL_CONST AWE_CANVAS *canvas, AL_CONST AWE_TEXTURE *tex, int x1, int y1, int x2, int y2);


/** draws a texture on a canvas using position and size
    @param canvas destination canvas
    @param tex texture to draw
    @param x left coordinate
    @param y top coordinate
    @param w width
    @param h height
 */
void awe_draw_texture_s(AL_CONST AWE_CANVAS *canvas, AL_CONST AWE_TEXTURE *tex, int x, int y, int w, int h);


/** draws a horizontal strip filled with a texture
    @param canvas destination canvas
    @param tex texture to draw
    @param x1 left coordinate
    @param y1 top coordinate
    @param x2 right coordinate
    @param y2 bottom coordinate
 */
void awe_draw_texture_hor(AL_CONST AWE_CANVAS *canvas, AL_CONST AWE_TEXTURE *tex, int x1, int y1, int x2, int y2);


/** draws a horizontal strip filled with a texture using position and size
    @param canvas destination canvas
    @param tex texture to draw
    @param x left coordinate
    @param y top coordinate
    @param w width
    @param h height
 */
void awe_draw_texture_hor_s(AL_CONST AWE_CANVAS *canvas, AL_CONST AWE_TEXTURE *tex, int x, int y, int w, int h);


/** draws a vertical strip filled with a texture
    @param canvas destination canvas
    @param tex texture to draw
    @param x1 left coordinate
    @param y1 top coordinate
    @param x2 right coordinate
    @param y2 bottom coordinate
 */
void awe_draw_texture_ver(AL_CONST AWE_CANVAS *canvas, AL_CONST AWE_TEXTURE *tex, int x1, int y1, int x2, int y2);


/** draws a vertical strip filled with a texture using position and size
    @param canvas destination canvas
    @param tex texture to draw
    @param x left coordinate
    @param y top coordinate
    @param w width
    @param h height
 */
void awe_draw_texture_ver_s(AL_CONST AWE_CANVAS *canvas, AL_CONST AWE_TEXTURE *tex, int x, int y, int w, int h);


/** draws a shadow 
    @param canvas destination canvas
    @param x1 left coordinate
    @param y1 top coordinate
    @param x2 right coordinate
    @param y2 bottom coordinate
    @param start_trans start translucency (0 to 255)
    @param end_trans end translucency (0 to 255)
    @param hor if true, the direction is horizontal, else vertical
 */
void awe_draw_shadow(AL_CONST AWE_CANVAS *canvas, int x1, int y1, int x2, int y2, int start_trans, int end_trans, int hor);


/** draws a shadow using position and size
    @param canvas destination canvas
    @param x1 left coordinate
    @param y1 top coordinate
    @param x2 right coordinate
    @param y2 bottom coordinate
    @param start_trans start translucency (0 to 255)
    @param end_trans end translucency (0 to 255)
    @param hor if true, the direction is horizontal, else vertical
 */
void awe_draw_shadow_s(AL_CONST AWE_CANVAS *canvas, int x, int y, int w, int h, int start_trans, int end_trans, int hor);


/** draws a shadow inside the bottom-right edge of given rectangle
    @param canvas destination canvas
    @param x1 left coordinate
    @param y1 top coordinate
    @param x2 right coordinate
    @param y2 bottom coordinate
    @param start_trans start translucency (0 to 255)
    @param end_trans end translucency (0 to 255)
    @param width width of shadow
 */
void awe_draw_bottom_right_shadow(AL_CONST AWE_CANVAS *canvas, int x1, int y1, int x2, int y2, int start_trans, int end_trans, int width);


/** draws a shadow inside the bottom-right edge of given rectangle,
    using position and size
    @param canvas destination canvas
    @param x left coordinate
    @param y top coordinate
    @param w width
    @param h height
    @param start_trans start translucency (0 to 255)
    @param end_trans end translucency (0 to 255)
    @param width width of shadow
 */
void awe_draw_bottom_right_shadow_s(AL_CONST AWE_CANVAS *canvas, int x, int y, int w, int h, int start_trans, int end_trans, int width);


/*@}*/


#ifdef __cplusplus
   }
#endif


#endif //AWE_GDI_H
