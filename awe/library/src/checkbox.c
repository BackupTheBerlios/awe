#include "checkbox.h"


/*****************************************************************************
    INTERNALS
 *****************************************************************************/


static RGB _face_color_normal       = { 255, 255, 255, 0 };
static RGB _face_color_highlighted  = { 212, 208, 200, 0 };
static RGB _face_color_disabled     = { 232, 228, 220, 0 };
static RGB _font_color_normal       = { 0  , 0  , 0  , 0 };
static RGB _font_color_disabled     = { 128, 128, 128, 0 };
static RGB _shadow_color_normal     = { 255, 0  , 255, 0 };		//Magenta will not show
static RGB _shadow_color_disabled   = { 255, 255, 255, 0 };
static RGB _edge_color_top_left     = { 128, 128, 128, 0 };
static RGB _edge_color_bottom_right = { 241, 239, 226, 0 };


//checkbox events
static AWE_CLASS_EVENT _checkbox_events[] = {
    { AWE_ID_CHECKBOX_CHECKED,   0 },
    { AWE_ID_CHECKBOX_UNCHECKED, 0 },
    { AWE_ID_CHECKBOX_HELD_DOWN, 0 },
    { 0 }
};


//constructor
static void _checkbox_constructor(AWE_OBJECT *obj)
{
    int i, j;
    AWE_CHECKBOX *tmp = (AWE_CHECKBOX *)obj;
    tmp->btn.btn.text = ustrdup(empty_string);
    tmp->btn.btn.font = font;
    //memcpy(&tmp->btn.btn.margin, &_margin, sizeof(_margin));
    for(i = 0; i < AWE_PUSH_BUTTON_NUM_TEXTURES; i++){
        for(j = 0; j < AWE_PUSH_BUTTON_NUM_FACES; j++){
            if(i == AWE_PUSH_BUTTON_TEXTURE_HIGHLIGHTED)
                memcpy(&tmp->btn.btn.texture[i].face_col[j], &_face_color_highlighted, sizeof(RGB));
            else if(i == AWE_PUSH_BUTTON_TEXTURE_DISABLED)
                memcpy(&tmp->btn.btn.texture[i].face_col[j], &_face_color_disabled, sizeof(RGB));
            else
                memcpy(&tmp->btn.btn.texture[i].face_col[j], &_face_color_normal, sizeof(RGB));
        }
        for(j = 0; j < AWE_PUSH_BUTTON_NUM_EDGES; j++){
            if(j == AWE_PUSH_BUTTON_EDGE_TOP_LEFT)
                memcpy(&tmp->btn.btn.texture[i].edge_col[j], &_edge_color_top_left, sizeof(RGB));
            else
                memcpy(&tmp->btn.btn.texture[i].edge_col[j], &_edge_color_bottom_right, sizeof(RGB));
        }
        if(i == AWE_PUSH_BUTTON_TEXTURE_DISABLED){
            memcpy(&tmp->btn.btn.texture[i].font_col, &_font_color_disabled, sizeof(RGB));
            memcpy(&tmp->btn.btn.texture[i].font_sdw, &_shadow_color_disabled, sizeof(RGB));
        }
        else{
            memcpy(&tmp->btn.btn.texture[i].font_col, &_font_color_normal, sizeof(RGB));
            memcpy(&tmp->btn.btn.texture[i].font_sdw, &_shadow_color_normal, sizeof(RGB));
        }      
    }
    memcpy(&tmp->bg, &_face_color_highlighted, sizeof(RGB));
}


/*****************************************************************************
    PUBLIC
 *****************************************************************************/


//checkbox vtable
AWE_PUSH_BUTTON_VTABLE awe_checkbox_vtable = {
    //widget
    {
        //object
        {
            awe_push_button_get_interface,
            awe_widget_properties_changed,
            awe_push_button_clone
        },
        awe_checkbox_paint,
        awe_checkbox_down,
        awe_checkbox_up,
        awe_toggle_button_mouse_enter,
        awe_widget_mouse_move,
        awe_toggle_button_mouse_leave,
        awe_widget_mouse_wheel,
        awe_widget_key_down,
        awe_widget_key_up,
        awe_checkbox_timer,
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


//checkbox class
AWE_CLASS awe_checkbox_class = {
    AWE_ID_CHECKBOX,
    AWE_ID_AWE,
    &awe_toggle_button_class,
    sizeof(AWE_CHECKBOX),
    0,
    _checkbox_events,
    &awe_checkbox_vtable.widget.object,
    _checkbox_constructor,
    0,
};


//checkbox paint
void awe_checkbox_paint(AWE_WIDGET *wgt, AWE_CANVAS *canvas, const AWE_RECT *clip)
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

    awe_fill_gradient_s(canvas, 2, 2, wgt->height - 4, wgt->height - 4, 
            AWE_MAKE_COLOR(btn->btn.btn.texture[state].face_col[AWE_PUSH_BUTTON_FACE_TOP_LEFT]),
            AWE_MAKE_COLOR(btn->btn.btn.texture[state].face_col[AWE_PUSH_BUTTON_FACE_BOTTOM_LEFT]),
            AWE_MAKE_COLOR(btn->btn.btn.texture[state].face_col[AWE_PUSH_BUTTON_FACE_BOTTOM_RIGHT]),
            AWE_MAKE_COLOR(btn->btn.btn.texture[state].face_col[AWE_PUSH_BUTTON_FACE_TOP_RIGHT]));

    awe_draw_3d_rect_s(canvas, 0, 0, wgt->height, wgt->height, 
        AWE_MAKE_COLOR(btn->btn.btn.texture[state].edge_col[AWE_PUSH_BUTTON_EDGE_TOP_LEFT]),
        AWE_MAKE_COLOR(btn->btn.btn.texture[state].edge_col[AWE_PUSH_BUTTON_EDGE_BOTTOM_RIGHT]), 
        2);

    awe_fill_rect_s(canvas, wgt->height + 3, 0, wgt->width - wgt->height - 3, wgt->height, 
            AWE_MAKE_COLOR(btn->bg));

    if(AWE_MAKE_COLOR(btn->btn.btn.texture[state].font_sdw) != makecol(255, 0, 255))
        awe_draw_gui_text(canvas, btn->btn.btn.font, btn->btn.btn.text, tx + 1, ty + 1, AWE_MAKE_COLOR(btn->btn.btn.texture[state].font_sdw), -1);
    
    if(btn->btn.toggle)
        awe_draw_tick_icon(canvas, wgt->height >> 1, wgt->height >> 1, (wgt->height >> 1) - 3, AWE_MAKE_COLOR(btn->btn.btn.texture[state].font_col), 3);

    awe_draw_gui_text(canvas, btn->btn.btn.font, btn->btn.btn.text, tx, ty, AWE_MAKE_COLOR(btn->btn.btn.texture[state].font_col), -1);

    if (awe_get_focus_widget() == wgt && state != AWE_PUSH_BUTTON_TEXTURE_DISABLED)
        awe_draw_rect_pattern_s(canvas, wgt->height + 3, 0, wgt->width - wgt->height - 3, wgt->height, AWE_MAKE_COLOR(btn->btn.btn.texture[state].font_col), AWE_PATTERN_DOT_DOT);
}


void awe_checkbox_down(AWE_WIDGET *wgt, const AWE_EVENT *event)
{
    AWE_CHECKBOX *btn = (AWE_CHECKBOX *)wgt;
    if (!awe_set_focus_widget(wgt)) return;
    awe_enter_event_mode(awe_grab_event_proc, wgt);
    btn->btn.btn.pressed = 1;
    awe_add_widget_timer(wgt, 0, 100);
    awe_set_widget_dirty(wgt);
}


void awe_checkbox_up(AWE_WIDGET *wgt, const AWE_EVENT *event)
{
    AWE_CHECKBOX *btn = (AWE_CHECKBOX *)wgt;
    awe_leave_event_mode();
    btn->btn.btn.lostmouse = 0;
    btn->btn.btn.pressed = 0;
    if(awe_widget_has_mouse(wgt)){
        btn->btn.toggle ^= 1;
        if(btn->btn.toggle){
            awe_do_widget_event0(wgt, AWE_ID_CHECKBOX_CHECKED);
        }
        else{
            awe_do_widget_event0(wgt, AWE_ID_CHECKBOX_UNCHECKED);
        }
    }
    awe_remove_widget_timer(wgt, 0);
    awe_set_widget_dirty(wgt);
}


void awe_checkbox_timer(AWE_WIDGET *wgt, const AWE_EVENT *event)
{
    awe_do_widget_event0(wgt, AWE_ID_CHECKBOX_HELD_DOWN);
}


void awe_checkbox_set_geometry(AWE_WIDGET *wgt)
{
    AWE_CHECKBOX *btn = (AWE_CHECKBOX *)wgt;
    int height = MAX(13, text_height(btn->btn.btn.font));
    int width = text_length(btn->btn.btn.font, btn->btn.btn.text) + height + 5;
    awe_override_widget_size(wgt, width, height);
}
