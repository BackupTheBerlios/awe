#include "radio.h"


/*****************************************************************************
    INTERNALS
 *****************************************************************************/


//checkbox events
static AWE_CLASS_EVENT _radio_events[] = {
    { AWE_ID_RADIO_CHECKED,   0 },
    { AWE_ID_RADIO_UNCHECKED, 0 },
    { AWE_ID_RADIO_HELD_DOWN, 0 },
    { 0 }
};


/*****************************************************************************
    PUBLIC
 *****************************************************************************/


//radio vtable
AWE_PUSH_BUTTON_VTABLE awe_radio_vtable = {
    //widget
    {
        //object
        {
            awe_push_button_get_interface,
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
};


//radio class
AWE_CLASS awe_radio_class = {
    AWE_ID_RADIO,
    AWE_ID_AWE,
    &awe_checkbox_class,
    sizeof(AWE_RADIO),
    0,
    _radio_events,
    &awe_radio_vtable.widget.object,
    0,
    0,
};


//radio paint
void awe_radio_paint(AWE_WIDGET *wgt, AWE_CANVAS *canvas, const AWE_RECT *clip)
{
    AWE_CHECKBOX *btn = (AWE_CHECKBOX *)wgt;
    int state = 0;
    int tx = wgt->height + 5;
    int ty = (wgt->height - text_height(btn->btn.btn.font)) >> 1;
    
    solid_mode();
    
    if(!awe_is_enabled_widget_tree(wgt))
        state = AWE_PUSH_BUTTON_TEXTURE_DISABLED;
    else if(btn->btn.btn.pressed)
        state = AWE_PUSH_BUTTON_TEXTURE_PRESSED;
    else if(awe_widget_has_mouse(wgt))
        state = AWE_PUSH_BUTTON_TEXTURE_HIGHLIGHTED;
    else
        state = AWE_PUSH_BUTTON_TEXTURE_ENABLED;

    awe_fill_circle(canvas, wgt->height >> 1, wgt->height >> 1, (wgt->height >> 1) - 1, AWE_MAKE_COLOR(btn->btn.btn.texture[state].face_col[AWE_PUSH_BUTTON_FACE_TOP_LEFT]));

    awe_draw_3d_circle(canvas, wgt->height >> 1, wgt->height >> 1, wgt->height >> 1,
          AWE_MAKE_COLOR(btn->btn.btn.texture[state].edge_col[AWE_PUSH_BUTTON_EDGE_TOP_LEFT]),
          AWE_MAKE_COLOR(btn->btn.btn.texture[state].edge_col[AWE_PUSH_BUTTON_EDGE_BOTTOM_RIGHT]), 
          1);
    
    awe_fill_rect_s(canvas, wgt->height + 3, 0, wgt->width - wgt->height - 3, wgt->height, 
            AWE_MAKE_COLOR(btn->bg));

    if(AWE_MAKE_COLOR(btn->btn.btn.texture[state].font_sdw) != makecol(255, 0, 255))
        awe_draw_gui_text(canvas, btn->btn.btn.font, btn->btn.btn.text, tx + 1, ty + 1, AWE_MAKE_COLOR(btn->btn.btn.texture[state].font_sdw), -1);
    
    if(btn->btn.toggle)
        awe_fill_circle(canvas, wgt->height >> 1, wgt->height >> 1, (wgt->height >> 1) - 4, AWE_MAKE_COLOR(btn->btn.btn.texture[state].font_col));

    awe_draw_gui_text(canvas, btn->btn.btn.font, btn->btn.btn.text, tx, ty, AWE_MAKE_COLOR(btn->btn.btn.texture[state].font_col), -1);

    if (awe_get_focus_widget() == wgt && state != AWE_PUSH_BUTTON_TEXTURE_DISABLED)
        awe_draw_rect_pattern_s(canvas, wgt->height + 3, 0, wgt->width - wgt->height - 3, wgt->height, AWE_MAKE_COLOR(btn->btn.btn.texture[state].font_col), AWE_PATTERN_DOT_DOT);
}


void awe_radio_down(AWE_WIDGET *wgt, const AWE_EVENT *event)
{
    AWE_CHECKBOX *btn = (AWE_CHECKBOX *)wgt;
    if (!awe_set_focus_widget(wgt)) return;
    awe_enter_event_mode(awe_grab_event_proc, wgt);
    btn->btn.btn.pressed = 1;
    awe_add_widget_timer(wgt, 0, 100);
    awe_set_widget_dirty(wgt);
}


void awe_radio_up(AWE_WIDGET *wgt, const AWE_EVENT *event)
{
    AWE_CHECKBOX *btn = (AWE_CHECKBOX *)wgt;
    awe_leave_event_mode();
    btn->btn.btn.lostmouse = 0;
    btn->btn.btn.pressed = 0;
    if(awe_widget_has_mouse(wgt)){
        btn->btn.toggle ^= 1;
        if(btn->btn.toggle){
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
