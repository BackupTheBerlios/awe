#ifndef AWE_MOUSE_H
#define AWE_MOUSE_H

#include <allegro.h> 
#include <allegro/internal/aintern.h>

#ifdef LOADPNG
    #include <png.h>
    #include <loadpng.h>
#endif

#include "linkedlist.h"

#ifdef __cplusplus
extern "C" {
#endif


/**@name Mouse
    <p>The purpose of the Mouse module is to provide an easy way to create custom cursors and offer
    extened cursor set based on a theme. It offers the following additional functionality
    to Allegro:</p>
    <LL>
    <LI>Custom Cursors; Cursors can be more than just a static bitmap, the library offers the ease of
        animated cursors, dynamic shadows, transparency, and alpha transparency support.
    </LI>
    <LI>Themed cursors; A default system cursor set is loaded initially but additional themes can be installed.
    </LI>
    </LL>
    <p>The Mouse module is an independent library.</p>
 */
/*@{*/


/** cursor types
 */
enum AWE_CURSOR_TYPE {
    ///user cursor
    AWE_MOUSE_USER = -1,

    ///arrow cursor
    AWE_MOUSE_ARROW,

    ///cross cursor
    AWE_MOUSE_CROSS,

    ///hand cursor
    AWE_MOUSE_HAND,

    ///no cursor
    AWE_MOUSE_NO,

    ///wait cursor
    AWE_MOUSE_WAIT,

    ///handwriting cursor
    AWE_MOUSE_HANDWRITING,

    ///help cursor
    AWE_MOUSE_HELP,

    ///sizeall cursor
    AWE_MOUSE_SIZEALL,

    ///NWSE cursor
    AWE_MOUSE_SIZENWSE,

    ///NESW cursor
    AWE_MOUSE_SIZENESW,

    ///WE cursor
    AWE_MOUSE_SIZEWE,

    ///NS cursor
    AWE_MOUSE_SIZENS,

    ///ibeam cursor
    AWE_MOUSE_IBEAM,

    ///uparrow cursor
    AWE_MOUSE_UPARROW,

    ///button cursor
    AWE_MOUSE_BUTTON,

    ///application cursor
    AWE_MOUSE_APPSTARTING
};
typedef enum AWE_CURSOR_TYPE AWE_CURSOR_TYPE;


/** animation types
 */
typedef enum AWE_CURSOR_ANIM {
    ///loop and repeat from first frame
    AWE_MOUSE_LOOP_REPEAT,

    ///loop forwards than backwards and repeat
    AWE_MOUSE_LOOP_BACK,

    ///loop once forwards
    AWE_MOUSE_LOOP_ONCE
} AWE_CURSOR_ANIM;


/** shadow flags
 */
enum AWE_CURSOR_SHADOW {
    ///shadow will be hidden
    AWE_MOUSE_HIDE_SHADOW,

    ///shadow will be expressed
    AWE_MOUSE_SHOW_SHADOW
};
typedef enum AWE_CURSOR_SHADOW AWE_CURSOR_SHADOW;


/** shadow methods
 */
enum AWE_SHADOW_METHOD {
    ///shadow generated only once for every frame of animation
    AWE_MOUSE_SINGLE_SHADOW,

    ///shadow generated for each frame of animation
    AWE_MOUSE_MULTI_SHADOW
};
typedef enum AWE_SHADOW_METHOD AWE_SHADOW_METHOD;


/** cursor transparency methods
 */
enum AWE_TRANS_LEVEL {
    ///set if cursor contains an alpha channel for transparency
    AWE_MOUSE_TRANS_ALPHA = -1,

    ///default transparency level (full opacity)
    AWE_MOUSE_DEFAULT_TRANS
};
typedef enum AWE_TRANS_LEVEL AWE_TRANS_LEVEL;

/**cursor struct
 */
typedef struct AWE_CURSOR AWE_CURSOR;


/** initializes mouse system
    @return returns 1 on success, otherwise 0
 */
int awe_install_mouse(void);


/** closes down mouse system
 */
void awe_mouse_exit(void);

/** creates a custom cursor
    @param cursor bitmap to the cursor sprite
    @param hotx hotspot on x axis
    @param hoty hotspot on y axis
    @param shadowed set if cursor will display a shadow
    @param shadow_method method of generating shadows based on frames
    @param num_frames number of frames of animation
    @param speed speed delay between frames of animation - higher is slower
    @param anim_type type of animation the cursor will express
    @param cur_trans cursor transparency level - higher the number the more transparent the cursor will be
    @return the newly created cursor or NULL on error
 */
AWE_CURSOR *awe_create_mouse_cursor(BITMAP *cursor, int hotx, int hoty, AWE_CURSOR_SHADOW shadowed, AWE_SHADOW_METHOD shadow_method, int num_frames, int speed, AWE_CURSOR_ANIM anim_type, AWE_TRANS_LEVEL cur_trans);


/** destroys a custom cursor
    @param cursor pointer to the cursor to be destroyed, will not destroy system cursors
 */
void awe_destroy_mouse_cursor(AWE_CURSOR *cursor);


/** loads a cursor set from a path
    @param path path to the datafile containing the cursor set
 */
int awe_load_mouse(const char *path);


/** loads a cursor set from a datafile
    @param dat pointer to the datafile containing the cursor set
 */
int awe_load_datafile_mouse(DATAFILE *dat);


/** forces the redraw of the mouse
 */
void awe_redraw_mouse();


/** updates the mouse
 */
void awe_update_mouse(void);


/** scares the mouse pointer
 */
void awe_scare_mouse(void);


/** scares the mouse pointer only if it is within the area specified
 */
void awe_scare_mouse_area(int x, int y, int w, int h);


/** returns the area of the mouse
 */
void awe_get_mouse_area(int *x, int *y, int *w, int *h);


/** unscares the mouse cursor
 */
void awe_unscare_mouse(void);


/** returns a system cursor
    @param type type of cursor to be returned
    @return returns a pointer to a system cursor or NULL on error
 */
AWE_CURSOR *awe_get_system_cursor(AWE_CURSOR_TYPE type);


/** sets the mouse cursor
    @param cursor pointer to a valid cursor
    @return returns the pointer of the last cursor set
 */
AWE_CURSOR *awe_set_mouse_cursor(AWE_CURSOR *cursor);


/** shows the mouse to the specified bitmap
 */
void awe_show_mouse(BITMAP *bmp);


/** enables or disables the shadow
 */
void awe_show_mouse_shadow(AWE_CURSOR_SHADOW shadow);


/** sets the shadow offset
 */
void awe_set_mouse_shadow_offset(int offsetx, int offsety);


/** sets the shadow intensity
 */
void awe_set_mouse_shadow_intensity(int intensity);


/** sets the shadow focus
 */
void awe_set_shadow_focus(int focus);


/** returns the current mouse sprite
 */
AWE_CURSOR *awe_get_mouse_cursor(void);


/** repositions the mouse cursor
 */
void awe_position_mouse(int x, int y);


/** repositions the mouse wheel
 */
void awe_position_mouse_z(int z);


/** sets the mouse range
 */
void awe_set_mouse_range(int x1, int y1, int x2, int y2);


/** sets the mouse speed
 */
void awe_set_mouse_speed(int xspeed, int yspeed);


/** returns the mouse coordinates
 */
void awe_get_mouse_mickeys(int *mickeyx, int *mickeyy);


/** returns the mouse screen
 */
BITMAP *awe_get_mouse_screen();


/** sets the mouse screen
 */
void awe_set_mouse_screen(BITMAP *bmp);


/*@}*/


#ifdef __cplusplus
}
#endif

#endif //AWE_MOUSE_H
