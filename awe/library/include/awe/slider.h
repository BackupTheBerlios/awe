#ifndef AWE_SLIDER_H
#define AWE_SLIDER_H

#include <string.h>
#include "event.h"
#include "widget.h"
#include "control.h"
#include "dataobjects.h"
#include "geomman.h"
#include "skin.h"

#ifdef __cplusplus
   extern "C" {
#endif


/**@name Slider
    <p>Slider widget.</p>
 */
/*@{*/


///slider class name
#define AWE_ID_SLIDER         "Slider"


///position property
#define AWE_ID_POSITION       "Position"


///maximum value property
#define AWE_ID_MAX            "Max"


///orientation property
#define AWE_ID_ORIENTATION    "Orientation"


///handle width property
#define AWE_ID_HANDLE_WIDTH   "HandleWidth"


///handle type property
#define AWE_ID_HANDLE_TYPE    "HandleType"


///step property
#define AWE_ID_STEP           "Step"


///moved event name
#define AWE_ID_SLIDER_MOVED   "SliderMoved"


///slider orientation
enum AWE_SLIDER_ORIENTATION {
    ///horiztonal slider
    AWE_SLIDER_HORIZONTAL,

    ///vertical slider
    AWE_SLIDER_VERTICAL,    
};
typedef enum AWE_SLIDER_ORIENTATION AWE_SLIDER_ORIENTATION;


enum AWE_SLIDER_EDGE {
    AWE_SLIDER_EDGE_TOP_LEFT,
    AWE_SLIDER_EDGE_BOTTOM_RIGHT,
    AWE_SLIDER_NUM_EDGES,
};
typedef enum AWE_SLIDER_EDGE AWE_SLIDER_EDGE;


enum AWE_SLIDER_TEXTURE {
    AWE_SLIDER_TEXTURE_ENABLED,
    AWE_SLIDER_TEXTURE_PRESSED,
    AWE_SLIDER_TEXTURE_DISABLED,
    AWE_SLIDER_TEXTURE_HIGHLIGHTED,
    AWE_SLIDER_NUM_TEXTURES,
};
typedef enum AWE_SLIDER_TEXTURE AWE_SLIDER_TEXTURE;


enum AWE_SLIDER_HANDLE_TYPE {
    AWE_SLIDER_HANDLE_NORMAL = 0,
    AWE_SLIDER_HANDLE_LEFT   = 1,
    AWE_SLIDER_HANDLE_RIGHT  = 2,
    AWE_SLIDER_HANDLE_UP     = 1,
    AWE_SLIDER_HANDLE_DOWN   = 2,
};
typedef enum AWE_SLIDER_HANDLE_TYPE AWE_SLIDER_HANDLE_TYPE;


///slider
struct AWE_SLIDER {
    AWE_WIDGET widget;
    RGB bar_col[AWE_SLIDER_NUM_EDGES];
    RGB edge_col[AWE_SLIDER_NUM_TEXTURES][AWE_SLIDER_NUM_EDGES];
    RGB face_col[AWE_SLIDER_NUM_TEXTURES];
    AWE_SLIDER_HANDLE_TYPE handle_type;
    AWE_SLIDER_ORIENTATION orientation;
    int handle_width;
    int pos;
    int old_pos;
    int max_val;
    int step;
    int step_pos;
    int oz;
    int highlighted:1;
    int pressed:1;
    int lostmouse:1;
};
typedef struct AWE_SLIDER AWE_SLIDER;


///slider vtable
struct AWE_SLIDER_VTABLE {
    AWE_WIDGET_VTABLE widget;
    AWE_CONTROL_VTABLE control;
    AWE_GEOMETRY_MANAGER_VTABLE geometry_manager;
};
typedef struct AWE_SLIDER_VTABLE AWE_SLIDER_VTABLE;


///slider vtable
extern AWE_SLIDER_VTABLE awe_slider_vtable;


///slider class
extern AWE_CLASS awe_slider_class;


///returns a slider interface
void *awe_slider_get_interface(AWE_OBJECT *obj, const char *name, const char *pnamespace);


///clones a slider
AWE_OBJECT *awe_slider_clone(AWE_OBJECT *wgt);


///paints a slider
void awe_slider_paint(AWE_WIDGET *wgt, AWE_CANVAS *canvas, const AWE_RECT *clip);


///slider button down
void awe_slider_button_down(AWE_WIDGET *wgt, const AWE_EVENT *event);


///slider button up
void awe_slider_button_up(AWE_WIDGET *wgt, const AWE_EVENT *event);


///slider mouse enter
void awe_slider_mouse_enter(AWE_WIDGET *wgt, const AWE_EVENT *event);


///slider mouse leave
void awe_slider_mouse_leave(AWE_WIDGET *wgt, const AWE_EVENT *event);


//slider mouse move
void awe_slider_mouse_move(AWE_WIDGET *wgt, const AWE_EVENT *event);


///slider wheel movement
void awe_slider_mouse_wheel(AWE_WIDGET *wgt, const AWE_EVENT *event);


/*@}*/


#ifdef __cplusplus
   }
#endif


#endif //AWE_SLIDER_H
