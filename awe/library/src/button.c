#include "button.h"


/*****************************************************************************
    INTERNALS
 *****************************************************************************/


/*****************************************************************************
    PUBLIC
 *****************************************************************************/


//button properties
AWE_CLASS_PROPERTY awe_button_properties[] = {
    {AWE_ID_TEXT, "const char *", sizeof(const char *), awe_button_get_text, awe_button_set_text, 0},
    {0}
};


//button vtable
AWE_BUTTON_VTABLE awe_button_vtable = {
    //widget
    {
        //object
        {
            awe_button_get_interface,
            awe_widget_properties_changed,
            awe_widget_clone
        },
        awe_button_paint,
        awe_button_down,
        awe_button_up,
        awe_widget_mouse_enter,
        awe_widget_mouse_move,
        awe_widget_mouse_leave,
        awe_widget_mouse_wheel,
        awe_widget_key_down,
        awe_widget_key_up,
        0,
        awe_widget_get_focus,
        awe_widget_loose_focus,
        awe_widget_begin_display,
        awe_widget_end_display,
        awe_widget_insert_widget,
        awe_widget_remove_widget,
        0,
        0,
        0,
        0
    }
    ,
    //control
    {
        awe_button_key_press
    }
};


//button class
AWE_CLASS awe_button_class = {
    "Button",
    AWE_ID_AWE,
    &awe_widget_class,
    sizeof(AWE_BUTTON),
    awe_button_properties,
    0,
    &awe_button_vtable.widget.object,
    0,
    0
};


//constructor
void awe_button_constructor(AWE_OBJECT *obj)
{
    AWE_BUTTON *tmp = (AWE_BUTTON *)obj;
    tmp->text = ustrdup(empty_string);
    tmp->state = 0;
}


//destructor
void awe_button_destructor(AWE_OBJECT *obj)
{
    AWE_BUTTON *tmp = (AWE_BUTTON *)obj;
    free(tmp->text);
}


//gets the text
void awe_button_get_text(AWE_OBJECT *obj, void *data)
{
    AWE_BUTTON *tmp = (AWE_BUTTON *)obj;
    *(const char**)data = tmp->text;
}


//sets the text
void awe_button_set_text(AWE_OBJECT *obj, void *data)
{
    const char *new_text = *(const char **)data;
    AWE_BUTTON *tmp = (AWE_BUTTON *)obj;
    free(tmp->text);
    tmp->text = ustrdup(new_text ? new_text : empty_string);
    awe_set_widget_dirty(&tmp->widget);
}


void *awe_button_get_interface(AWE_OBJECT *obj, const char *name, const char *pnamespace)
{
    if (!strcmp(pnamespace, AWE_ID_AWE)) {
        if (!strcmp(name, "Button")) {
            return (AWE_BUTTON_VTABLE *)obj->pclass->vtable;
        }
        if (!strcmp(name, AWE_ID_CONTROL)) {
            return &((AWE_BUTTON_VTABLE *)obj->pclass->vtable)->control;
        }
    }
    return awe_widget_get_interface(obj, name, pnamespace);
}


//button paint
void awe_button_paint(AWE_WIDGET *wgt, AWE_CANVAS *canvas, const AWE_RECT *clip)
{
    AWE_BUTTON *btn = (AWE_BUTTON *)wgt;
    int tx = (wgt->width - gui_strlen(btn->text)) >> 1;
    int ty = (wgt->height - text_height(font)) >> 1;

    solid_mode();
    awe_fill_rect_s(canvas, 3, 3, wgt->width - 3, wgt->height - 3, makecol(212, 208, 200));
    if(btn->state == 0)
        awe_draw_3d_rect_s(canvas, 0, 0, wgt->width, wgt->height, makecol(241, 239, 226), makecol(128, 128, 128), 3);
    if(btn->state == 1)
        awe_draw_3d_rect_s(canvas, 0, 0, wgt->width, wgt->height, makecol(128, 128, 128), makecol(241, 239, 226), 3);
    awe_draw_gui_text(canvas, font, btn->text, tx, ty, makecol(0, 0, 0), -1);
    if (awe_get_focus_widget() == wgt) {
        awe_draw_rect_pattern_s(canvas, 4, 4, wgt->width - 9, wgt->height - 9, makecol(0, 0, 0), AWE_PATTERN_DOT_DOT);
    }
}


void awe_button_down(AWE_WIDGET *wgt, const AWE_EVENT *event)
{
    AWE_BUTTON *btn = (AWE_BUTTON *)wgt;
    if (!awe_set_focus_widget(wgt)) return;
    awe_enter_event_mode(awe_grab_event_proc, wgt);
    btn->state = 1;
}


void awe_button_up(AWE_WIDGET *wgt, const AWE_EVENT *event)
{
    AWE_BUTTON *btn = (AWE_BUTTON *)wgt;
    awe_leave_event_mode();
    btn->state = 0; 
}


int awe_button_key_press(AWE_WIDGET *wgt, const AWE_EVENT *event)
{
    AWE_BUTTON *btn = (AWE_BUTTON *)wgt;
    const char *t = ustrchr(btn->text, '&');

    if (!t) return 0;
    t += ucwidth('&');
    if (utolower(ugetc(t)) != utolower(event->key.key)) return 0;
    if (!awe_set_focus_widget(wgt)) return 0;
    return 1;
}
