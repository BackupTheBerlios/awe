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


#define AWE_ID_PUSH_BUTTON	  		"PushButton"


#define AWE_ID_FONT		  		"Font"


#define AWE_ID_ICON		  		"Icon"
#define AWE_ID_ICON_DIR		  		"IconDir"


#define AWE_ID_FONT_COLOR_ENABLED		"FontColorEnabled"
#define AWE_ID_FONT_COLOR_DISABLED		"FontColorDisabled"
#define AWE_ID_FONT_COLOR_PRESSED		"FontColorPressed"
#define AWE_ID_FONT_COLOR_HIGHLIGHTED   	"FontColorHighlighted"
#define AWE_ID_FONT_COLOR_FOCUSED		"FontColorFocused"


#define AWE_ID_SHADOW_COLOR_ENABLED     	"ShadowColorEnabled"
#define AWE_ID_SHADOW_COLOR_DISABLED		"ShadowColorDisabled"
#define AWE_ID_SHADOW_COLOR_PRESSED		"ShadowColorPressed"
#define AWE_ID_SHADOW_COLOR_HIGHLIGHTED 	"ShadowColorHighlighted"
#define AWE_ID_SHADOW_COLOR_FOCUSED		"ShadowColorFocused"


#define AWE_ID_FACE_COLOR_LEFT_ENABLED		"FaceColorLeftEnabled"
#define AWE_ID_FACE_COLOR_LEFT_DISABLED		"FaceColorLeftDisabed"
#define AWE_ID_FACE_COLOR_LEFT_PRESSED		"FaceColorLeftPressed"
#define AWE_ID_FACE_COLOR_LEFT_HIGHLIGHTED	"FaceColorLeftHighlighted"
#define AWE_ID_FACE_COLOR_LEFT_FOCUSED		"FaceColorLeftFocused"


#define AWE_ID_FACE_COLOR_RIGHT_ENABLED		"FaceColorRightEnabled"
#define AWE_ID_FACE_COLOR_RIGHT_DISABLED	"FaceColorRightDisabed"
#define AWE_ID_FACE_COLOR_RIGHT_PRESSED		"FaceColorRightPressed"
#define AWE_ID_FACE_COLOR_RIGHT_HIGHLIGHTED	"FaceColorRightHighlighted"
#define AWE_ID_FACE_COLOR_RIGHT_FOCUSED		"FaceColorRightFocused"


#define AWE_ID_FACE_COLOR_TOP_ENABLED		"FaceColorTopEnabled"
#define AWE_ID_FACE_COLOR_TOP_DISABLED		"FaceColorTopDisabed"
#define AWE_ID_FACE_COLOR_TOP_PRESSED		"FaceColorTopPressed"
#define AWE_ID_FACE_COLOR_TOP_HIGHLIGHTED	"FaceColorTopHighlighted"
#define AWE_ID_FACE_COLOR_TOP_FOCUSED		"FaceColorTopFocused"


#define AWE_ID_FACE_COLOR_BOTTOM_ENABLED	"FaceColorBottomEnabled"
#define AWE_ID_FACE_COLOR_BOTTOM_DISABLED	"FaceColorBottomDisabed"
#define AWE_ID_FACE_COLOR_BOTTOM_PRESSED	"FaceColorBottomPressed"
#define AWE_ID_FACE_COLOR_BOTTOM_HIGHLIGHTED	"FaceColorBottomHighlighted"
#define AWE_ID_FACE_COLOR_BOTTOM_FOCUSED	"FaceColorBottomFocused"


#define AWE_ID_EDGE_COLOR_TOPLEFT     		"EdgeColorTopLeft"
#define AWE_ID_EDGE_COLOR_BOTTOMRIGHT 		"EdgeColorBottomRight"


#define AWE_ID_TEXTURE_ENABLED			"TextureEnabled"
#define AWE_ID_TEXTURE_DISABLED			"TextureDisabled"
#define AWE_ID_TEXTURE_PRESSED			"TexturePressed"
#define AWE_ID_TEXTURE_HIGHLIGHTED		"TextureHighlighted"
#define AWE_ID_TEXTURE_FOCUSED			"TextureFocused"


#define AWE_ID_ANIM_ENABLED			"AnimEnabled"
#define AWE_ID_ANIM_DISABLED			"AnimDisabled"
#define AWE_ID_ANIM_PRESSED			"AnimPressed"
#define AWE_ID_ANIM_HIGHLIGHTED			"AnimHighlighted"
#define AWE_ID_ANIM_FOCUSED			"AnimFocused"


enum AWE_PUSH_BUTTON_EDGE {
    AWE_PUSH_BUTTON_TOP_LEFT,
    AWE_PUSH_BUTTON_BOTTOM_RIGHT,
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


enum AWE_PUSH_BUTTON_STATE {
    AWE_PUSH_BUTTON_ENABLED,
    AWE_PUSH_BUTTON_PRESSED,
    AWE_PUSH_BUTTON_GOTMOUSE,
};
typedef enum AWE_PUSH_BUTTON_STATE AWE_PUSH_BUTTON_STATE;


enum AWE_PUSH_BUTTON_TEXTURE {
    AWE_PUSH_BUTTON_TEXTURE_ENABLED,
    AWE_PUSH_BUTTON_TEXTURE_PRESSED,
    AWE_PUSH_BUTTON_TEXTURE_DISABLED,
    AWE_PUSH_BUTTON_TEXTURE_HIGHLIGHTED,
    AWE_PUSH_BUTTON_TEXTURE_FOCUSED,
    AWE_PUSH_BUTTON_NUM_TEXTURES,
};
typedef enum AWE_PUSH_BUTTON_TEXTURE AWE_PUSH_BUTTON_TEXTURE;


//push button
struct AWE_PUSH_BUTTON {
    AWE_WIDGET widget;
    AWE_PUSH_BUTTON_STATE state;
    char *text;
    FONT *font;
    BITMAP *icon;
    int icon_dir:1;
    AWE_RECT margin;
    RGB *face_col[AWE_PUSH_BUTTON_NUM_FACES][AWE_PUSH_BUTTON_NUM_TEXTURES];
    RGB *edge_col[AWE_PUSH_BUTTON_NUM_EDGES];
    RGB *font_col[AWE_PUSH_BUTTON_NUM_TEXTURES];
    RGB *font_sdw[AWE_PUSH_BUTTON_NUM_TEXTURES];
    AWE_TEXTURE **texture[AWE_PUSH_BUTTON_NUM_TEXTURES];
    AWE_SKIN_ANIM *anim[AWE_PUSH_BUTTON_NUM_TEXTURES];
};
typedef struct AWE_PUSH_BUTTON AWE_PUSH_BUTTON;


//push button vtable
struct AWE_PUSH_BUTTON_VTABLE {
    AWE_WIDGET_VTABLE widget;
    AWE_CONTROL_VTABLE control;
    AWE_GEOMETRY_MANAGER_VTABLE geometry_manager;
};
typedef struct AWE_PUSH_BUTTON_VTABLE AWE_PUSH_BUTTON_VTABLE;


//push button vtable
extern AWE_PUSH_BUTTON_VTABLE awe_push_button_vtable;


//push button class
extern AWE_CLASS awe_push_button_class;


void *awe_push_button_get_interface(AWE_OBJECT *obj, const char *name, const char *pnamespace);
AWE_OBJECT *awe_push_button_clone(AWE_OBJECT *wgt);
void awe_push_button_paint(AWE_WIDGET *wgt, AWE_CANVAS *canvas, const AWE_RECT *clip);
void awe_push_button_down(AWE_WIDGET *wgt, const AWE_EVENT *event);
void awe_push_button_up(AWE_WIDGET *wgt, const AWE_EVENT *event);
int awe_push_button_key_press(AWE_WIDGET *wgt, const AWE_EVENT *event);
int awe_push_button_lose_focus(AWE_WIDGET *wgt);
void awe_push_button_mouse_enter(AWE_WIDGET *wgt, const AWE_EVENT *event);
void awe_push_button_mouse_leave(AWE_WIDGET *wgt, const AWE_EVENT *event);
void awe_push_button_set_geometry(AWE_WIDGET *wgt);
void awe_push_button_geometry_changed(AWE_WIDGET *wgt, int x, int y, int width, int height);
void awe_push_button_z_order_changed(AWE_WIDGET *wgt, int z);
void awe_push_button_visible_changed(AWE_WIDGET *wgt, int visible);


#ifdef __cplusplus
   }
#endif


#endif //AWE_PUSHBUTTON_H
