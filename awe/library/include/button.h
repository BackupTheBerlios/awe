#ifndef AWE_BUTTON_H
#define AWE_BUTTON_H

#include <string.h>
#include "event.h"
#include "widget.h"
#include "control.h"
#include "dataobjects.h"

#ifdef __cplusplus
   extern "C" {
#endif


//button
struct AWE_BUTTON {
    AWE_WIDGET widget;
    int state;
    char *text;
};
typedef struct AWE_BUTTON AWE_BUTTON;


//button vtable
struct AWE_BUTTON_VTABLE {
    AWE_WIDGET_VTABLE widget;
    AWE_CONTROL_VTABLE control;
};
typedef struct AWE_BUTTON_VTABLE AWE_BUTTON_VTABLE;


//button properties
extern AWE_CLASS_PROPERTY awe_button_properties[];


//button vtable
extern AWE_BUTTON_VTABLE awe_button_vtable;


//button class
extern AWE_CLASS awe_button_class;


void awe_button_constructor(AWE_OBJECT *obj);
void awe_button_destructor(AWE_OBJECT *obj);
void awe_button_get_text(AWE_OBJECT *obj, void *data);
void awe_button_set_text(AWE_OBJECT *obj, void *data);
void *awe_button_get_interface(AWE_OBJECT *obj, const char *name, const char *pnamespace);
void awe_button_paint(AWE_WIDGET *wgt, AWE_CANVAS *canvas, const AWE_RECT *clip);
void awe_button_down(AWE_WIDGET *wgt, const AWE_EVENT *event);
void awe_button_up(AWE_WIDGET *wgt, const AWE_EVENT *event);
int awe_button_key_press(AWE_WIDGET *wgt, const AWE_EVENT *event);


#ifdef __cplusplus
   }
#endif


#endif //AWE_BUTTON_H
