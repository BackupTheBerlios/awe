#ifndef AWE_PUSHBUTTON_H
#define AWE_PUSHBUTTON_H

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


/**@name Push button
    <p>Push button widget.</p>
 */
/*@{*/


///pushbutton class name
#define AWE_ID_PUSH_BUTTON                          "PushButton"


///font property name
#define AWE_ID_FONT                                 "Font"


///bitmap property name
#define AWE_ID_BITMAP                               "Bitmap"


///bitmap direction property name
#define AWE_ID_BITMAP_DIR                           "BitmapDir"


///font color enabled property name
#define AWE_ID_FONT_COLOR_ENABLED                   "FontColorEnabled"


///font color disabled property name
#define AWE_ID_FONT_COLOR_DISABLED                  "FontColorDisabled"


///font color pressed property name
#define AWE_ID_FONT_COLOR_PRESSED                   "FontColorPressed"


///font color highlighted property name
#define AWE_ID_FONT_COLOR_HIGHLIGHTED               "FontColorHighlighted"


///font color focused property name
#define AWE_ID_FONT_COLOR_FOCUSED                   "FontColorFocused"


///font shadow color enabled property name
#define AWE_ID_SHADOW_COLOR_ENABLED                 "ShadowColorEnabled"


///font shadow color disabled property name
#define AWE_ID_SHADOW_COLOR_DISABLED                "ShadowColorDisabled"


///font shadow color pressed property name
#define AWE_ID_SHADOW_COLOR_PRESSED                 "ShadowColorPressed"


///font shadow color highlighted property name
#define AWE_ID_SHADOW_COLOR_HIGHLIGHTED             "ShadowColorHighlighted"


///font shadow color focused property name
#define AWE_ID_SHADOW_COLOR_FOCUSED                 "ShadowColorFocused"


///face color top left enabled property name
#define AWE_ID_FACE_COLOR_TOP_LEFT_ENABLED          "FaceColorTopLeftEnabled"


///face color top left disabled property name
#define AWE_ID_FACE_COLOR_TOP_LEFT_DISABLED         "FaceColorTopLeftDisabed"


///face color top left pressed property name
#define AWE_ID_FACE_COLOR_TOP_LEFT_PRESSED          "FaceColorTopLeftPressed"


///face color top left highlighted property name
#define AWE_ID_FACE_COLOR_TOP_LEFT_HIGHLIGHTED      "FaceColorTopLeftHighlighted"


///face color top left focused property name
#define AWE_ID_FACE_COLOR_TOP_LEFT_FOCUSED          "FaceColorTopLeftFocused"


///face color top right enabled property name
#define AWE_ID_FACE_COLOR_TOP_RIGHT_ENABLED         "FaceColorTopRightEnabled"


///face color top right disabled property name
#define AWE_ID_FACE_COLOR_TOP_RIGHT_DISABLED        "FaceColorTopRightDisabed"


///face color top right pressed property name
#define AWE_ID_FACE_COLOR_TOP_RIGHT_PRESSED         "FaceColorTopRightPressed"


///face color top right highlighted property name
#define AWE_ID_FACE_COLOR_TOP_RIGHT_HIGHLIGHTED     "FaceColorTopRightHighlighted"


///face color top right focused property name
#define AWE_ID_FACE_COLOR_TOP_RIGHT_FOCUSED         "FaceColorTopRightFocused"


///face color bottom left enabled property name
#define AWE_ID_FACE_COLOR_BOTTOM_LEFT_ENABLED       "FaceColorBottomLeftEnabled"


///face color bottom left disabled property name
#define AWE_ID_FACE_COLOR_BOTTOM_LEFT_DISABLED      "FaceColorBottomLeftDisabed"


///face color bottom left pressed property name
#define AWE_ID_FACE_COLOR_BOTTOM_LEFT_PRESSED       "FaceColorBottomLeftPressed"


///face color bottom left highlighted property name
#define AWE_ID_FACE_COLOR_BOTTOM_LEFT_HIGHLIGHTED   "FaceColorBottomLeftHighlighted"


///face color bottom left focused property name
#define AWE_ID_FACE_COLOR_BOTTOM_LEFT_FOCUSED       "FaceColorBottomLeftFocused"


///face color bottom right enabled property name
#define AWE_ID_FACE_COLOR_BOTTOM_RIGHT_ENABLED      "FaceColorBottomRightEnabled"


///face color bottom right disabled property name
#define AWE_ID_FACE_COLOR_BOTTOM_RIGHT_DISABLED     "FaceColorBottomRightDisabed"


///face color bottom right disabled property name
#define AWE_ID_FACE_COLOR_BOTTOM_RIGHT_PRESSED      "FaceColorBottomRightPressed"


///face color bottom right highlighted property name
#define AWE_ID_FACE_COLOR_BOTTOM_RIGHT_HIGHLIGHTED  "FaceColorBottomRightHighlighted"


///face color bottom right focused property name
#define AWE_ID_FACE_COLOR_BOTTOM_RIGHT_FOCUSED      "FaceColorBottomRightFocused"


///edge color top left enabled property name
#define AWE_ID_EDGE_COLOR_TOP_LEFT_ENABLED          "EdgeColorTopLeftEnabled"


///edge color top left disabled property name
#define AWE_ID_EDGE_COLOR_TOP_LEFT_DISABLED         "EdgeColorTopLeftDisabled"


///edge color top left pressed property name
#define AWE_ID_EDGE_COLOR_TOP_LEFT_PRESSED          "EdgeColorTopLeftPressed"


///edge color top left highlighted property name
#define AWE_ID_EDGE_COLOR_TOP_LEFT_HIGHLIGHTED      "EdgeColorTopLeftHighlighted"


///edge color top left focused property name
#define AWE_ID_EDGE_COLOR_TOP_LEFT_FOCUSED          "EdgeColorTopLeftFocused"


///edge color bottom right enabled property name
#define AWE_ID_EDGE_COLOR_BOTTOM_RIGHT_ENABLED      "EdgeColorBottomRightEnabled"


///edge color bottom right disabled property name
#define AWE_ID_EDGE_COLOR_BOTTOM_RIGHT_DISABLED     "EdgeColorBottomRightDisabled"


///edge color bottom right pressed property name
#define AWE_ID_EDGE_COLOR_BOTTOM_RIGHT_PRESSED      "EdgeColorBottomRightPressed"


///edge color bottom right highlighted property name
#define AWE_ID_EDGE_COLOR_BOTTOM_RIGHT_HIGHLIGHTED  "EdgeColorBottomRightHighlighted"


///edge color bottom right focused property name
#define AWE_ID_EDGE_COLOR_BOTTOM_RIGHT_FOCUSED      "EdgeColorBottomRightFocused"


///texture enabled property name
#define AWE_ID_TEXTURE_ENABLED                      "TextureEnabled"


///texture disabled property name
#define AWE_ID_TEXTURE_DISABLED                     "TextureDisabled"


///texture pressed property name
#define AWE_ID_TEXTURE_PRESSED                      "TexturePressed"


///texture highlighted property name
#define AWE_ID_TEXTURE_HIGHLIGHTED                  "TextureHighlighted"


///texture focused property name
#define AWE_ID_TEXTURE_FOCUSED                      "TextureFocused"


///top margin property name
#define AWE_ID_MARGIN_TOP                           "MarginTop"


///left margin property name
#define AWE_ID_MARGIN_LEFT                          "MarginLeft"


///right margin property name
#define AWE_ID_MARGIN_RIGHT                         "MarginRight"


///bottom margin property name
#define AWE_ID_MARGIN_BOTTOM                        "MarginBottom"


///activated event name
#define AWE_ID_PUSH_BUTTON_ACTIVATED                "PushButtonActivated"


///released event name
#define AWE_ID_PUSH_BUTTON_RELEASED                 "PushButtonReleased"


///held down event name
#define AWE_ID_PUSH_BUTTON_HELD_DOWN                "PushButtonHeldDown"


///pressed event name
#define AWE_ID_PUSH_BUTTON_PRESSED                  "PushButtonPressed"


enum AWE_PUSH_BUTTON_EDGE {
    AWE_PUSH_BUTTON_EDGE_TOP_LEFT,
    AWE_PUSH_BUTTON_EDGE_BOTTOM_RIGHT,
    AWE_PUSH_BUTTON_NUM_EDGES,
};
typedef enum AWE_PUSH_BUTTON_EDGE AWE_PUSH_BUTTON_EDGE;


enum AWE_PUSH_BUTTON_FACE {
    AWE_PUSH_BUTTON_FACE_TOP_LEFT,
    AWE_PUSH_BUTTON_FACE_TOP_RIGHT,
    AWE_PUSH_BUTTON_FACE_BOTTOM_LEFT,
    AWE_PUSH_BUTTON_FACE_BOTTOM_RIGHT,
    AWE_PUSH_BUTTON_NUM_FACES,
};
typedef enum AWE_PUSH_BUTTON_FACE AWE_PUSH_BUTTON_FACE;


enum AWE_PUSH_BUTTON_TEXTURE {
    AWE_PUSH_BUTTON_TEXTURE_ENABLED,
    AWE_PUSH_BUTTON_TEXTURE_PRESSED,
    AWE_PUSH_BUTTON_TEXTURE_DISABLED,
    AWE_PUSH_BUTTON_TEXTURE_HIGHLIGHTED,
    AWE_PUSH_BUTTON_TEXTURE_FOCUSED,
    AWE_PUSH_BUTTON_NUM_TEXTURES,
};
typedef enum AWE_PUSH_BUTTON_TEXTURE AWE_PUSH_BUTTON_TEXTURE;


/** bitmap direction
 */
enum AWE_PUSH_BUTTON_BITMAP_DIR {
    ///bitmap positioned to the left of text
    AWE_PUSH_BUTTON_BITMAP_LEFT,

    ///bitmap positioned on top of the text
    AWE_PUSH_BUTTON_BITMAP_TOP,
};
typedef enum AWE_PUSH_BUTTON_ICON_DIR AWE_PUSH_BUTTON_ICON_DIR;


///push button state
struct AWE_PUSH_BUTTON_STATE {
    RGB face_col[AWE_PUSH_BUTTON_NUM_FACES];
    RGB edge_col[AWE_PUSH_BUTTON_NUM_EDGES];
    RGB font_col;
    RGB font_sdw;
    AWE_TEXTURE *texture;
};
typedef struct AWE_PUSH_BUTTON_STATE AWE_PUSH_BUTTON_STATE;


///push button
struct AWE_PUSH_BUTTON {
    AWE_WIDGET widget;
    int pressed:1;
    char *text;
    FONT *font;
    BITMAP *bitmap;
    int bitmap_dir:1;
    AWE_RECT margin;
    AWE_PUSH_BUTTON_STATE texture[AWE_PUSH_BUTTON_NUM_TEXTURES];
};
typedef struct AWE_PUSH_BUTTON AWE_PUSH_BUTTON;


///push button vtable
struct AWE_PUSH_BUTTON_VTABLE {
    AWE_WIDGET_VTABLE widget;
    AWE_CONTROL_VTABLE control;
    AWE_GEOMETRY_MANAGER_VTABLE geometry_manager;
};
typedef struct AWE_PUSH_BUTTON_VTABLE AWE_PUSH_BUTTON_VTABLE;


///push button vtable
extern AWE_PUSH_BUTTON_VTABLE awe_push_button_vtable;


///push button class
extern AWE_CLASS awe_push_button_class;


///returns a push button interface
void *awe_push_button_get_interface(AWE_OBJECT *obj, const char *name, const char *pnamespace);


///clones a push button
AWE_OBJECT *awe_push_button_clone(AWE_OBJECT *wgt);


///paints a push button
void awe_push_button_paint(AWE_WIDGET *wgt, AWE_CANVAS *canvas, const AWE_RECT *clip);


///push button down event
void awe_push_button_down(AWE_WIDGET *wgt, const AWE_EVENT *event);


///push button up event
void awe_push_button_up(AWE_WIDGET *wgt, const AWE_EVENT *event);


///push button keypressed event
int awe_push_button_key_press(AWE_WIDGET *wgt, const AWE_EVENT *event);


///push button got mouse event
void awe_push_button_mouse_enter(AWE_WIDGET *wgt, const AWE_EVENT *event);


///push button lost mose event
void awe_push_button_mouse_leave(AWE_WIDGET *wgt, const AWE_EVENT *event);


///push button timer event
void awe_push_button_timer(AWE_WIDGET *wgt, const AWE_EVENT *event);


///sets the push button geometry
void awe_push_button_set_geometry(AWE_WIDGET *wgt);


/*@}*/


#ifdef __cplusplus
   }
#endif


#endif //AWE_PUSHBUTTON_H
