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


#define AWE_ID_TOGGLE_BUTTON		"ToggleButton"

#define AWE_ID_TOGGLE_BUTTON_RELEASED	"PushButtonReleased"
#define AWE_ID_TOGGLE_BUTTON_HELD_DOWN	"PushButtonHeldDown"
#define AWE_ID_TOGGLE_BUTTON_PRESSED	"PushButtonPressed"

//toggle button vtable
extern AWE_PUSH_BUTTON_VTABLE awe_toggle_button_vtable;


//toggle button class
extern AWE_CLASS awe_toggle_button_class;


void awe_toggle_button_down(AWE_WIDGET *wgt, const AWE_EVENT *event);
void awe_toggle_button_up(AWE_WIDGET *wgt, const AWE_EVENT *event);


#ifdef __cplusplus
   }
#endif


#endif //AWE_TOGGLEBUTTON_H
