#ifndef AWE_TOGGLEBUTTON_H
#define AWE_TOGGLEBUTTON_H

#include <string.h>
#include "pushbutton.h"
#include "event.h"
#include "widget.h"
#include "control.h"
#include "dataobjects.h"
#include "geomman.h"
#include "skin.h"

#ifdef __cplusplus
   extern "C" {
#endif


/**@name Toggle button
    <p>Toggle button widget.</p>
    <p>The toggle button widget is derived from the push button widget, see the push button link for all available toggle button properties.</p>
 */
/*@{*/


///toggle button class name
#define AWE_ID_TOGGLE_BUTTON            "ToggleButton"


///toggled state
#define AWE_ID_TOGGLED			"Toggled"


///toggle button released event name
#define AWE_ID_TOGGLE_BUTTON_RELEASED   "ToggleButtonReleased"


///toggle button held down event name
#define AWE_ID_TOGGLE_BUTTON_HELD_DOWN  "ToggleButtonHeldDown"


///toggle button pressed event name
#define AWE_ID_TOGGLE_BUTTON_PRESSED    "ToggleButtonPressed"


///toggle button
struct AWE_TOGGLE_BUTTON {
    AWE_PUSH_BUTTON btn;
    int toggle:1;
};
typedef struct AWE_TOGGLE_BUTTON AWE_TOGGLE_BUTTON;


///toggle button vtable
extern AWE_PUSH_BUTTON_VTABLE awe_toggle_button_vtable;


///toggle button class
extern AWE_CLASS awe_toggle_button_class;


///clones a toggle button
extern AWE_OBJECT *awe_toggle_button_clone(AWE_OBJECT *wgt);


///toggle button down event
void awe_toggle_button_down(AWE_WIDGET *wgt, const AWE_EVENT *event);


///toggle button up event
void awe_toggle_button_up(AWE_WIDGET *wgt, const AWE_EVENT *event);


///toggle button mouse enter event
void awe_toggle_button_mouse_enter(AWE_WIDGET *wgt, const AWE_EVENT *event);


///toggle button mouse leave event
void awe_toggle_button_mouse_leave(AWE_WIDGET *wgt, const AWE_EVENT *event);


///toggle button timer event
void awe_toggle_button_timer(AWE_WIDGET *wgt, const AWE_EVENT *event);


/*@}*/


#ifdef __cplusplus
   }
#endif


#endif //AWE_TOGGLEBUTTON_H
