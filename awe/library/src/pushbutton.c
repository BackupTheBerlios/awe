#include "pushbutton.h"


/*****************************************************************************
    INTERNALS
 *****************************************************************************/

char *_push_button_texture[AWE_PUSH_BUTTON_NUM_TEXTURES] = {
    "Enabled",
    "Pressed",
    "Disabled",
    "Highlighted",
    "Focused"
};


//constructor
static void _push_button_constructor(AWE_OBJECT *obj)
{
    AWE_PUSH_BUTTON *tmp = (AWE_PUSH_BUTTON *)obj;
    tmp->text = ustrdup(empty_string);
    tmp->state = AWE_PUSH_BUTTON_ENABLED;
}


//destructor
static void _push_button_destructor(AWE_OBJECT *obj)
{
    AWE_PUSH_BUTTON *tmp = (AWE_PUSH_BUTTON *)obj;
    free(tmp->text);
}


//gets the text
static void _push_button_get_text(AWE_OBJECT *obj, void *data)
{
    AWE_PUSH_BUTTON *tmp = (AWE_PUSH_BUTTON *)obj;
    *(const char **)data = tmp->text;
}


//sets the text
static void _push_button_set_text(AWE_OBJECT *obj, void *data)
{
    const char *new_text = *(const char **)data;
    AWE_PUSH_BUTTON *tmp = (AWE_PUSH_BUTTON *)obj;
    free(tmp->text);
    tmp->text = ustrdup(new_text ? new_text : empty_string);
    awe_set_widget_dirty(&tmp->widget);
}


//gets the font
static void _push_button_get_font(AWE_OBJECT *obj, void *data)
{
    AWE_PUSH_BUTTON *tmp = (AWE_PUSH_BUTTON *)obj;
    *(FONT **)data = tmp->font;
}


//sets the font
static void _push_button_set_font(AWE_OBJECT *obj, void *data)
{
    AWE_PUSH_BUTTON *tmp = (AWE_PUSH_BUTTON *)obj;
    tmp->font = *(FONT **)data;
    awe_set_widget_dirty(&tmp->widget);
}


//push_button properties
AWE_CLASS_PROPERTY _push_button_properties[] = {
    { AWE_ID_TEXT, "const char *", sizeof(const char *), _push_button_get_text, _push_button_set_text, 0 },
    { AWE_ID_FONT, "FONT *", sizeof(FONT *), _push_button_get_font, _push_button_set_font, 0 },
    { 0 }
};


AWE_CLASS_EVENT _push_button_events[] = {
    { AWE_ID_PUSH_BUTTON_ACTIVATED, 0 },
    { AWE_ID_PUSH_BUTTON_RELEASED,  0 },
    { AWE_ID_PUSH_BUTTON_HELD_DOWN, 0 },
    { AWE_ID_PUSH_BUTTON_PRESSED,   0 },
    { 0 }
};


/*****************************************************************************
    PUBLIC
 *****************************************************************************/


//push_button vtable
AWE_PUSH_BUTTON_VTABLE awe_push_button_vtable = {
    //widget
    {
        //object
        {
            awe_push_button_get_interface,
            awe_widget_properties_changed,
            awe_push_button_clone
        },
        awe_push_button_paint,
        awe_push_button_down,
        awe_push_button_up,
        awe_push_button_mouse_enter,
        awe_widget_mouse_move,
        awe_push_button_mouse_leave,
        awe_widget_mouse_wheel,
        awe_widget_key_down,
        awe_widget_key_up,
        awe_push_button_timer,
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
    //control
    {
        awe_push_button_key_press
    }
    ,
    //geometry
    {
        0,
        awe_push_button_set_geometry,
        0
    }
};


//push_button class
AWE_CLASS awe_push_button_class = {
    AWE_ID_PUSH_BUTTON,
    AWE_ID_AWE,
    &awe_widget_class,
    sizeof(AWE_PUSH_BUTTON),
    _push_button_properties,
    _push_button_events,
    &awe_push_button_vtable.widget.object,
    _push_button_constructor,
    _push_button_destructor
};


void *awe_push_button_get_interface(AWE_OBJECT *obj, const char *name, const char *pnamespace)
{
    if (!strcmp(pnamespace, AWE_ID_AWE)) {
        if (!strcmp(name, AWE_ID_PUSH_BUTTON)) {
            return (AWE_PUSH_BUTTON_VTABLE *)obj->pclass->vtable;
        }
        if (!strcmp(name, AWE_ID_CONTROL)) {
            return &((AWE_PUSH_BUTTON_VTABLE *)obj->pclass->vtable)->control;
        }
        if (!strcmp(name, AWE_ID_GEOMETRY_MANAGER)) {
            return &((AWE_PUSH_BUTTON_VTABLE *)obj->pclass->vtable)->geometry_manager;
        }
    }
    return awe_widget_get_interface(obj, name, pnamespace);
}


//clone widget
AWE_OBJECT *awe_push_button_clone(AWE_OBJECT *wgt)
{
    return awe_create_object(&awe_push_button_class, 
        AWE_ID_X           , ((AWE_WIDGET *)wgt)->x           ,
        AWE_ID_Y           , ((AWE_WIDGET *)wgt)->y           ,
        AWE_ID_WIDTH       , ((AWE_WIDGET *)wgt)->width       ,
        AWE_ID_HEIGHT      , ((AWE_WIDGET *)wgt)->height      ,
        AWE_ID_VISIBLE     , ((AWE_WIDGET *)wgt)->visible     ,
        AWE_ID_ENABLED     , ((AWE_WIDGET *)wgt)->enabled     ,
        AWE_ID_OPAQUE      , ((AWE_WIDGET *)wgt)->opaque      , 
        AWE_ID_TRANSLUCENCY, ((AWE_WIDGET *)wgt)->translucency,
        AWE_ID_OUTPUT_TYPE , ((AWE_WIDGET *)wgt)->output_type ,
        AWE_ID_TEXT        , ((AWE_PUSH_BUTTON *)wgt)->text   ,
        AWE_ID_FONT        , ((AWE_PUSH_BUTTON *)wgt)->font   ,
        0);
}


//push_button paint
void awe_push_button_paint(AWE_WIDGET *wgt, AWE_CANVAS *canvas, const AWE_RECT *clip)
{
    AWE_PUSH_BUTTON *btn = (AWE_PUSH_BUTTON *)wgt;
    int tx = (wgt->width - gui_strlen(btn->text)) >> 1;
    int ty = (wgt->height - text_height(font)) >> 1;
    solid_mode();
    if(awe_widget_has_mouse(wgt))
        awe_fill_rect_s(canvas, 3, 3, wgt->width - 6, wgt->height - 6, makecol(228, 224, 216));
    else
        awe_fill_rect_s(canvas, 3, 3, wgt->width - 6, wgt->height - 6, makecol(212, 208, 200));
    if(btn->state & AWE_PUSH_BUTTON_PRESSED){
        awe_draw_3d_rect_s(canvas, 0, 0, wgt->width, wgt->height, makecol(128, 128, 128), makecol(241, 239, 226), 3);
        tx += 1;
        ty += 1;
    }
    else
        awe_draw_3d_rect_s(canvas, 0, 0, wgt->width, wgt->height, makecol(241, 239, 226), makecol(128, 128, 128), 3);
    awe_draw_gui_text(canvas, font, btn->text, tx, ty, makecol(0, 0, 0), -1);
    if (awe_get_focus_widget() == wgt) {
        awe_draw_rect_pattern_s(canvas, 4, 4, wgt->width - 9, wgt->height - 9, makecol(0, 0, 0), AWE_PATTERN_DOT_DOT);
    }
}


void awe_push_button_down(AWE_WIDGET *wgt, const AWE_EVENT *event)
{
    AWE_PUSH_BUTTON *btn = (AWE_PUSH_BUTTON *)wgt;
    if (!awe_set_focus_widget(wgt)) return;
    awe_enter_event_mode(awe_grab_event_proc, wgt);
    btn->state |= AWE_PUSH_BUTTON_PRESSED;
    awe_do_widget_event0(wgt, AWE_ID_PUSH_BUTTON_PRESSED);
    awe_add_widget_timer(wgt, 0, 100);
    awe_set_widget_dirty(wgt);
}


void awe_push_button_up(AWE_WIDGET *wgt, const AWE_EVENT *event)
{
    AWE_PUSH_BUTTON *btn = (AWE_PUSH_BUTTON *)wgt;
    awe_leave_event_mode();
    btn->state &= ~AWE_PUSH_BUTTON_PRESSED;
    if(awe_widget_has_mouse(wgt)){
        awe_do_widget_event0(wgt, AWE_ID_PUSH_BUTTON_ACTIVATED);
    }
    else{
        awe_do_widget_event0(wgt, AWE_ID_PUSH_BUTTON_RELEASED);
    }
    awe_remove_widget_timer(wgt, 0);
    awe_set_widget_dirty(wgt);
}


void awe_push_button_mouse_enter(AWE_WIDGET *wgt, const AWE_EVENT *event)
{
    awe_set_widget_dirty(wgt);
}


void awe_push_button_mouse_leave(AWE_WIDGET *wgt, const AWE_EVENT *event)
{
    awe_set_widget_dirty(wgt);
}


int awe_push_button_key_press(AWE_WIDGET *wgt, const AWE_EVENT *event)
{
    AWE_PUSH_BUTTON *btn = (AWE_PUSH_BUTTON *)wgt;
    const char *t = ustrchr(btn->text, '&');

    if (!t) return 0;
    t += ucwidth('&');
    if (utolower(ugetc(t)) != utolower(event->key.key)) return 0;
    if (!awe_set_focus_widget(wgt)) return 0;
    return 1;
}


void awe_push_button_timer(AWE_WIDGET *wgt, const AWE_EVENT *event)
{
    awe_do_widget_event0(wgt, AWE_ID_PUSH_BUTTON_HELD_DOWN);
}


void awe_push_button_set_geometry(AWE_WIDGET *wgt)
{
    AWE_PUSH_BUTTON *btn = (AWE_PUSH_BUTTON *)wgt;
    awe_override_widget_size(
        wgt, 
        text_length(btn->font,
        btn->text) + btn->margin.left + btn->margin.right, 
        text_height(btn->font) + btn->margin.top + btn->margin.bottom
    );
}

