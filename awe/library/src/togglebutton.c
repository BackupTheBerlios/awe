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
            awe_push_button_clone
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
    0,
    _toggle_button_events,
    &awe_toggle_button_vtable.widget.object,
    0,
    0,
};


void awe_toggle_button_down(AWE_WIDGET *wgt, const AWE_EVENT *event)
{
    AWE_TOGGLE_BUTTON *btn = (AWE_TOGGLE_BUTTON *)wgt;
    if (!awe_set_focus_widget(wgt)) return;
    awe_enter_event_mode(awe_grab_event_proc, wgt);
    btn->btn.pressed = 1;
    awe_do_widget_event0(wgt, AWE_ID_TOGGLE_BUTTON_PRESSED);
    awe_add_widget_timer(wgt, 0, 100);
    awe_set_widget_dirty(wgt);
}


void awe_toggle_button_up(AWE_WIDGET *wgt, const AWE_EVENT *event)
{
    AWE_TOGGLE_BUTTON *btn = (AWE_TOGGLE_BUTTON *)wgt;
    awe_leave_event_mode();
    btn->btn.lostmouse = 0;
    if(awe_widget_has_mouse(wgt))
        btn->toggle ^= 1;
    btn->btn.pressed = btn->toggle;
    awe_do_widget_event0(wgt, AWE_ID_TOGGLE_BUTTON_RELEASED);
    awe_remove_widget_timer(wgt, 0);
    awe_set_widget_dirty(wgt);
}


void awe_toggle_button_mouse_enter(AWE_WIDGET *wgt, const AWE_EVENT *event)
{
    AWE_TOGGLE_BUTTON *btn = (AWE_TOGGLE_BUTTON *)wgt;
    if(btn->btn.lostmouse)
        btn->btn.pressed = 1;
    awe_set_widget_dirty(wgt);
}


void awe_toggle_button_mouse_leave(AWE_WIDGET *wgt, const AWE_EVENT *event)
{
    AWE_TOGGLE_BUTTON *btn = (AWE_TOGGLE_BUTTON *)wgt;
    if(btn->btn.pressed){
        btn->btn.lostmouse = 1;
        if(!btn->toggle)
            btn->btn.pressed = 0;
    }
    awe_set_widget_dirty(wgt);
}


void awe_toggle_button_timer(AWE_WIDGET *wgt, const AWE_EVENT *event)
{
    awe_do_widget_event0(wgt, AWE_ID_TOGGLE_BUTTON_HELD_DOWN);
}

