#include "radio.h"


/*****************************************************************************
    INTERNALS
 *****************************************************************************/


#define AWE_ID_RADIO_CHECK             "RadioCheck"


//checkbox events
static AWE_CLASS_EVENT _radio_events[] = {
    { AWE_ID_RADIO_CHECKED,   0 },
    { AWE_ID_RADIO_UNCHECKED, 0 },
    { AWE_ID_RADIO_HELD_DOWN, 0 },
    { 0 }
};


//gets the radio button group
static void _radio_get_group(AWE_OBJECT *obj, void *data)
{
    *(int *)data = ((AWE_RADIO *)obj)->group;
}


//sets the radio button group
static void _radio_set_group(AWE_OBJECT *obj, void *data)
{
    ((AWE_RADIO *)obj)->group = *(int *)data;
}


//radio properties
static AWE_CLASS_PROPERTY _radio_properties[] = {
    { AWE_ID_GROUP, "int", sizeof(int), _radio_get_group, _radio_set_group, 0 },
    { 0 }
};


static void _radio_check(AWE_WIDGET *wgt)
{
    AWE_TOGGLE_BUTTON *tmp = (AWE_TOGGLE_BUTTON *)wgt;
    if(tmp->toggle){
        tmp->toggle = 0;
        awe_set_widget_dirty(wgt);
    }
};


/*****************************************************************************
    PUBLIC
 *****************************************************************************/


//radio vtable
AWE_RADIO_VTABLE awe_radio_vtable = {
    //widget
    {
        //object
        {
            awe_radio_get_interface,
            awe_widget_properties_changed,
            awe_push_button_clone
        },
        awe_radio_paint,
        awe_radio_down,
        awe_radio_up,
        awe_toggle_button_mouse_enter,
        awe_widget_mouse_move,
        awe_toggle_button_mouse_leave,
        awe_widget_mouse_wheel,
        awe_widget_key_down,
        awe_widget_key_up,
        awe_radio_timer,
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
        awe_checkbox_set_geometry,
        0
    }
    ,
    //radio callback
    _radio_check
};


//radio class
AWE_CLASS awe_radio_class = {
    AWE_ID_RADIO,
    AWE_ID_AWE,
    &awe_checkbox_class,
    sizeof(AWE_RADIO),
    _radio_properties,
    _radio_events,
    &awe_radio_vtable.widget.object,
    0,
    0,
};


//clone radio
AWE_OBJECT *awe_radio_clone(AWE_OBJECT *wgt)
{
    return awe_create_object(&awe_radio_class, 
        AWE_ID_GROUP, ((AWE_RADIO *)wgt)->group,
        0);
}


void *awe_radio_get_interface(AWE_OBJECT *obj, const char *name, const char *pnamespace)
{
    if (!strcmp(pnamespace, AWE_ID_AWE)) {
        if (!strcmp(name, AWE_ID_PUSH_BUTTON)) {
            return (AWE_RADIO_VTABLE *)obj->pclass->vtable;
        }
        if (!strcmp(name, AWE_ID_CONTROL)) {
            return &((AWE_RADIO_VTABLE *)obj->pclass->vtable)->control;
        }
        if (!strcmp(name, AWE_ID_GEOMETRY_MANAGER)) {
            return &((AWE_RADIO_VTABLE *)obj->pclass->vtable)->geometry_manager;
        }
        if (!strcmp(name, AWE_ID_RADIO_CHECK)) {
            return ((AWE_RADIO_VTABLE *)obj->pclass->vtable)->radio_check;
        }
    }
    return awe_widget_get_interface(obj, name, pnamespace);
}


//radio paint
void awe_radio_paint(AWE_WIDGET *wgt, AWE_CANVAS *canvas, const AWE_RECT *clip)
{
    AWE_PUSH_BUTTON *btn = (AWE_PUSH_BUTTON *)wgt;
    int state = 0;
    int tx = ((AWE_CHECKBOX *)wgt)->text_dir ? wgt->height + 5 : wgt->width - wgt->height - text_length(btn->font, btn->text) - 5;
    int ty = (wgt->height - text_height(btn->font)) >> 1;
    int cx = ((AWE_CHECKBOX *)wgt)->text_dir ? 0 : wgt->width - wgt->height;

    solid_mode();
    
    if(!awe_is_enabled_widget_tree(wgt))
        state = AWE_PUSH_BUTTON_TEXTURE_DISABLED;
    else if(btn->pressed)
        state = AWE_PUSH_BUTTON_TEXTURE_PRESSED;
    else if(awe_widget_has_mouse(wgt))
        state = AWE_PUSH_BUTTON_TEXTURE_HIGHLIGHTED;
    else
        state = AWE_PUSH_BUTTON_TEXTURE_ENABLED;

    awe_fill_circle(canvas, cx + (wgt->height >> 1), wgt->height >> 1, (wgt->height >> 1) - 1, AWE_MAKE_COLOR(btn->texture[state].face_col[AWE_PUSH_BUTTON_FACE_TOP_LEFT]));

    awe_draw_3d_circle(canvas, cx + (wgt->height >> 1), wgt->height >> 1, wgt->height >> 1,
        AWE_MAKE_COLOR(btn->texture[state].edge_col[AWE_PUSH_BUTTON_EDGE_TOP_LEFT]),
        AWE_MAKE_COLOR(btn->texture[state].edge_col[AWE_PUSH_BUTTON_EDGE_BOTTOM_RIGHT]), 
        1);
    
    awe_fill_rect_s(canvas, ((AWE_CHECKBOX *)wgt)->text_dir ? wgt->height + 3 : 0,
        0, wgt->width - wgt->height - 3, wgt->height, AWE_MAKE_COLOR(((AWE_CHECKBOX *)wgt)->bg));

    if(AWE_MAKE_COLOR(btn->texture[state].font_sdw) != makecol(255, 0, 255))
        awe_draw_gui_text(canvas, btn->font, btn->text, tx + 1, ty + 1, AWE_MAKE_COLOR(btn->texture[state].font_sdw), -1);
    
    awe_draw_gui_text(canvas, btn->font, btn->text, tx, ty, AWE_MAKE_COLOR(btn->texture[state].font_col), -1);

    if(((AWE_TOGGLE_BUTTON *)wgt)->toggle)
        awe_fill_circle(canvas, cx + (wgt->height >> 1), wgt->height >> 1, (wgt->height >> 1) - 4, AWE_MAKE_COLOR(btn->texture[state].font_col));

    if (awe_get_focus_widget() == wgt && state != AWE_PUSH_BUTTON_TEXTURE_DISABLED)
        awe_draw_rect_pattern_s(canvas, ((AWE_CHECKBOX *)wgt)->text_dir ? wgt->height + 3 : 0,
            0, wgt->width - wgt->height - 3, wgt->height, AWE_MAKE_COLOR(btn->texture[state].font_col), AWE_PATTERN_DOT_DOT);
}


void awe_radio_down(AWE_WIDGET *wgt, const AWE_EVENT *event)
{
    if (!awe_set_focus_widget(wgt)) return;
    awe_enter_event_mode(awe_grab_event_proc, wgt);
    ((AWE_PUSH_BUTTON *)wgt)->pressed = 1;
    awe_add_widget_timer(wgt, 0, 100);
    awe_set_widget_dirty(wgt);
}


void awe_radio_up(AWE_WIDGET *wgt, const AWE_EVENT *event)
{
    AWE_PUSH_BUTTON *btn = (AWE_PUSH_BUTTON *)wgt;
    void (*radio_check)(AWE_WIDGET *wgt);
    awe_leave_event_mode();
    btn->lostmouse = 0;
    btn->pressed = 0;
    if(awe_widget_has_mouse(wgt)){
        AWE_WIDGET *tmp = awe_get_first_child_widget(awe_get_root_widget(wgt));
        while(tmp){
            int group = -1;
            awe_get_widget_properties(tmp, AWE_ID_GROUP, &group, NULL);
            if(group == ((AWE_RADIO *)wgt)->group && tmp != wgt){
                radio_check = awe_radio_get_interface((AWE_OBJECT *)tmp, AWE_ID_RADIO_CHECK, AWE_ID_AWE);
                if(radio_check)
                    radio_check(tmp);
            }
            tmp = awe_get_next_sibling_widget(tmp);
        }
        ((AWE_TOGGLE_BUTTON *)wgt)->toggle = 1;
        if(((AWE_TOGGLE_BUTTON *)wgt)->toggle){
            awe_do_widget_event0(wgt, AWE_ID_RADIO_CHECKED);
        }
        else{
            awe_do_widget_event0(wgt, AWE_ID_RADIO_UNCHECKED);
        }
    }
    awe_remove_widget_timer(wgt, 0);
    awe_set_widget_dirty(wgt);
}


void awe_radio_timer(AWE_WIDGET *wgt, const AWE_EVENT *event)
{
    awe_do_widget_event0(wgt, AWE_ID_RADIO_HELD_DOWN);
}
