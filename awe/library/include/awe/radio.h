#ifndef AWE_RADIO_H
#define AWE_RADIO_H

#include <string.h>
#include "checkbox.h"
#include "event.h"
#include "widget.h"
#include "control.h"
#include "dataobjects.h"
#include "geomman.h"
#include "skin.h"

#ifdef __cplusplus
   extern "C" {
#endif


/**@name Radio
    <p>Radio button widget.</p>
    <p>The radio widget is derived from the checkbox widget.</p>
 */
/*@{*/


///radio class name
#define AWE_ID_RADIO                    "Radio"


///radio group property
#define AWE_ID_GROUP			"Group"


///radio checked event
#define AWE_ID_RADIO_CHECKED            "RadioChecked"


///radio unchecked event
#define AWE_ID_RADIO_UNCHECKED          "RadioUnchecked"


///radio held down event
#define AWE_ID_RADIO_HELD_DOWN          "RadioHeldDown"


///radio
struct AWE_RADIO {
    AWE_CHECKBOX btn;
    int group;
};
typedef struct AWE_RADIO AWE_RADIO;


///radio vtable
struct AWE_RADIO_VTABLE {
    AWE_WIDGET_VTABLE widget;
    AWE_CONTROL_VTABLE control;
    AWE_GEOMETRY_MANAGER_VTABLE geometry_manager;
    void (*radio_check)(AWE_WIDGET *wgt);
};
typedef struct AWE_RADIO_VTABLE AWE_RADIO_VTABLE;


///radio vtable
extern AWE_RADIO_VTABLE awe_radio_vtable;


///radio class
extern AWE_CLASS awe_radio_class;


///clone a radio button
extern AWE_OBJECT *awe_radio_clone(AWE_OBJECT *wgt);


void *awe_radio_get_interface(AWE_OBJECT *obj, const char *name, const char *pnamespace);


void awe_radio_paint(AWE_WIDGET *wgt, AWE_CANVAS *canvas, const AWE_RECT *clip);
void awe_radio_down(AWE_WIDGET *wgt, const AWE_EVENT *event);
void awe_radio_up(AWE_WIDGET *wgt, const AWE_EVENT *event);
void awe_radio_timer(AWE_WIDGET *wgt, const AWE_EVENT *event);


/*@}*/


#ifdef __cplusplus
   }
#endif


#endif //AWE_RADIO_H
