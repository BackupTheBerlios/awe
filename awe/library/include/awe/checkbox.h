#ifndef AWE_CHECKBOX_H
#define AWE_CHECKBOX_H

#include <string.h>
#include "togglebutton.h"
#include "event.h"
#include "widget.h"
#include "control.h"
#include "dataobjects.h"
#include "geomman.h"
#include "skin.h"

#ifdef __cplusplus
   extern "C" {
#endif


/**@name Checkbox
    <p>Checkbox widget.</p>
    <p>The checkbox widget is derived from the toggle button widget.</p>
 */
/*@{*/


///checkbox class name
#define AWE_ID_CHECKBOX                 "Checkbox"


///background color
#define AWE_ID_BACKGROUND               "Background"


///text direction
#define AWE_ID_TEXT_DIRECTION           "TextDirection"


///checkbox checked event
#define AWE_ID_CHECKBOX_CHECKED         "CheckboxChecked"


///checkbox unchecked event
#define AWE_ID_CHECKBOX_UNCHECKED       "CheckboxUnchecked"


///checkbox held down event
#define AWE_ID_CHECKBOX_HELD_DOWN       "CheckboxHeldDown"


///text direction
enum AWE_TEXT_DIRECTION {
    ///text drawn on the left
    AWE_TEXT_DIRECTION_LEFT,

    ///text drawn on the right
    AWE_TEXT_DIRECTION_RIGHT,
};
typedef enum AWE_TEXT_DIRECTION AWE_TEXT_DIRECTION;


///checkbox
struct AWE_CHECKBOX {
    AWE_TOGGLE_BUTTON btn;
    RGB bg;
    int text_dir:1;
};
typedef struct AWE_CHECKBOX AWE_CHECKBOX;


///checkbox vtable
extern AWE_PUSH_BUTTON_VTABLE awe_checkbox_vtable;


///checkbox class
extern AWE_CLASS awe_checkbox_class;


///clones a checkbox
extern AWE_OBJECT *awe_checkbox_clone(AWE_OBJECT *wgt);


void awe_checkbox_paint(AWE_WIDGET *wgt, AWE_CANVAS *canvas, const AWE_RECT *clip);
void awe_checkbox_down(AWE_WIDGET *wgt, const AWE_EVENT *event);
void awe_checkbox_up(AWE_WIDGET *wgt, const AWE_EVENT *event);
void awe_checkbox_timer(AWE_WIDGET *wgt, const AWE_EVENT *event);
void awe_checkbox_set_geometry(AWE_WIDGET *wgt);


/*@}*/


#ifdef __cplusplus
   }
#endif


#endif //AWE_CHECKBOX_H
