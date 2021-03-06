#ifndef AWE_LABEL_H
#define AWE_LABEL_H

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


/**@name Label
    <p>Label widget.</p>
 */
/*@{*/


///label class name
#define AWE_ID_LABEL		"Label"


enum AWE_LABEL_STATE {
    AWE_LABEL_ENABLED,
    AWE_LABEL_DISABLED,
    AWE_LABEL_NUM_STATES,
};
typedef enum AWE_LABEL_STATE AWE_LABEL_STATE;


///label colors
struct AWE_LABEL_COLOR {
    RGB font_col;
    RGB font_sdw;
};
typedef struct AWE_LABEL_COLOR AWE_LABEL_COLOR;


///label
struct AWE_LABEL {
    AWE_WIDGET widget;
    char *text;
    FONT *font;
    AWE_LABEL_COLOR color[AWE_LABEL_NUM_STATES];
};
typedef struct AWE_LABEL AWE_LABEL;


///label vtable
struct AWE_LABEL_VTABLE {
    AWE_WIDGET_VTABLE widget;
    AWE_GEOMETRY_MANAGER_VTABLE geometry_manager;
};
typedef struct AWE_LABEL_VTABLE AWE_LABEL_VTABLE;


///label vtable
extern AWE_LABEL_VTABLE awe_label_vtable;


///label class
extern AWE_CLASS awe_label_class;


///returns a label interface
void *awe_label_get_interface(AWE_OBJECT *obj, const char *name, const char *pnamespace);


///clones a label
AWE_OBJECT *awe_label_clone(AWE_OBJECT *wgt);


///paints a label
void awe_label_paint(AWE_WIDGET *wgt, AWE_CANVAS *canvas, const AWE_RECT *clip);


///sets a labels geometry
void awe_label_set_geometry(AWE_WIDGET *wgt);


/*@}*/


#ifdef __cplusplus
   }
#endif


#endif //AWE_LABEL_H
