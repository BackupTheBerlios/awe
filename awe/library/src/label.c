#include "label.h"


/*****************************************************************************
    INTERNALS
 *****************************************************************************/


static RGB _font_color_enabled    = { 0  , 0  , 0  , 0 };
static RGB _font_color_disabled   = { 128, 128, 128, 0 };
static RGB _shadow_color_enabled  = { 255, 0  , 255, 0 };  //Magenta will not show
static RGB _shadow_color_disabled = { 255, 255, 255, 0 };


//constructor
static void _label_constructor(AWE_OBJECT *obj)
{
    AWE_LABEL *tmp = (AWE_LABEL *)obj;
    tmp->text = ustrdup(empty_string);
    tmp->font = font;
    memcpy(&tmp->color[AWE_LABEL_ENABLED].font_col, &_font_color_enabled, sizeof(RGB));
    memcpy(&tmp->color[AWE_LABEL_DISABLED].font_col, &_font_color_disabled, sizeof(RGB));
    memcpy(&tmp->color[AWE_LABEL_ENABLED].font_sdw, &_shadow_color_enabled, sizeof(RGB));
    memcpy(&tmp->color[AWE_LABEL_DISABLED].font_sdw, &_shadow_color_disabled, sizeof(RGB));
}


//destructor
static void _label_destructor(AWE_OBJECT *obj)
{
    free(((AWE_LABEL *)obj)->text);
}


//gets the text
static void _label_get_text(AWE_OBJECT *obj, void *data)
{
    *(const char **)data = ((AWE_LABEL *)obj)->text;
}


//sets the text
static void _label_set_text(AWE_OBJECT *obj, void *data)
{
    const char *new_text = *(const char **)data;
    free(((AWE_LABEL *)obj)->text);
    ((AWE_LABEL *)obj)->text = ustrdup(new_text ? new_text : empty_string);
    awe_set_widget_dirty((AWE_WIDGET *)obj);
}


//gets the font
static void _label_get_font(AWE_OBJECT *obj, void *data)
{
    *(FONT **)data = ((AWE_LABEL *)obj)->font;
}


//sets the font
static void _label_set_font(AWE_OBJECT *obj, void *data)
{ 
    ((AWE_LABEL *)obj)->font = *(FONT **)data;
    awe_set_widget_dirty((AWE_WIDGET *)obj);
}


//gets the enabled font color
static void _label_get_font_color_enabled(AWE_OBJECT *obj, void *data)
{
    *(RGB *)data = ((AWE_LABEL *)obj)->color[AWE_LABEL_ENABLED].font_col;
}


//sets the enabled font color
static void _label_set_font_color_enabled(AWE_OBJECT *obj, void *data)
{
    ((AWE_LABEL *)obj)->color[AWE_LABEL_ENABLED].font_col = *(RGB *)data;
    awe_set_widget_dirty((AWE_WIDGET *)obj);
}


//gets the disabled font color
static void _label_get_font_color_disabled(AWE_OBJECT *obj, void *data)
{
    *(RGB *)data = ((AWE_LABEL *)obj)->color[AWE_LABEL_DISABLED].font_col;
}


//sets the disabled font color
static void _label_set_font_color_disabled(AWE_OBJECT *obj, void *data)
{
    ((AWE_LABEL *)obj)->color[AWE_LABEL_DISABLED].font_col = *(RGB *)data;
    awe_set_widget_dirty((AWE_WIDGET *)obj);
}


//gets the enabled font shadow
static void _label_get_font_shadow_enabled(AWE_OBJECT *obj, void *data)
{
    *(RGB *)data = ((AWE_LABEL *)obj)->color[AWE_LABEL_ENABLED].font_sdw;
}


//sets the enabled font shadow
static void _label_set_font_shadow_enabled(AWE_OBJECT *obj, void *data)
{
    ((AWE_LABEL *)obj)->color[AWE_LABEL_ENABLED].font_sdw = *(RGB *)data;
    awe_set_widget_dirty((AWE_WIDGET *)obj);
}


//gets the disabled font shadow
static void _label_get_font_shadow_disabled(AWE_OBJECT *obj, void *data)
{
    *(RGB *)data = ((AWE_LABEL *)obj)->color[AWE_LABEL_DISABLED].font_sdw;
}


//sets the disabled font shadow
static void _label_set_font_shadow_disabled(AWE_OBJECT *obj, void *data)
{
    ((AWE_LABEL *)obj)->color[AWE_LABEL_DISABLED].font_sdw = *(RGB *)data;
    awe_set_widget_dirty((AWE_WIDGET *)obj);
}


//label properties
static AWE_CLASS_PROPERTY _label_properties[] = {
    { AWE_ID_TEXT, "const char *", sizeof(const char *), _label_get_text, _label_set_text, 0 },
    { AWE_ID_FONT, "FONT *", sizeof(FONT *), _label_get_font, _label_set_font, 0 },
    { AWE_ID_FONT_COLOR_ENABLED, "RGB", sizeof(RGB), _label_get_font_color_enabled, _label_set_font_color_enabled, 0 },
    { AWE_ID_FONT_COLOR_DISABLED, "RGB", sizeof(RGB), _label_get_font_color_disabled, _label_set_font_color_disabled, 0 },
    { AWE_ID_SHADOW_COLOR_ENABLED, "RGB", sizeof(RGB), _label_get_font_shadow_enabled, _label_set_font_shadow_enabled, 0 },
    { AWE_ID_SHADOW_COLOR_DISABLED, "RGB", sizeof(RGB), _label_get_font_shadow_disabled, _label_set_font_shadow_disabled, 0 },
    { 0 }
};


/*****************************************************************************
    PUBLIC
 *****************************************************************************/


//label vtable
AWE_LABEL_VTABLE awe_label_vtable = {
    //widget
    {
        //object
        {
            awe_label_get_interface,
            awe_widget_properties_changed,
            awe_label_clone
        },
        awe_label_paint,
        awe_widget_button_down,
        awe_widget_button_up,
        awe_widget_mouse_enter,
        awe_widget_mouse_move,
        awe_widget_mouse_leave,
        awe_widget_mouse_wheel,
        awe_widget_key_down,
        awe_widget_key_up,
        0,
        awe_widget_get_focus,
        awe_widget_lose_focus,
        awe_widget_begin_display,
        awe_widget_end_display,
        awe_widget_insert_widget,
        awe_widget_remove_widget,
        awe_geometry_manager_geometry_changed,
        awe_geometry_manager_z_order_changed,
        awe_geometry_manager_visible_changed,
        0
    }
    ,
    //geometry
    {
        0,
        awe_label_set_geometry,
        0
    }
};


//label class
AWE_CLASS awe_label_class = {
    AWE_ID_LABEL,
    AWE_ID_AWE,
    &awe_widget_class,
    sizeof(AWE_LABEL),
    _label_properties,
    0,
    &awe_label_vtable.widget.object,
    _label_constructor,
    _label_destructor
};


void *awe_label_get_interface(AWE_OBJECT *obj, const char *name, const char *pnamespace)
{
    if (!strcmp(pnamespace, AWE_ID_AWE)) {
        if (!strcmp(name, AWE_ID_LABEL)) {
            return (AWE_LABEL_VTABLE *)obj->pclass->vtable;
        }
        if (!strcmp(name, AWE_ID_GEOMETRY_MANAGER)) {
            return &((AWE_LABEL_VTABLE *)obj->pclass->vtable)->geometry_manager;
        }
    }
    return awe_widget_get_interface(obj, name, pnamespace);
}


//clone label
AWE_OBJECT *awe_label_clone(AWE_OBJECT *wgt)
{
    return awe_create_object(&awe_label_class,
        AWE_ID_TEXT                 , ((AWE_LABEL *)wgt)->text                              ,
        AWE_ID_FONT                 , ((AWE_LABEL *)wgt)->font                              ,
        AWE_ID_FONT_COLOR_ENABLED   , ((AWE_LABEL *)wgt)->color[AWE_LABEL_ENABLED].font_col ,
        AWE_ID_FONT_COLOR_DISABLED  , ((AWE_LABEL *)wgt)->color[AWE_LABEL_DISABLED].font_col,
        AWE_ID_SHADOW_COLOR_ENABLED , ((AWE_LABEL *)wgt)->color[AWE_LABEL_ENABLED].font_sdw ,
        AWE_ID_SHADOW_COLOR_DISABLED, ((AWE_LABEL *)wgt)->color[AWE_LABEL_DISABLED].font_sdw,
        0);
}


//label paint
void awe_label_paint(AWE_WIDGET *wgt, AWE_CANVAS *canvas, const AWE_RECT *clip)
{
    AWE_LABEL *lbl = (AWE_LABEL *)wgt;
    if(!awe_is_enabled_widget_tree(wgt)){
        if(AWE_MAKE_COLOR(lbl->color[AWE_LABEL_DISABLED].font_sdw) != makecol(255, 0, 255))
            awe_draw_gui_text(canvas, lbl->font, lbl->text, 1, 1, AWE_MAKE_COLOR(lbl->color[AWE_LABEL_DISABLED].font_sdw), -1);
        awe_draw_gui_text(canvas, lbl->font, lbl->text, 0, 0, AWE_MAKE_COLOR(lbl->color[AWE_LABEL_DISABLED].font_col), -1);
    }
    else{
        if(AWE_MAKE_COLOR(lbl->color[AWE_LABEL_ENABLED].font_sdw) != makecol(255, 0, 255))
            awe_draw_gui_text(canvas, lbl->font, lbl->text, 1, 1, AWE_MAKE_COLOR(lbl->color[AWE_LABEL_ENABLED].font_sdw), -1);
        awe_draw_gui_text(canvas, lbl->font, lbl->text, 0, 0, AWE_MAKE_COLOR(lbl->color[AWE_LABEL_ENABLED].font_col), -1);
    }
}


void awe_label_set_geometry(AWE_WIDGET *wgt)
{
    AWE_LABEL *lbl = (AWE_LABEL *)wgt;
    awe_override_widget_size(wgt, text_length(lbl->font, lbl->text), text_height(lbl->font));
}
