#include "togglebutton.h"


/*****************************************************************************
    INTERNALS
 *****************************************************************************/


//push button events
static AWE_CLASS_EVENT _toggle_button_events[] = {
    { AWE_ID_TOGGLE_BUTTON_RELEASED,  0 },
    { AWE_ID_TOGGLE_BUTTON_HELD_DOWN, 0 },
    { AWE_ID_TOGGLE_BUTTON_PRESSED,   0 },
    { 0 }
};


//gets the toggled state
static void _toggle_button_get_toggled(AWE_OBJECT *obj, void *data)
{
    *(int *)data = ((AWE_TOGGLE_BUTTON *)obj)->toggle;
}


//sets the toggled state
static void _toggle_button_set_toggled(AWE_OBJECT *obj, void *data)
{
    ((AWE_TOGGLE_BUTTON *)obj)->toggle = *(int *)data;
    //pushbutton doesn't know about toggle, set pressed to data as well
    ((AWE_PUSH_BUTTON *)obj)->pressed = *(int *)data;
    awe_set_widget_dirty((AWE_WIDGET *)obj);
}


//toggle button properties
static AWE_CLASS_PROPERTY _toggle_button_properties[] = {
    { AWE_ID_TOGGLED, "int", sizeof(int), _toggle_button_get_toggled, _toggle_button_set_toggled, 0 },
    { 0 }
};


/*****************************************************************************
    PUBLIC
 *****************************************************************************/


//push_button vtable
AWE_PUSH_BUTTON_VTABLE awe_toggle_button_vtable = {
    //widget
    {
        //object
        {
            awe_push_button_get_interface,
            awe_widget_properties_changed,
            awe_toggle_button_clone
        },
        awe_push_button_paint,
        awe_toggle_button_down,
        awe_toggle_button_up,
        awe_toggle_button_mouse_enter,
        awe_widget_mouse_move,
        awe_toggle_button_mouse_leave,
        awe_widget_mouse_wheel,
        awe_widget_key_down,
        awe_widget_key_up,
        awe_toggle_button_timer,
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


//toggle_button class
AWE_CLASS awe_toggle_button_class = {
    AWE_ID_TOGGLE_BUTTON,
    AWE_ID_AWE,
    &awe_push_button_class,
    sizeof(AWE_TOGGLE_BUTTON),
    _toggle_button_properties,
    _toggle_button_events,
    &awe_toggle_button_vtable.widget.object,
    0,
    0,
};


//clone toggle_button
AWE_OBJECT *awe_toggle_button_clone(AWE_OBJECT *wgt)
{
    return awe_create_object(&awe_toggle_button_class, 
        AWE_ID_TOGGLED, ((AWE_TOGGLE_BUTTON *)wgt)->toggle,
        0);
}


void awe_toggle_button_down(AWE_WIDGET *wgt, const AWE_EVENT *event)
{
    if (!awe_set_focus_widget(wgt)) return;
    awe_enter_event_mode(awe_grab_event_proc, wgt);
    ((AWE_PUSH_BUTTON *)wgt)->pressed = 1;
    awe_do_widget_event0(wgt, AWE_ID_TOGGLE_BUTTON_PRESSED);
    awe_add_widget_timer(wgt, 0, 100);
    awe_set_widget_dirty(wgt);
}


void awe_toggle_button_up(AWE_WIDGET *wgt, const AWE_EVENT *event)
{
    awe_leave_event_mode();
    ((AWE_PUSH_BUTTON *)wgt)->lostmouse = 0;
    if(awe_widget_has_mouse(wgt))
        ((AWE_TOGGLE_BUTTON *)wgt)->toggle ^= 1;
    ((AWE_PUSH_BUTTON *)wgt)->pressed = ((AWE_TOGGLE_BUTTON *)wgt)->toggle;
    awe_do_widget_event0(wgt, AWE_ID_TOGGLE_BUTTON_RELEASED);
    awe_remove_widget_timer(wgt, 0);
    awe_set_widget_dirty(wgt);
}


void awe_toggle_button_mouse_enter(AWE_WIDGET *wgt, const AWE_EVENT *event)
{
    if(((AWE_PUSH_BUTTON *)wgt)->lostmouse)
        ((AWE_PUSH_BUTTON *)wgt)->pressed = 1;
    awe_set_widget_dirty(wgt);
}


void awe_toggle_button_mouse_leave(AWE_WIDGET *wgt, const AWE_EVENT *event)
{
    if(((AWE_PUSH_BUTTON *)wgt)->pressed){
        ((AWE_PUSH_BUTTON *)wgt)->lostmouse = 1;
        if(!((AWE_TOGGLE_BUTTON *)wgt)->toggle)
            ((AWE_PUSH_BUTTON *)wgt)->pressed = 0;
    }
    awe_set_widget_dirty(wgt);
}


void awe_toggle_button_timer(AWE_WIDGET *wgt, const AWE_EVENT *event)
{
    awe_do_widget_event0(wgt, AWE_ID_TOGGLE_BUTTON_HELD_DOWN);
}

