#include "mouse.h"


/*****************************************************************************
    PRIVATE
 *****************************************************************************/


// Total cursors * 2 for pressed and non-pressed set
#define MOUSE_NUM_CURSORS            32


#define MOUSE_OFFSCREEN              -4096
#define MOUSE_DEFAULT_FOCUSX         1
#define MOUSE_DEFAULT_FOCUSY         1
#define MOUSE_DEFAULT_OFFSETX        2
#define MOUSE_DEFAULT_OFFSETY        1
#define MOUSE_DEFAULT_FOCUS          1
#define MOUSE_SCARED_SIZE            16          /* for unscare_mouse() */
#define MOUSE_DEFAULT_INTENSITY      140         /* default shadow intensity */


//cursor structure
struct AWE_CURSOR {
    AWE_CURSOR_TYPE type;
    BITMAP **cur;
    BITMAP **shadow;
    int cur_trans;
    int num_frames;
    int cur_frame;
    int anim_type;
    int speed;
    unsigned int timer;
    int hotx;
    int hoty;
}; 


typedef struct _default_cursor {
    unsigned char *cursor;
    int hotx;
    int hoty;
    int shadow;    
} _DEFAULT_CURSOR;


static volatile unsigned int _mouse_timer;


static void _timer(void){ 
    _mouse_timer++; 
}END_OF_FUNCTION(_timer);


static AWE_CURSOR *_cursor[MOUSE_NUM_CURSORS];


static AWE_DL_LIST _user_list = { NULL, NULL };


static DATAFILE *_mouse = NULL;


static int _mx = MOUSE_OFFSCREEN;
static int _my = MOUSE_OFFSCREEN;
static int _old_mx = MOUSE_OFFSCREEN;
static int _old_my = MOUSE_OFFSCREEN;


static BITMAP *_mouse_bg = NULL;
static BITMAP *_shadow_bg = NULL;
static BITMAP *_mouse_vid_screen = NULL;


static int _shadow_offsetx = MOUSE_DEFAULT_OFFSETX;
static int _shadow_offsety = MOUSE_DEFAULT_OFFSETY;
static int _shadow_intensity = MOUSE_DEFAULT_INTENSITY;
static int _shadow_focus = MOUSE_DEFAULT_FOCUS;
static int _show_shadow = TRUE;


static AWE_CURSOR *_current_cur = NULL;


static int _animation_dir = 1;


static BITMAP *_scared_screen[MOUSE_SCARED_SIZE]; 
static int _scared_freeze[MOUSE_SCARED_SIZE]; 
static int _scared_size = 0;


// Current cursor path
static char *_cursor_path = NULL;


// Mouse install count
static int _mouse_install_count = 0;


// Cursor names for datafile
static char *_cur_names[MOUSE_NUM_CURSORS] = {
    "Arrow",
    "Cross",
    "Hand",
    "No",
    "Wait",
    "Handwriting",
    "Help",
    "Sizeall",
    "Sizenwse",
    "Sizenesw",
    "Sizewe",
    "Sizens",
    "Ibeam",
    "Uparrow",
    "Button",
    "Appstarting",
    "Arrow_down",
    "Cross_down",
    "Hand_down",
    "No_down",
    "Wait_down",
    "Handwriting_down",
    "Help_down",
    "Sizeall_down",
    "Sizenwse_down",
    "Sizenesw_down",
    "Sizewe_down",
    "Sizens_down",
    "Ibeam_down",
    "Uparrow_down",
    "Button_down",
    "Appstarting_down"
};


// Known extensions for loading
static char *_mouse_ext[] = {
    "bmp",
    "pcx",
    "tga",
    #ifdef LOADPNG
    "png",
    #endif
    NULL
};


//default arrow cursor
static unsigned char _default_cursor_arrow[] = {
     11, 19,
     1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
     1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
     1, 2, 1, 0, 0, 0, 0, 0, 0, 0, 0, 
     1, 2, 2, 1, 0, 0, 0, 0, 0, 0, 0, 
     1, 2, 2, 2, 1, 0, 0, 0, 0, 0, 0, 
     1, 2, 2, 2, 2, 1, 0, 0, 0, 0, 0, 
     1, 2, 2, 2, 2, 2, 1, 0, 0, 0, 0, 
     1, 2, 2, 2, 2, 2, 2, 1, 0, 0, 0, 
     1, 2, 2, 2, 2, 2, 2, 2, 1, 0, 0, 
     1, 2, 2, 2, 2, 2, 2, 2, 2, 1, 0, 
     1, 2, 2, 2, 2, 2, 1, 1, 1, 1, 1, 
     1, 2, 2, 1, 2, 2, 1, 0, 0, 0, 0, 
     1, 2, 1, 0, 1, 2, 2, 1, 0, 0, 0, 
     1, 1, 0, 0, 1, 2, 2, 1, 0, 0, 0, 
     1, 0, 0, 0, 0, 1, 2, 2, 1, 0, 0, 
     0, 0, 0, 0, 0, 1, 2, 2, 1, 0, 0, 
     0, 0, 0, 0, 0, 0, 1, 2, 2, 1, 0, 
     0, 0, 0, 0, 0, 0, 1, 2, 2, 1, 0, 
     0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0 
};


//default cross cursor
static unsigned char _default_cursor_cross[] = {
     17, 17,
     0, 0, 0, 0, 0, 0, 0, 2, 1, 2, 0, 0, 0, 0, 0, 0, 0, 
     0, 0, 0, 0, 0, 0, 0, 2, 1, 2, 0, 0, 0, 0, 0, 0, 0, 
     0, 0, 0, 0, 0, 0, 0, 2, 1, 2, 0, 0, 0, 0, 0, 0, 0, 
     0, 0, 0, 0, 0, 0, 0, 2, 1, 2, 0, 0, 0, 0, 0, 0, 0, 
     0, 0, 0, 0, 0, 0, 0, 2, 1, 2, 0, 0, 0, 0, 0, 0, 0, 
     0, 0, 0, 0, 0, 0, 0, 2, 1, 2, 0, 0, 0, 0, 0, 0, 0, 
     0, 0, 0, 0, 0, 0, 0, 2, 1, 2, 0, 0, 0, 0, 0, 0, 0, 
     2, 2, 2, 2, 2, 2, 2, 2, 1, 2, 2, 2, 2, 2, 2, 2, 2, 
     1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 
     2, 2, 2, 2, 2, 2, 2, 2, 1, 2, 2, 2, 2, 2, 2, 2, 2, 
     0, 0, 0, 0, 0, 0, 0, 2, 1, 2, 0, 0, 0, 0, 0, 0, 0, 
     0, 0, 0, 0, 0, 0, 0, 2, 1, 2, 0, 0, 0, 0, 0, 0, 0, 
     0, 0, 0, 0, 0, 0, 0, 2, 1, 2, 0, 0, 0, 0, 0, 0, 0, 
     0, 0, 0, 0, 0, 0, 0, 2, 1, 2, 0, 0, 0, 0, 0, 0, 0, 
     0, 0, 0, 0, 0, 0, 0, 2, 1, 2, 0, 0, 0, 0, 0, 0, 0, 
     0, 0, 0, 0, 0, 0, 0, 2, 1, 2, 0, 0, 0, 0, 0, 0, 0, 
     0, 0, 0, 0, 0, 0, 0, 2, 1, 2, 0, 0, 0, 0, 0, 0, 0 
};


//default hand cursor
static unsigned char _default_cursor_hand[] = {
     17, 22,
     0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
     0, 0, 0, 0, 1, 2, 2, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
     0, 0, 0, 0, 1, 2, 2, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
     0, 0, 0, 0, 1, 2, 2, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
     0, 0, 0, 0, 1, 2, 2, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
     0, 0, 0, 0, 1, 2, 2, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 
     0, 0, 0, 0, 1, 2, 2, 1, 2, 2, 1, 1, 1, 0, 0, 0, 0, 
     0, 0, 0, 0, 1, 2, 2, 1, 2, 2, 1, 2, 2, 1, 1, 0, 0, 
     0, 0, 0, 0, 1, 2, 2, 1, 2, 2, 1, 2, 2, 1, 2, 1, 0, 
     1, 1, 1, 0, 1, 2, 2, 1, 2, 2, 1, 2, 2, 1, 2, 2, 1, 
     1, 2, 2, 1, 1, 2, 2, 2, 2, 2, 2, 2, 2, 1, 2, 2, 1, 
     1, 2, 2, 2, 1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 1, 
     0, 1, 2, 2, 1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 1, 
     0, 0, 1, 2, 1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 1, 
     0, 0, 1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 1, 
     0, 0, 0, 1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 1, 
     0, 0, 0, 1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 1, 0, 
     0, 0, 0, 0, 1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 1, 0, 
     0, 0, 0, 0, 1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 1, 0, 
     0, 0, 0, 0, 0, 1, 2, 2, 2, 2, 2, 2, 2, 2, 1, 0, 0, 
     0, 0, 0, 0, 0, 1, 2, 2, 2, 2, 2, 2, 2, 2, 1, 0, 0, 
     0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0 
};


//default handwriting cursor
static unsigned char _default_cursor_handwriting[] = {
     24, 24,
     2, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
     2, 1, 2, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
     0, 2, 1, 1, 2, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
     0, 2, 1, 1, 1, 1, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
     0, 0, 2, 1, 1, 2, 1, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
     0, 0, 2, 1, 1, 1, 2, 1, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
     0, 0, 0, 2, 1, 1, 2, 2, 1, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
     0, 0, 0, 0, 2, 1, 1, 2, 2, 1, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
     0, 0, 0, 0, 0, 2, 1, 1, 2, 2, 1, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
     0, 0, 0, 0, 0, 0, 2, 1, 1, 2, 2, 1, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
     0, 0, 0, 0, 0, 0, 0, 2, 1, 1, 2, 2, 1, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
     0, 0, 0, 0, 0, 0, 0, 0, 2, 1, 1, 2, 2, 1, 2, 0, 0, 2, 0, 0, 0, 0, 0, 0, 
     0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 1, 1, 2, 2, 1, 2, 2, 1, 2, 0, 0, 0, 0, 0, 
     0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 1, 2, 1, 2, 1, 2, 1, 1, 2, 0, 0, 0, 0, 
     0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 1, 1, 1, 2, 1, 1, 1, 1, 2, 0, 0, 0, 
     0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 1, 1, 1, 2, 1, 2, 1, 1, 2, 0, 0, 
     0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 1, 1, 1, 2, 1, 2, 1, 1, 2, 0, 
     0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 1, 1, 1, 2, 1, 2, 1, 1, 2, 
     0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 1, 1, 1, 2, 1, 2, 1, 2, 
     0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 1, 1, 1, 2, 1, 1, 2, 
     0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 1, 1, 1, 2, 1, 2, 
     0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 1, 1, 1, 1, 2, 
     0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 1, 1, 2, 0, 
     0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 2, 0, 0 
};


//default help cursor
static unsigned char _default_cursor_help[] = {
     22, 19,
     1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
     1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 2, 2, 2, 2, 2, 2, 0, 0, 0, 
     1, 2, 1, 0, 0, 0, 0, 0, 0, 0, 0, 2, 1, 1, 1, 1, 1, 1, 1, 2, 0, 0, 
     1, 2, 2, 1, 0, 0, 0, 0, 0, 0, 2, 1, 1, 1, 2, 2, 1, 1, 1, 1, 2, 0, 
     1, 2, 2, 2, 1, 0, 0, 0, 0, 2, 1, 1, 1, 2, 0, 0, 2, 1, 1, 1, 1, 2, 
     1, 2, 2, 2, 2, 1, 0, 0, 0, 2, 1, 1, 1, 2, 0, 0, 2, 1, 1, 1, 1, 2, 
     1, 2, 2, 2, 2, 2, 1, 0, 0, 2, 1, 1, 1, 2, 0, 0, 2, 1, 1, 1, 1, 2, 
     1, 2, 2, 2, 2, 2, 2, 1, 0, 2, 1, 1, 1, 2, 0, 0, 2, 1, 1, 1, 2, 0, 
     1, 2, 2, 2, 2, 2, 2, 2, 1, 0, 2, 2, 2, 0, 0, 2, 1, 1, 1, 2, 0, 0, 
     1, 2, 2, 2, 2, 2, 2, 2, 2, 1, 0, 0, 0, 0, 2, 1, 1, 1, 2, 0, 0, 0, 
     1, 2, 2, 2, 2, 2, 1, 1, 1, 1, 1, 0, 0, 2, 1, 1, 1, 2, 0, 0, 0, 0, 
     1, 2, 2, 1, 2, 2, 1, 0, 0, 0, 0, 0, 0, 2, 1, 1, 1, 2, 0, 0, 0, 0, 
     1, 2, 1, 0, 1, 2, 2, 1, 0, 0, 0, 0, 0, 2, 1, 1, 1, 2, 0, 0, 0, 0, 
     1, 1, 0, 0, 1, 2, 2, 1, 0, 0, 0, 0, 0, 0, 2, 2, 2, 0, 0, 0, 0, 0, 
     1, 0, 0, 0, 0, 1, 2, 2, 1, 0, 0, 0, 0, 2, 1, 1, 1, 2, 0, 0, 0, 0, 
     0, 0, 0, 0, 0, 1, 2, 2, 1, 0, 0, 0, 2, 1, 1, 1, 1, 1, 2, 0, 0, 0, 
     0, 0, 0, 0, 0, 0, 1, 2, 2, 1, 0, 0, 0, 2, 1, 1, 1, 2, 0, 0, 0, 0, 
     0, 0, 0, 0, 0, 0, 1, 2, 2, 1, 0, 0, 0, 0, 2, 2, 2, 0, 0, 0, 0, 0, 
     0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 
};


//default ibeam cursor
static unsigned char _default_cursor_ibeam[] = {
     7, 16,
     1, 1, 1, 2, 1, 1, 1, 
     2, 2, 2, 1, 2, 2, 2, 
     0, 0, 2, 1, 2, 0, 0, 
     0, 0, 2, 1, 2, 0, 0, 
     0, 0, 2, 1, 2, 0, 0, 
     0, 0, 2, 1, 2, 0, 0, 
     0, 0, 2, 1, 2, 0, 0, 
     0, 0, 2, 1, 2, 0, 0, 
     0, 0, 2, 1, 2, 0, 0, 
     0, 0, 2, 1, 2, 0, 0, 
     0, 0, 2, 1, 2, 0, 0, 
     0, 0, 2, 1, 2, 0, 0, 
     0, 0, 2, 1, 2, 0, 0, 
     0, 0, 2, 1, 2, 0, 0, 
     2, 2, 2, 1, 2, 2, 2, 
     1, 1, 1, 2, 1, 1, 1 
};


//default no cursor
static unsigned char _default_cursor_no[] = {
     20, 20,
     0, 0, 0, 0, 0, 0, 0, 2, 2, 2, 2, 2, 2, 0, 0, 0, 0, 0, 0, 0, 
     0, 0, 0, 0, 0, 2, 2, 1, 1, 1, 1, 1, 1, 2, 2, 0, 0, 0, 0, 0, 
     0, 0, 0, 0, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 0, 0, 0, 0, 
     0, 0, 0, 2, 1, 1, 1, 1, 2, 2, 2, 2, 1, 1, 1, 1, 2, 0, 0, 0, 
     0, 0, 2, 1, 1, 1, 2, 2, 0, 0, 0, 0, 2, 2, 1, 1, 1, 2, 0, 0, 
     0, 2, 1, 1, 1, 1, 1, 2, 0, 0, 0, 0, 0, 0, 2, 1, 1, 1, 2, 0, 
     0, 2, 1, 1, 2, 1, 1, 1, 2, 0, 0, 0, 0, 0, 0, 2, 1, 1, 2, 0, 
     2, 1, 1, 1, 2, 2, 1, 1, 1, 2, 0, 0, 0, 0, 0, 2, 1, 1, 1, 2, 
     2, 1, 1, 2, 0, 0, 2, 1, 1, 1, 2, 0, 0, 0, 0, 0, 2, 1, 1, 2, 
     2, 1, 1, 2, 0, 0, 0, 2, 1, 1, 1, 2, 0, 0, 0, 0, 2, 1, 1, 2, 
     2, 1, 1, 2, 0, 0, 0, 0, 2, 1, 1, 1, 2, 0, 0, 0, 2, 1, 1, 2, 
     2, 1, 1, 2, 0, 0, 0, 0, 0, 2, 1, 1, 1, 2, 0, 0, 2, 1, 1, 2, 
     2, 1, 1, 1, 2, 0, 0, 0, 0, 0, 2, 1, 1, 1, 2, 2, 1, 1, 1, 2, 
     0, 2, 1, 1, 2, 0, 0, 0, 0, 0, 0, 2, 1, 1, 1, 2, 1, 1, 2, 0, 
     0, 2, 1, 1, 1, 2, 0, 0, 0, 0, 0, 0, 2, 1, 1, 1, 1, 1, 2, 0, 
     0, 0, 2, 1, 1, 1, 2, 2, 0, 0, 0, 0, 2, 2, 1, 1, 1, 2, 0, 0, 
     0, 0, 0, 2, 1, 1, 1, 1, 2, 2, 2, 2, 1, 1, 1, 1, 2, 0, 0, 0, 
     0, 0, 0, 0, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 0, 0, 0, 0, 
     0, 0, 0, 0, 0, 2, 2, 1, 1, 1, 1, 1, 1, 2, 2, 0, 0, 0, 0, 0, 
     0, 0, 0, 0, 0, 0, 0, 2, 2, 2, 2, 2, 2, 0, 0, 0, 0, 0, 0, 0 
};


//default sizeall cursor
static unsigned char _default_cursor_sizeall[] = {
     21, 21,
     0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
     0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 1, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
     0, 0, 0, 0, 0, 0, 0, 0, 2, 1, 1, 1, 2, 0, 0, 0, 0, 0, 0, 0, 0, 
     0, 0, 0, 0, 0, 0, 0, 2, 1, 1, 1, 1, 1, 2, 0, 0, 0, 0, 0, 0, 0, 
     0, 0, 0, 0, 0, 0, 2, 1, 1, 1, 1, 1, 1, 1, 2, 0, 0, 0, 0, 0, 0, 
     0, 0, 0, 0, 0, 0, 2, 2, 2, 2, 2, 2, 2, 2, 2, 0, 0, 0, 0, 0, 0, 
     0, 0, 0, 0, 2, 2, 0, 0, 0, 2, 1, 2, 0, 0, 0, 2, 2, 0, 0, 0, 0, 
     0, 0, 0, 2, 1, 2, 0, 0, 0, 2, 1, 2, 0, 0, 0, 2, 1, 2, 0, 0, 0, 
     0, 0, 2, 1, 1, 2, 0, 0, 0, 2, 1, 2, 0, 0, 0, 2, 1, 1, 2, 0, 0, 
     0, 2, 1, 1, 1, 2, 2, 2, 2, 2, 1, 2, 2, 2, 2, 2, 1, 1, 1, 2, 0, 
     2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 
     0, 2, 1, 1, 1, 2, 2, 2, 2, 2, 1, 2, 2, 2, 2, 2, 1, 1, 1, 2, 0, 
     0, 0, 2, 1, 1, 2, 0, 0, 0, 2, 1, 2, 0, 0, 0, 2, 1, 1, 2, 0, 0, 
     0, 0, 0, 2, 1, 2, 0, 0, 0, 2, 1, 2, 0, 0, 0, 2, 1, 2, 0, 0, 0, 
     0, 0, 0, 0, 2, 2, 0, 0, 0, 2, 1, 2, 0, 0, 0, 2, 2, 0, 0, 0, 0, 
     0, 0, 0, 0, 0, 0, 2, 2, 2, 2, 2, 2, 2, 2, 2, 0, 0, 0, 0, 0, 0, 
     0, 0, 0, 0, 0, 0, 2, 1, 1, 1, 1, 1, 1, 1, 2, 0, 0, 0, 0, 0, 0, 
     0, 0, 0, 0, 0, 0, 0, 2, 1, 1, 1, 1, 1, 2, 0, 0, 0, 0, 0, 0, 0, 
     0, 0, 0, 0, 0, 0, 0, 0, 2, 1, 1, 1, 2, 0, 0, 0, 0, 0, 0, 0, 0, 
     0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 1, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
     0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 
};


//default sizenesw cursor
static unsigned char _default_cursor_sizenesw[] = {
     15, 15,
     0, 0, 0, 0, 0, 0, 0, 0, 2, 2, 2, 2, 2, 2, 2, 
     0, 0, 0, 0, 0, 0, 0, 0, 2, 1, 1, 1, 1, 1, 2, 
     0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 1, 1, 1, 1, 2, 
     0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 1, 1, 1, 2, 
     0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 1, 2, 1, 1, 2, 
     0, 0, 0, 0, 0, 0, 0, 0, 2, 1, 2, 0, 2, 1, 2, 
     0, 0, 0, 0, 0, 0, 0, 2, 1, 2, 0, 0, 0, 2, 2, 
     0, 0, 0, 0, 0, 0, 2, 1, 2, 0, 0, 0, 0, 0, 0, 
     2, 2, 0, 0, 0, 2, 1, 2, 0, 0, 0, 0, 0, 0, 0, 
     2, 1, 2, 0, 2, 1, 2, 0, 0, 0, 0, 0, 0, 0, 0, 
     2, 1, 1, 2, 1, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
     2, 1, 1, 1, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
     2, 1, 1, 1, 1, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
     2, 1, 1, 1, 1, 1, 2, 0, 0, 0, 0, 0, 0, 0, 0, 
     2, 2, 2, 2, 2, 2, 2, 0, 0, 0, 0, 0, 0, 0, 0 
};


//default sizens cursor
static unsigned char _default_cursor_sizens[] = {
     9, 20,
     0, 0, 0, 0, 2, 0, 0, 0, 0, 
     0, 0, 0, 2, 1, 2, 0, 0, 0, 
     0, 0, 2, 1, 1, 1, 2, 0, 0, 
     0, 2, 1, 1, 1, 1, 1, 2, 0, 
     2, 1, 1, 1, 1, 1, 1, 1, 2, 
     2, 2, 2, 2, 1, 2, 2, 2, 2, 
     0, 0, 0, 2, 1, 2, 0, 0, 0, 
     0, 0, 0, 2, 1, 2, 0, 0, 0, 
     0, 0, 0, 2, 1, 2, 0, 0, 0, 
     0, 0, 0, 2, 1, 2, 0, 0, 0, 
     0, 0, 0, 2, 1, 2, 0, 0, 0, 
     0, 0, 0, 2, 1, 2, 0, 0, 0, 
     0, 0, 0, 2, 1, 2, 0, 0, 0, 
     0, 0, 0, 2, 1, 2, 0, 0, 0, 
     2, 2, 2, 2, 1, 2, 2, 2, 2, 
     2, 1, 1, 1, 1, 1, 1, 1, 2, 
     0, 2, 1, 1, 1, 1, 1, 2, 0, 
     0, 0, 2, 1, 1, 1, 2, 0, 0, 
     0, 0, 0, 2, 1, 2, 0, 0, 0, 
     0, 0, 0, 0, 2, 0, 0, 0, 0 
};


//default sizenwse cursor
static unsigned char _default_cursor_sizenwse[] = {
     15, 15,
     2, 2, 2, 2, 2, 2, 2, 0, 0, 0, 0, 0, 0, 0, 0, 
     2, 1, 1, 1, 1, 1, 2, 0, 0, 0, 0, 0, 0, 0, 0, 
     2, 1, 1, 1, 1, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
     2, 1, 1, 1, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
     2, 1, 1, 2, 1, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
     2, 1, 2, 0, 2, 1, 2, 0, 0, 0, 0, 0, 0, 0, 0, 
     2, 2, 0, 0, 0, 2, 1, 2, 0, 0, 0, 0, 0, 0, 0, 
     0, 0, 0, 0, 0, 0, 2, 1, 2, 0, 0, 0, 0, 0, 0, 
     0, 0, 0, 0, 0, 0, 0, 2, 1, 2, 0, 0, 0, 2, 2, 
     0, 0, 0, 0, 0, 0, 0, 0, 2, 1, 2, 0, 2, 1, 2, 
     0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 1, 2, 1, 1, 2, 
     0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 1, 1, 1, 2, 
     0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 1, 1, 1, 1, 2, 
     0, 0, 0, 0, 0, 0, 0, 0, 2, 1, 1, 1, 1, 1, 2, 
     0, 0, 0, 0, 0, 0, 0, 0, 2, 2, 2, 2, 2, 2, 2 
};


//default sizewe cursor
static unsigned char _default_cursor_sizewe[] = {
     21, 9,
     0, 0, 0, 0, 2, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 2, 0, 0, 0, 0, 
     0, 0, 0, 2, 1, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 1, 2, 0, 0, 0, 
     0, 0, 2, 1, 1, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 1, 1, 2, 0, 0, 
     0, 2, 1, 1, 1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 1, 1, 1, 2, 0, 
     2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 
     0, 2, 1, 1, 1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 1, 1, 1, 2, 0, 
     0, 0, 2, 1, 1, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 1, 1, 2, 0, 0, 
     0, 0, 0, 2, 1, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 1, 2, 0, 0, 0, 
     0, 0, 0, 0, 2, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 2, 0, 0, 0, 0 
};


//default uparrow cursor
static unsigned char _default_cursor_uparrow[] = {
     9, 20,
     0, 0, 0, 0, 2, 0, 0, 0, 0, 
     0, 0, 0, 2, 1, 2, 0, 0, 0, 
     0, 0, 2, 1, 1, 1, 2, 0, 0, 
     0, 2, 1, 1, 1, 1, 1, 2, 0, 
     2, 1, 1, 1, 1, 1, 1, 1, 2, 
     2, 2, 2, 2, 1, 2, 2, 2, 2, 
     0, 0, 0, 2, 1, 2, 0, 0, 0, 
     0, 0, 0, 2, 1, 2, 0, 0, 0, 
     0, 0, 0, 2, 1, 2, 0, 0, 0, 
     0, 0, 0, 2, 1, 2, 0, 0, 0, 
     0, 0, 0, 2, 1, 2, 0, 0, 0, 
     0, 0, 0, 2, 1, 2, 0, 0, 0, 
     0, 0, 0, 2, 1, 2, 0, 0, 0, 
     0, 0, 0, 2, 1, 2, 0, 0, 0, 
     0, 0, 0, 2, 1, 2, 0, 0, 0, 
     0, 0, 0, 2, 1, 2, 0, 0, 0, 
     0, 0, 0, 2, 1, 2, 0, 0, 0, 
     0, 0, 0, 2, 1, 2, 0, 0, 0, 
     0, 0, 0, 2, 1, 2, 0, 0, 0, 
     0, 0, 0, 2, 1, 2, 0, 0, 0 
};


//default wait cursor
static unsigned char _default_cursor_wait[] = {
     13, 22,
     1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 
     1, 1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 1, 1, 
     1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 
     0, 1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 1, 0, 
     0, 1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 1, 0, 
     0, 1, 2, 2, 1, 2, 1, 2, 1, 2, 2, 1, 0, 
     0, 1, 2, 2, 2, 1, 2, 1, 2, 2, 2, 1, 0, 
     0, 1, 1, 2, 2, 2, 1, 2, 2, 2, 1, 1, 0, 
     0, 0, 1, 1, 2, 2, 2, 2, 2, 1, 1, 0, 0, 
     0, 0, 0, 1, 1, 2, 1, 2, 1, 1, 0, 0, 0, 
     0, 0, 0, 0, 1, 1, 2, 1, 1, 0, 0, 0, 0, 
     0, 0, 0, 0, 1, 1, 2, 1, 1, 0, 0, 0, 0, 
     0, 0, 0, 1, 1, 2, 2, 2, 1, 1, 0, 0, 0, 
     0, 0, 1, 1, 2, 2, 1, 2, 2, 1, 1, 0, 0, 
     0, 1, 1, 2, 2, 2, 2, 2, 2, 2, 1, 1, 0, 
     0, 1, 2, 2, 2, 2, 1, 2, 2, 2, 2, 1, 0, 
     0, 1, 2, 2, 2, 1, 2, 1, 2, 2, 2, 1, 0, 
     0, 1, 2, 2, 1, 2, 1, 2, 1, 2, 2, 1, 0, 
     0, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 0, 
     1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 
     1, 1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 1, 1, 
     1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 
};


//default cursor list
static _DEFAULT_CURSOR _default_cursor[] = {
    { _default_cursor_arrow,       1,  1,  1 },
    { _default_cursor_cross,       8,  8,  1 },
    { _default_cursor_hand,        5,  1,  1 },
    { _default_cursor_no,          10, 10, 1 },
    { _default_cursor_wait,        6,  11, 1 },
    { _default_cursor_handwriting, 1,  1,  1 },
    { _default_cursor_help,        1,  1,  1 },
    { _default_cursor_sizeall,     10, 10, 1 },
    { _default_cursor_sizenwse,    7,  7,  1 },
    { _default_cursor_sizenesw,    7,  7,  1 },
    { _default_cursor_sizewe,      10, 4,  1 },
    { _default_cursor_sizens,      4,  10, 1 },
    { _default_cursor_ibeam,       3,  8,  0 },
    { _default_cursor_uparrow,     4,  1,  1 }
};


//creates a cursor from a default cursor array
static BITMAP *_create_mouse_sprite(unsigned char *cursor){
    BITMAP *bmp;
    int x, y, col;
    bmp = create_bitmap(cursor[0], cursor[1]);
    if(!bmp){
        TRACE("Mouse: Failed to Allocate Memory\n");
        return NULL;
    }
    for(y = 0; y < cursor[1]; y++){
        for(x = 0; x < cursor[0]; x++){
            switch(cursor[(y * cursor[0]) + x + 2]){
                case 1:  
                    col = makecol(0, 0, 0);  
                break;
                case 2:
                    col = makecol(255, 255, 255);        
                break;
                default: 
                    col = bmp->vtable->mask_color; 
                break;
            }
            putpixel(bmp, x, y, col);
        }
    }
    return bmp;
}


// Blurs the shadow using its alpha channel
static void _blur_shadow(BITMAP *bmp, int radius) {
    BITMAP *tmp;
    int x, y, i, j;
    int src, dest;
    int a;
    int div = (2 * radius + 1) * (2 * radius + 1);
    tmp = create_bitmap_ex(32, bmp->w + 2 * radius, bmp->h + 2 * radius);
    if(!tmp)
        return;
    clear(tmp);
    blit(bmp, tmp, 0, 0, radius, radius, bmp->w, bmp->h);
    for (y = radius; y < tmp->h - radius; y++) {
        for (x = radius; x < tmp->w - radius; x++) {
            a = 0;
            for (j =- radius; j <= radius; j++) {
                for (i =- radius; i <= radius; i++) {
                    src = getpixel(tmp, x + i, y + j);
                    a += geta32(src);
                }
            }
            if((a / div) > 0)
                dest = makeacol32(0, 0, 0, (a / div));
            else
                dest = makeacol32(255, 0, 255, 0);
            putpixel(bmp, x - radius, y - radius, dest);
        }
    }
    destroy_bitmap(tmp);
}


// Creates a cursor shadow from a cursor bitmap
static BITMAP *_create_shadow(BITMAP *bmp, int alpha) {
    int x, y, a, pink;
    BITMAP *shdw;
    // shadow is a 32 bit RGBA bitmap
    if((shdw = create_bitmap_ex(32, bmp->w + 2 * _shadow_focus, bmp->h + 2 * _shadow_focus)) == NULL)
        return NULL;
    clear(shdw);
    if(alpha){
        for (y = 0; y < bmp->h; y++) {
            for (x = 0; x < bmp->w; x++) {
                a = geta32(getpixel(bmp, x, y));
                putpixel(shdw, x + _shadow_focus, y + _shadow_focus, makeacol32(0, 0, 0, MAX(a - (255 - _shadow_intensity), 0)));
            }
        }
    }
    else{
        pink = makecol(255,0,255);
        for (y = 0; y < bmp->h; y++) {
            for (x = 0; x < bmp->w; x++) {
                if(getpixel(bmp, x, y) != pink)
                    a = makeacol32(0, 0, 0, _shadow_intensity);
                else
                    a = makeacol32(0, 0, 0, 0);
                putpixel(shdw, x + _shadow_focus, y + _shadow_focus, a);
            }
        }
    }
    _blur_shadow(shdw, _shadow_focus);
    return shdw;
}


// Rebuilds the shadow array for a single cursor
static int _rebuild_cursor_shadow(AWE_CURSOR *cursor){
    int j;
    if(!cursor)
        return 0;
    if(cursor->shadow){
        if(cursor->num_frames > 1 && cursor->shadow[0] == cursor->shadow[1]){
            destroy_bitmap(cursor->shadow[0]);
            if(cursor->cur_trans == AWE_MOUSE_TRANS_ALPHA){
                if((cursor->shadow[0] = _create_shadow(cursor->cur[0], TRUE)) == NULL)
                    return 0;
            }
            else{
                if((cursor->shadow[0] = _create_shadow(cursor->cur[0], FALSE)) == NULL)
                    return 0;
            }
            for(j = 1; j < cursor->num_frames; j++)
                cursor->shadow[j] = cursor->shadow[0];
        }
        else{
            for(j = 0; j < cursor->num_frames; j++){
                destroy_bitmap(cursor->shadow[j]);
                if(cursor->cur_trans == AWE_MOUSE_TRANS_ALPHA){
                    if((cursor->shadow[j] = _create_shadow(cursor->cur[j], TRUE)) == NULL)
                        return 0;
                }
                else{
                    if((cursor->shadow[j] = _create_shadow(cursor->cur[j], FALSE)) == NULL)
                        return 0;
                }
            }
        }
        if(cursor == _current_cur && _shadow_bg){
            destroy_bitmap(_shadow_bg);
            _shadow_bg = create_bitmap(cursor->shadow[0]->w, cursor->shadow[0]->h);
        }
    }
    return 1;
}


// Rebuilds the shadow array, useful for changing shadow properties
static void _rebuild_shadows(void){
    int i;
    AWE_DL_NODE *node;
    for(i = 0; i < MOUSE_NUM_CURSORS; i++){
        _rebuild_cursor_shadow(_cursor[i]);
    }
    /* Rebuild user cursor shadows */
    for(node = _user_list.first; node; node = node->next)
        _rebuild_cursor_shadow((AWE_CURSOR*)((AWE_DL_DATA_NODE*)node)->data);
}


// Helper function for drawing and hiding the mouse
static void _draw_mouse(int remove, int add){
    int cur_frame;
    if(!_mouse_screen)
        return;
    if(!_current_cur)
        return;
    if (remove){
        if(_current_cur->shadow && _show_shadow)
            blit(_shadow_bg, _mouse_screen, 0, 0, _old_mx - _current_cur->hotx + _shadow_offsetx - _shadow_focus, _old_my - _current_cur->hoty + _shadow_offsety - _shadow_focus, _shadow_bg->w, _shadow_bg->h);
        blit(_mouse_bg, _mouse_screen, 0, 0, _old_mx - _current_cur->hotx, _old_my - _current_cur->hoty, _mouse_bg->w, _mouse_bg->h);
    }
    if (add) {
        cur_frame = _current_cur->cur_frame;
        if(_current_cur->shadow)
            blit(_mouse_screen, _shadow_bg, _mx - _current_cur->hotx - _shadow_focus + _shadow_offsetx, _my - _current_cur->hoty - _shadow_focus + _shadow_offsety, 0, 0, _shadow_bg->w, _shadow_bg->h);
        blit(_mouse_screen, _mouse_bg, _mx - _current_cur->hotx, _my - _current_cur->hoty, 0, 0, _mouse_bg->w, _mouse_bg->h);
        if(_current_cur->shadow && _show_shadow){
            set_alpha_blender();
            draw_trans_sprite(_mouse_screen, _current_cur->shadow[cur_frame], _mx + _shadow_offsetx - _shadow_focus - _current_cur->hotx, _my + _shadow_offsety - _shadow_focus - _current_cur->hoty);
        }
        if(_current_cur->cur_trans == AWE_MOUSE_TRANS_ALPHA){
            set_alpha_blender();
            draw_trans_sprite(_mouse_screen, _current_cur->cur[cur_frame], _mx - _current_cur->hotx, _my - _current_cur->hoty);
        }
        else if(_current_cur->cur_trans != 0){
            set_trans_blender(0, 0, 0, _current_cur->cur_trans); 
            draw_trans_sprite(_mouse_screen, _current_cur->cur[cur_frame], _mx - _current_cur->hotx, _my - _current_cur->hoty);
        }    
        else
            draw_sprite(_mouse_screen, _current_cur->cur[cur_frame], _mx - _current_cur->hotx, _my - _current_cur->hoty);
    }
}


// Searches a datafile for a name and returns the index
static int _get_dat_idx(DATAFILE *dat, const char *name){
    int i;
    for(i = 0; dat[i].type != DAT_END; i++){
        if(ustricmp(get_datafile_property(dat + i, DAT_ID('N', 'A', 'M', 'E')), name) == 0)
            return i;
    }
    return -1;
}


// Helper function to get a datafile property as an integer with val being returned if the property does not exist.
static int _get_dat_int(DATAFILE *dat, int idx, int prop, int val){
    if(idx > 0){
        const char *tmp;
        tmp = get_datafile_property(dat + idx, prop);
        if(*tmp == '\0')
	    return val;
        return atoi(tmp);
    }
    return val;
}


// Returns 1 if a bounding box collision is detected, 0 otherwise
static int _mouse_collision(int x1, int y1, int w1, int h1, int x2, int y2, int w2, int h2){
        if(y1 + h1 <= y2) 
            return 0;
        if(y1 >= y2 + h2) 
            return 0;
        if(x1 + w1 <= x2) 
            return 0;
        if(x1 >= x2 + w2) 
               return 0;
        return 1;
}


// destroys a mouse cursor
static void _destroy_mouse_cursor(AWE_CURSOR *cursor){
    int j;
    if(!cursor)
        return;
    if(cursor->type == AWE_MOUSE_USER){
        AWE_DL_NODE *node;
        for(node = _user_list.first; node; node = node->next){
            if(((AWE_DL_DATA_NODE*)node)->data == cursor){
                TRACE("Cursor: Removing Custom Cursor\n");
                awe_list_remove(&_user_list, node);
                free(node);
                break;
            }
        }
    }
    if(cursor->cur){
        for(j = 0; j < cursor->num_frames; j++){
            if(cursor->cur[j])
                destroy_bitmap(cursor->cur[j]);
        }
        free(cursor->cur);
    }
    if(cursor->shadow){
        if(cursor->num_frames > 1 && cursor->shadow[0] == cursor->shadow[1]){
            if(cursor->shadow[0])
                destroy_bitmap(cursor->shadow[0]);
        }
        else{
            for(j = 0; j < cursor->num_frames; j++){
                if(cursor->shadow[j])
                    destroy_bitmap(cursor->shadow[j]);
            }
        }
        free(cursor->shadow);
    }
    free(cursor);
}


static AWE_CURSOR *_create_mouse_cursor(BITMAP *cursor, int type, int hotx, int hoty, AWE_CURSOR_SHADOW shadowed, AWE_SHADOW_METHOD shadow_method, int num_frames, int speed, AWE_CURSOR_ANIM anim_type, AWE_TRANS_LEVEL cur_trans){
    int width, j;
    AWE_CURSOR *tmp = (AWE_CURSOR*)malloc(sizeof(AWE_CURSOR));
    if(!tmp)
        return NULL;
    tmp->shadow = NULL;
    tmp->cur = NULL;
    tmp->type = type;
    tmp->hotx = hotx;
    tmp->hoty = hoty;
    tmp->anim_type = anim_type;
    tmp->speed = speed;
    tmp->num_frames = num_frames;
    tmp->cur_trans = cur_trans;
    tmp->cur_frame = 0;
    tmp->timer = 0;
    width = cursor->w / num_frames;
    if((tmp->cur = malloc(num_frames * sizeof(BITMAP*))) == NULL)
	goto _cursor_error;
    if(shadowed){
        if((tmp->shadow = malloc(num_frames * sizeof(BITMAP*))) == NULL)
            goto _cursor_error;
    }
    else
        tmp->shadow = NULL;
    for(j = 0; j < num_frames; j++){
        if((tmp->cur[j] = create_sub_bitmap(cursor, j * width, 0, width, cursor->h)) == NULL)
            goto _cursor_error;
        if(tmp->shadow){
            if(shadow_method && j == 0){
                if(tmp->cur_trans == AWE_MOUSE_TRANS_ALPHA){
                    if((tmp->shadow[j] = _create_shadow(tmp->cur[j], TRUE)) == NULL)
                        goto _cursor_error;
                }
                else{
                    if((tmp->shadow[j] = _create_shadow(tmp->cur[j], FALSE)) == NULL)
                        goto _cursor_error;
                }
            }
            else if(shadow_method && j != 0)
                tmp->shadow[j] = tmp->shadow[0];
            else{
                if(tmp->cur_trans == AWE_MOUSE_TRANS_ALPHA){
                    if((tmp->shadow[j] = _create_shadow(tmp->cur[j], TRUE)) == NULL)
                        goto _cursor_error;
                }
                else{
                   if((tmp->shadow[j] = _create_shadow(tmp->cur[j], FALSE)) == NULL)
                        goto _cursor_error;
                }
            }
        }
    }
    if(type == AWE_MOUSE_USER){
        AWE_DL_DATA_NODE *user_cursor = (AWE_DL_DATA_NODE*)malloc(sizeof(AWE_DL_DATA_NODE));
        if(!user_cursor)
            goto _cursor_error;
        user_cursor->data = tmp;
        user_cursor->node.prev = NULL;
        user_cursor->node.next = NULL;
        TRACE("Cursor: Adding Custom Cursor\n");
        awe_list_insert(&_user_list, (AWE_DL_NODE*)user_cursor, NULL);
    }
    return tmp;
    /* Error: clean up and return NULL */
    _cursor_error:
    // Destroy cursor
    TRACE("Cursor: Error Creating Cursor\n");
    _destroy_mouse_cursor(tmp);
    return NULL;
}


// Destroys all used bitmaps and cursors
static void _destroy_mouse(){
    int i;
    if(!_cursor[0])
        return;
    for(i = 0; i < MOUSE_NUM_CURSORS; i++){
        _destroy_mouse_cursor(_cursor[i]);
        _cursor[i] = NULL;
    }
    if(_mouse_bg){
        destroy_bitmap(_mouse_bg);
        _mouse_bg = NULL;
    }
    if(_shadow_bg){
        destroy_bitmap(_shadow_bg);
        _shadow_bg = NULL;
    }
    if(_cursor_path){
        free(_cursor_path);
        _cursor_path = NULL;
        /* Unload datafile if loaded from a path */
        if(_mouse){
            unload_datafile(_mouse);
            _mouse = NULL;
        }
    }
    TRACE("Cursor: Successfully Destroyed\n");
}


/*****************************************************************************
    PUBLIC
 *****************************************************************************/


// Creates a user cursor
AWE_CURSOR *awe_create_mouse_cursor(BITMAP *cursor, int hotx, int hoty, AWE_CURSOR_SHADOW shadowed, AWE_SHADOW_METHOD shadow_method, int num_frames, int speed, AWE_CURSOR_ANIM anim_type, AWE_TRANS_LEVEL cur_trans){
    return _create_mouse_cursor(cursor, AWE_MOUSE_USER, hotx, hoty, shadowed, shadow_method, num_frames, speed, anim_type, cur_trans);
}


// Destroys a user cursor
void awe_destroy_mouse_cursor(AWE_CURSOR *cursor){
    if(cursor->type == AWE_MOUSE_USER)
        _destroy_mouse_cursor(cursor);
}


// Loads a mouse datafile and initializes all the cursor and shadow arrays
int awe_load_mouse(const char *path){
    int i, alpha = 0, shadowed = 0, shadow_method = 0, default_shadow = 0, default_trans = 0, ext, idx = 0;
    int hotx, hoty, anim_type, speed, num_frames, cur_trans;
    char buffer[1024];
    BITMAP *tmp;
    if(path){
        if((_cursor_path != NULL) && (ustricmp(path, _cursor_path) == 0))
            return 1;
        if(_cursor_path != NULL)
	    free(_cursor_path);
        if((_cursor_path = malloc((ustrlen(path) + uwidth_max(U_CURRENT)) * uwidth_max(U_CURRENT))) == NULL)
            return 0;
        ustrcpy(_cursor_path, path);
    }
    _destroy_mouse();
    /* Check to see if the cursor set contains an alpha channel */
    if(path){
        _mouse = load_datafile(path);
        if(_mouse){
            idx = _get_dat_idx(_mouse, "ALPHA_CFG");
            alpha = _get_dat_int(_mouse, idx, DAT_ID('C', 'F', 'G', 'A'), 0);
        }
    }
    /* Set color conversion if alpha channel is present */
    if(alpha){
        /* Alpha is true so we must reload the datafile with color conversion */
        unload_datafile(_mouse);
        set_color_conversion(COLORCONV_EXPAND_HI_TO_TRUE);
        /* Reload the datafile */
        _mouse = load_datafile(path);
    }
    if(_mouse){
        idx = _get_dat_idx(_mouse, "CURSOR");
        default_trans = _get_dat_int(_mouse, idx, DAT_ID('C', 'U', 'R', 'T'), 0);
        idx = _get_dat_idx(_mouse, "SHADOW");
        _shadow_intensity = _get_dat_int(_mouse, idx, DAT_ID('S', 'D', 'W', 'I'), MOUSE_DEFAULT_INTENSITY);
        _shadow_offsetx = _get_dat_int(_mouse, idx, DAT_ID('S', 'D', 'W', 'X'), MOUSE_DEFAULT_OFFSETX);
        _shadow_offsety = _get_dat_int(_mouse, idx, DAT_ID('S', 'D', 'W', 'Y'), MOUSE_DEFAULT_OFFSETY);
        _shadow_focus = _get_dat_int(_mouse, idx, DAT_ID('S', 'D', 'W', 'F'), MOUSE_DEFAULT_FOCUS);
        default_shadow = _get_dat_int(_mouse, idx, DAT_ID('S', 'H', 'D', 'W'), FALSE);
    }
    for(i = 0; i < MOUSE_NUM_CURSORS; i++){
        tmp = NULL;
        if(_mouse){
            for(ext = 0; _mouse_ext[ext]; ext++){
                usprintf(buffer, "%s_%s", _cur_names[i], _mouse_ext[ext]);
                idx = _get_dat_idx(_mouse, buffer);
                if(idx > 0){
                    tmp = (BITMAP*)_mouse[idx].dat;
                    break;
                }
            }
	    hotx = _get_dat_int(_mouse, idx, DAT_ID('H', 'O', 'T', 'X'), MOUSE_DEFAULT_FOCUSX);
	    hoty = _get_dat_int(_mouse, idx, DAT_ID('H', 'O', 'T', 'Y'), MOUSE_DEFAULT_FOCUSY);
            anim_type = _get_dat_int(_mouse, idx, DAT_ID('A', 'N', 'I', 'T'), AWE_MOUSE_LOOP_REPEAT);
            speed = _get_dat_int(_mouse, idx, DAT_ID('S', 'P', 'E', 'D'), 0);
            num_frames = _get_dat_int(_mouse, idx, DAT_ID('N', 'F', 'R', 'M'), 1);
            cur_trans = _get_dat_int(_mouse, idx, DAT_ID('T', 'R', 'N', 'S'), default_trans);
            shadowed = _get_dat_int(_mouse, idx, DAT_ID('S', 'H', 'D', 'W'), default_shadow);
            shadow_method = _get_dat_int(_mouse, idx, DAT_ID('S', 'D', 'W', 'M'), FALSE);
        }
        if(tmp == NULL){
            if(i < 14){
                if((tmp = _create_mouse_sprite(_default_cursor[i].cursor)) == NULL){
                    TRACE("Cursor: Load Failed\n");
                    return 0;
                }
                TRACE("Cursor: Loading Default Cursor %s\n", _cur_names[i]);
                hotx = _default_cursor[i].hotx;
                hoty = _default_cursor[i].hoty;
                anim_type = AWE_MOUSE_LOOP_REPEAT;
                speed = 0;
                num_frames = 1;
                cur_trans = 0;
                shadowed = _default_cursor[i].shadow;
                shadow_method = FALSE;
                _shadow_intensity = MOUSE_DEFAULT_INTENSITY;
                _shadow_focus = MOUSE_DEFAULT_FOCUS;
                _shadow_offsetx = MOUSE_DEFAULT_OFFSETX;
                _shadow_offsety = MOUSE_DEFAULT_OFFSETY;
            }
            else{
                // Return to top of loop
                continue;
            }
        }    
        _cursor[i] = _create_mouse_cursor(tmp, i, hotx, hoty, shadowed, shadow_method, num_frames, speed, anim_type, cur_trans);
    }
    if(alpha)
        set_color_conversion(COLORCONV_TOTAL);
    awe_set_mouse_cursor(awe_get_system_cursor(AWE_MOUSE_ARROW));
    TRACE("Cursor: %s Loaded Successfully\n", !path ? "Default Cursor" : path);
    return 1;
}


// Loads and initializes all the cursor and shadow arrays from a datafile
int awe_load_datafile_mouse(DATAFILE *dat){
    int i, shadowed = 0, shadow_method = 0, default_shadow = 0, default_trans = 0, ext, idx = 0;
    int hotx, hoty, anim_type, speed, num_frames, cur_trans;
    char buffer[1024];
    BITMAP *tmp;
    _destroy_mouse();
    _mouse = dat;
    if(_mouse){
        idx = _get_dat_idx(_mouse, "CURSOR");
        default_trans = _get_dat_int(_mouse, idx, DAT_ID('C', 'U', 'R', 'T'), 0);
        idx = _get_dat_idx(_mouse, "SHADOW");
        _shadow_intensity = _get_dat_int(_mouse, idx, DAT_ID('S', 'D', 'W', 'I'), MOUSE_DEFAULT_INTENSITY);
        _shadow_offsetx = _get_dat_int(_mouse, idx, DAT_ID('S', 'D', 'W', 'X'), MOUSE_DEFAULT_OFFSETX);
        _shadow_offsety = _get_dat_int(_mouse, idx, DAT_ID('S', 'D', 'W', 'Y'), MOUSE_DEFAULT_OFFSETY);
        _shadow_focus = _get_dat_int(_mouse, idx, DAT_ID('S', 'D', 'W', 'F'), MOUSE_DEFAULT_FOCUS);
        default_shadow = _get_dat_int(_mouse, idx, DAT_ID('S', 'H', 'D', 'W'), FALSE);
    }
    for(i = 0; i < MOUSE_NUM_CURSORS; i++){
        tmp = NULL;
        if(_mouse){
            for(ext = 0; _mouse_ext[ext]; ext++){
                usprintf(buffer, "%s_%s", _cur_names[i], _mouse_ext[ext]);
                idx = _get_dat_idx(_mouse, buffer);
                if(idx > 0){
                    tmp = (BITMAP*)_mouse[idx].dat;
                    break;
                }
            }
	    hotx = _get_dat_int(_mouse, idx, DAT_ID('H', 'O', 'T', 'X'), MOUSE_DEFAULT_FOCUSX);
	    hoty = _get_dat_int(_mouse, idx, DAT_ID('H', 'O', 'T', 'Y'), MOUSE_DEFAULT_FOCUSY);
            anim_type = _get_dat_int(_mouse, idx, DAT_ID('A', 'N', 'I', 'T'), AWE_MOUSE_LOOP_REPEAT);
            speed = _get_dat_int(_mouse, idx, DAT_ID('S', 'P', 'E', 'D'), 0);
            num_frames = _get_dat_int(_mouse, idx, DAT_ID('N', 'F', 'R', 'M'), 1);
            cur_trans = _get_dat_int(_mouse, idx, DAT_ID('T', 'R', 'N', 'S'), default_trans);
            shadowed = _get_dat_int(_mouse, idx, DAT_ID('S', 'H', 'D', 'W'), default_shadow);
            shadow_method = _get_dat_int(_mouse, idx, DAT_ID('S', 'D', 'W', 'M'), FALSE);
        }
        if(tmp == NULL){
            if(i < 14){
                if((tmp = _create_mouse_sprite(_default_cursor[i].cursor)) == NULL){
                    TRACE("Cursor: Load Failed\n");
                    return 0;
                }
                TRACE("Cursor: Loading Default Cursor %s\n", _cur_names[i]);
                hotx = _default_cursor[i].hotx;
                hoty = _default_cursor[i].hoty;
                anim_type = AWE_MOUSE_LOOP_REPEAT;
                speed = 0;
                num_frames = 1;
                cur_trans = 0;
                shadowed = _default_cursor[i].shadow;
                shadow_method = FALSE;
                _shadow_intensity = MOUSE_DEFAULT_INTENSITY;
                _shadow_focus = MOUSE_DEFAULT_FOCUS;
                _shadow_offsetx = MOUSE_DEFAULT_OFFSETX;
                _shadow_offsety = MOUSE_DEFAULT_OFFSETY;
            }
            else{
                // Return to top of loop
                continue;
            }
        }    
        _cursor[i] = _create_mouse_cursor(tmp, i, hotx, hoty, shadowed, shadow_method, num_frames, speed, anim_type, cur_trans);
    }
    awe_set_mouse_cursor(awe_get_system_cursor(AWE_MOUSE_ARROW));
    TRACE("Cursor: Loaded Successfully\n");
    return 1;
}


// Initializes the mouse system
int awe_install_mouse(void){
    if(_mouse_install_count == 0){
        int i;
        for(i = 0; i < MOUSE_NUM_CURSORS; i++)
            _cursor[i] = NULL;
        #ifdef LOADPNG
            loadpng_init();
        #endif
        LOCK_FUNCTION(_timer);
        LOCK_VARIABLE(_mouse_timer);
        install_int_ex(_timer, BPS_TO_TIMER(60));
	_mouse_install_count++;
	TRACE("Cursor: Initialization Successful\n");
	return 1;
    }
    _mouse_install_count++;
    return 0;
}


// Shuts down the mouse system
void awe_mouse_exit(void){
    if(_mouse_install_count == 1){
        AWE_DL_NODE *node;
        AWE_DL_NODE *next;
	awe_show_mouse(NULL);
        remove_int(_timer);
        /* Destroy System Cursors */
        _destroy_mouse();
        /* Destroy User Cursors */
        for(node = _user_list.first; node; ){
            next = node->next;
            _destroy_mouse_cursor((AWE_CURSOR*)((AWE_DL_DATA_NODE*)node)->data);
            node = next;
        }
	TRACE("Cursor: Shutdown Successful\n");
    }
    _mouse_install_count--;
}


// Gets a system cursor
AWE_CURSOR *awe_get_system_cursor(AWE_CURSOR_TYPE type){
    return _cursor[type];
}


AWE_CURSOR *awe_set_mouse_cursor(AWE_CURSOR *cursor){
    BITMAP *old_mouse_screen = _mouse_screen;
    AWE_CURSOR *old_cursor = _current_cur;
    if(!mouse_driver)
        return NULL;
    if(!cursor)
        return _current_cur;
    if(_current_cur == cursor && _mouse_bg)
        return NULL;
    if(_mouse_screen){
        awe_show_mouse(NULL);
        if(old_mouse_screen){
            acquire_bitmap(old_mouse_screen);
            if(_mouse_bg){
                blit(old_mouse_screen, _mouse_vid_screen, _old_mx - _current_cur->hotx, _old_my - _current_cur->hoty, _old_mx - _current_cur->hotx, _old_my - _current_cur->hoty, _mouse_bg->w, _mouse_bg->h);
                blit(old_mouse_screen, _mouse_vid_screen, _mx - _current_cur->hotx, _my - _current_cur->hoty, _mx - _current_cur->hotx, _my - _current_cur->hoty, _mouse_bg->w, _mouse_bg->h);
            }    
            if(_current_cur->shadow){
                blit(old_mouse_screen, _mouse_vid_screen, _old_mx - _current_cur->hotx + _shadow_offsetx - _shadow_focus, _old_my - _current_cur->hoty + _shadow_offsety - _shadow_focus, _old_mx - _current_cur->hotx + _shadow_offsetx - _shadow_focus, _old_my - _current_cur->hoty + _shadow_offsety - _shadow_focus, _shadow_bg->w, _shadow_bg->h);
                blit(old_mouse_screen, _mouse_vid_screen, _mx - _current_cur->hotx + _shadow_offsetx - _shadow_focus, _my - _current_cur->hoty + _shadow_offsety - _shadow_focus, _mx - _current_cur->hotx + _shadow_offsetx - _shadow_focus, _my - _current_cur->hoty + _shadow_offsety - _shadow_focus, _shadow_bg->w, _shadow_bg->h);
            }
            release_bitmap(old_mouse_screen);
        }
    }
    _animation_dir = 1;
    _current_cur = cursor;
    _current_cur->cur_frame = 0;
    /* make sure the bg bitmap is big enough */
    if(_mouse_bg)
        destroy_bitmap(_mouse_bg);
    _mouse_bg = create_bitmap(_current_cur->cur[0]->w, _current_cur->cur[0]->h);
    if(_current_cur->shadow){
        if(_shadow_bg)
            destroy_bitmap(_shadow_bg);
        _shadow_bg = create_bitmap(_current_cur->shadow[0]->w, _current_cur->shadow[0]->h);
    }    
    if(old_mouse_screen){
        awe_show_mouse(old_mouse_screen);
        acquire_bitmap(old_mouse_screen);
        if(_mouse_bg){
            blit(old_mouse_screen, _mouse_vid_screen, _old_mx - _current_cur->hotx, _old_my - _current_cur->hoty, _old_mx - _current_cur->hotx, _old_my - _current_cur->hoty, _mouse_bg->w, _mouse_bg->h);
            blit(old_mouse_screen, _mouse_vid_screen, _mx - _current_cur->hotx, _my - _current_cur->hoty, _mx - _current_cur->hotx, _my - _current_cur->hoty, _mouse_bg->w, _mouse_bg->h);
        }    
        if(_current_cur->shadow){
            blit(old_mouse_screen, _mouse_vid_screen, _old_mx - _current_cur->hotx + _shadow_offsetx - _shadow_focus, _old_my - _current_cur->hoty + _shadow_offsety - _shadow_focus, _old_mx - _current_cur->hotx + _shadow_offsetx - _shadow_focus, _old_my - _current_cur->hoty + _shadow_offsety - _shadow_focus, _shadow_bg->w, _shadow_bg->h);
            blit(old_mouse_screen, _mouse_vid_screen, _mx - _current_cur->hotx + _shadow_offsetx - _shadow_focus, _my - _current_cur->hoty + _shadow_offsety - _shadow_focus, _mx - _current_cur->hotx + _shadow_offsetx - _shadow_focus, _my - _current_cur->hoty + _shadow_offsety - _shadow_focus, _shadow_bg->w, _shadow_bg->h);
        }
        release_bitmap(old_mouse_screen);
    }
    return old_cursor;
}


// Gets the current mouse cursor
AWE_CURSOR *awe_get_mouse_cursor(void){
    return _current_cur;
}


// Sets the bitmap in which the mouse will be drawn to
void awe_show_mouse(BITMAP *bmp){
    if (!mouse_driver)
        return;
    /* Set default vid screen */
    if (!_mouse_vid_screen)
	_mouse_vid_screen = screen;
    /* Set up default cursor */
    if (!_cursor[AWE_MOUSE_ARROW])
        awe_load_mouse(NULL);
    if (_mouse_screen) {
        acquire_bitmap(_mouse_screen);
        _draw_mouse(TRUE, FALSE);
        release_bitmap(_mouse_screen);
    }
    _mouse_screen = bmp;
    if (bmp) {
        acquire_bitmap(_mouse_screen);
        _draw_mouse(FALSE, TRUE);
        release_bitmap(_mouse_screen);
    }
}


// Simply redraws the mouse 
void awe_redraw_mouse(){
    if(!_mouse_screen)
        return;
    _draw_mouse(TRUE, TRUE);
    blit(_mouse_screen, _mouse_vid_screen, _old_mx - _current_cur->hotx, _old_my - _current_cur->hoty, _old_mx - _current_cur->hotx, _old_my - _current_cur->hoty, _mouse_bg->w, _mouse_bg->h);
    blit(_mouse_screen, _mouse_vid_screen, _mx - _current_cur->hotx, _my - _current_cur->hoty, _mx - _current_cur->hotx, _my - _current_cur->hoty, _mouse_bg->w, _mouse_bg->h);
    if(_current_cur->shadow && _show_shadow){
        blit(_mouse_screen, _mouse_vid_screen, _old_mx - _current_cur->hotx + _shadow_offsetx - _shadow_focus, _old_my - _current_cur->hoty + _shadow_offsety - _shadow_focus, _old_mx - _current_cur->hotx + _shadow_offsetx - _shadow_focus, _old_my - _current_cur->hoty + _shadow_offsety - _shadow_focus, _shadow_bg->w, _shadow_bg->h);
        blit(_mouse_screen, _mouse_vid_screen, _mx - _current_cur->hotx + _shadow_offsetx - _shadow_focus, _my - _current_cur->hoty + _shadow_offsety - _shadow_focus, _mx - _current_cur->hotx + _shadow_offsetx - _shadow_focus, _my - _current_cur->hoty + _shadow_offsety - _shadow_focus, _shadow_bg->w, _shadow_bg->h);
    }
}


// Updates the mouse position and animation
void awe_update_mouse(){
    int flag = FALSE;
    if(_current_cur->type != AWE_MOUSE_USER){
        if((mouse_b & 1) && _current_cur->type < 16)
            awe_set_mouse_cursor(awe_get_system_cursor(_current_cur->type + 16));
        else if(!(mouse_b & 1) && _current_cur->type >= 16)
            awe_set_mouse_cursor(awe_get_system_cursor(_current_cur->type - 16));
    }
    if(_current_cur->num_frames > 1  && _current_cur->speed > 0 && _mouse_timer >= _current_cur->timer){
        switch(_current_cur->anim_type){
            case AWE_MOUSE_LOOP_ONCE:
                if(_current_cur->cur_frame < _current_cur->num_frames - 1){
                    _current_cur->cur_frame++;
                    flag = TRUE;
                }
            break;
            case AWE_MOUSE_LOOP_REPEAT:
                _current_cur->cur_frame = (_current_cur->cur_frame + 1) % _current_cur->num_frames;
                flag = TRUE;
            break;
            case AWE_MOUSE_LOOP_BACK:
                if((_current_cur->cur_frame == 0  && _animation_dir == 0) || _current_cur->cur_frame == _current_cur->num_frames - 1)
                    _animation_dir ^= 1;
                if(_animation_dir == 1)
                    _current_cur->cur_frame = _current_cur->cur_frame + 1;
                else
                    _current_cur->cur_frame = _current_cur->cur_frame - 1;
                flag = TRUE;
            break;
        }
        _current_cur->timer = _mouse_timer + _current_cur->speed;
    }
    if(mouse_x != _old_mx || mouse_y != _old_my || flag == TRUE){
        _mx = mouse_x;
        _my = mouse_y;
        awe_redraw_mouse();
        _old_mx = _mx;
        _old_my = _my;
    }
}


// Hides the mouse
void awe_scare_mouse(void){
    if (!mouse_driver)
        return;
    if (_scared_size < MOUSE_SCARED_SIZE) {
        _scared_screen[_scared_size] = _mouse_screen;
        _scared_freeze[_scared_size] = FALSE;
    }
    awe_show_mouse(NULL);
    _scared_size++;
}


// Unhides the mouse
void awe_unscare_mouse(void){
    if (!mouse_driver)
        return;
    if (_scared_size > 0)
        _scared_size--;
    if (_scared_size < MOUSE_SCARED_SIZE) {
        if (_scared_screen[_scared_size])
            awe_show_mouse(_scared_screen[_scared_size]);
        if (_scared_freeze[_scared_size])
            freeze_mouse_flag = FALSE;
        _scared_screen[_scared_size] = NULL;
        _scared_freeze[_scared_size] = FALSE;
    }
}


// Toggles shadows on or off and is also used to adjust shadow properties while it is hidden
void awe_show_mouse_shadow(AWE_CURSOR_SHADOW shadow){
    int tmp = shadow ? TRUE : FALSE;
    if(_show_shadow == tmp)
        return;
    awe_scare_mouse();
    _show_shadow = tmp;
    awe_unscare_mouse();
    if (_mouse_screen){
        acquire_bitmap(_mouse_screen);
        if(_current_cur->shadow){
            blit(_mouse_screen, _mouse_vid_screen, _old_mx - _current_cur->hotx + _shadow_offsetx - _shadow_focus, _old_my - _current_cur->hoty + _shadow_offsety - _shadow_focus, _old_mx - _current_cur->hotx + _shadow_offsetx - _shadow_focus, _old_my - _current_cur->hoty + _shadow_offsety - _shadow_focus, _shadow_bg->w, _shadow_bg->h);
            blit(_mouse_screen, _mouse_vid_screen, _mx - _current_cur->hotx + _shadow_offsetx - _shadow_focus, _my - _current_cur->hoty + _shadow_offsety - _shadow_focus, _mx - _current_cur->hotx + _shadow_offsetx - _shadow_focus, _my - _current_cur->hoty + _shadow_offsety - _shadow_focus, _shadow_bg->w, _shadow_bg->h);
        }
        release_bitmap(_mouse_screen);
    }
}


// Sets the shadows offset
void awe_set_mouse_shadow_offset(int offsetx, int offsety){
    int was = _show_shadow;
    if(_show_shadow && _shadow_offsetx == offsetx && _shadow_offsety == offsety)
        return;
    awe_show_mouse_shadow(FALSE);
    _shadow_offsetx = offsetx;
    _shadow_offsety = offsety;
    awe_show_mouse_shadow(was);
}


// Sets the shadows intensity
void awe_set_mouse_shadow_intensity(int intensity){
    int was = _show_shadow;
    if(intensity <= 0 || _shadow_intensity == intensity)
        return;
    awe_show_mouse_shadow(FALSE);
    _shadow_intensity = intensity;
    set_alpha_blender();
    _rebuild_shadows();
    awe_show_mouse_shadow(was);
}


// Sets the shadows focus
void awe_set_mouse_shadow_focus(int focus){
    int was = _show_shadow;
    if(_shadow_focus == focus)
        return;
    awe_show_mouse_shadow(FALSE);
    _shadow_focus = focus;
    set_alpha_blender();
    _rebuild_shadows();
    awe_show_mouse_shadow(was);
}


// Like mouse_scare_mouse, but it will only hide the mouse if it lies within the area
void awe_scare_mouse_area(int x, int y, int w, int h){
    int x2, y2, w2, h2;
    awe_get_mouse_area(&x2, &y2, &w2, &h2);
    if(_mouse_collision(x, y, w, h, x2, y2, w2, h2))
        awe_scare_mouse();
}


// Returns the portion of the screen the mouse occupies
void awe_get_mouse_area(int *x, int *y, int *w, int *h){
    if(_current_cur->shadow && _show_shadow){
        *x = MIN(_mx - _current_cur->hotx, _mx + _shadow_offsetx - _shadow_focus - _current_cur->hotx);
        *y = MIN(_my - _current_cur->hoty, _my + _shadow_offsety - _shadow_focus - _current_cur->hoty);
        *w = _mouse_bg->w + abs(_shadow_offsetx) + _shadow_focus;
        *h = _mouse_bg->h + abs(_shadow_offsety) + _shadow_focus;
    }
    else{
        *x = _mx - _current_cur->hotx;
        *y = _my - _current_cur->hoty;
        *w = _mouse_bg->w;
        *h = _mouse_bg->h;
    }
}


/** returns the mouse screen
 */
BITMAP *awe_get_mouse_screen() {
    return _mouse_vid_screen;
}


// Sets the mouse screen
void awe_set_mouse_screen(BITMAP *bmp){
    _mouse_vid_screen = bmp;
}


/* Below are just functions to keep the naming convention consistant */


// Sets the cursors position
void awe_position_mouse(int x, int y){
    position_mouse(x, y);
}


// Sets the cursors wheel position
void awe_position_mouse_z(int z){
    position_mouse_z(z);
}


// Sets the cursors x and y range
void awe_set_mouse_range(int x1, int y1, int x2, int y2){
    set_mouse_range(x1, y1, x2, y2);
}


// Sets the cursors x and y speed
void awe_set_mouse_speed(int xspeed, int yspeed){
    set_mouse_speed(xspeed, yspeed);
}


// Gets the cursors change in x and y position
void awe_get_mouse_mickeys(int *mickeyx, int *mickeyy){
    get_mouse_mickeys(mickeyx, mickeyy);
}
