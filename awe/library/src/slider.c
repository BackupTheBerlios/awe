#include "slider.h"


/*****************************************************************************
    INTERNALS
 *****************************************************************************/


static RGB _face_color_normal       = { 212, 208, 200, 0 };
static RGB _face_color_highlighted  = { 228, 224, 216, 0 };
static RGB _edge_color_top_left     = { 241, 239, 226, 0 };
static RGB _edge_color_bottom_right = { 128, 128, 128, 0 };
static RGB _bar_color_top           = { 128, 128, 128, 0 };
static RGB _bar_color_bottom        = { 241, 239, 226, 0 };


#define DEFAULT_HANDLE_WIDTH        9
#define DEFAULT_SLIDER_MAX          100


//slider moved argument
static AWE_CLASS_EVENT_ARGUMENT _pos[] = {
    { AWE_ID_POSITION, "int" },
    { 0 }
};


//slider events
static AWE_CLASS_EVENT _slider_events[] = {
    { AWE_ID_SLIDER_MOVED, _pos },
    { 0 }
};


//constructor
static void _slider_constructor(AWE_OBJECT *obj)
{
    AWE_SLIDER *tmp = (AWE_SLIDER *)obj;
    int i;
    tmp->handle_width = DEFAULT_HANDLE_WIDTH;
    tmp->max_val = DEFAULT_SLIDER_MAX;
    memcpy(&tmp->bar_col[0], &_bar_color_top, sizeof(RGB));
    memcpy(&tmp->bar_col[1], &_bar_color_bottom, sizeof(RGB));
    for(i = 0; i < 4; i++){
        if(i == AWE_SLIDER_TEXTURE_HIGHLIGHTED)
            memcpy(&tmp->face_col[i], &_face_color_highlighted, sizeof(RGB));
        else
            memcpy(&tmp->face_col[i], &_face_color_normal, sizeof(RGB));
        memcpy(&tmp->edge_col[i][AWE_SLIDER_EDGE_TOP_LEFT], &_edge_color_top_left, sizeof(RGB));
        memcpy(&tmp->edge_col[i][AWE_SLIDER_EDGE_BOTTOM_RIGHT], &_edge_color_bottom_right, sizeof(RGB));
    }
}


//destructor
static void _slider_destructor(AWE_OBJECT *obj)
{
    
}


//gets the slider orientation
static void _slider_get_orientation(AWE_OBJECT *obj, void *data)
{
    *(AWE_SLIDER_ORIENTATION *)data = ((AWE_SLIDER *)obj)->orientation;
}


//sets the slider orientation
static void _slider_set_orientation(AWE_OBJECT *obj, void *data)
{
    ((AWE_SLIDER *)obj)->orientation = *(AWE_SLIDER_ORIENTATION *)data;
    awe_set_widget_dirty((AWE_WIDGET *)obj);
}


//gets the slider position
static void _slider_get_position(AWE_OBJECT *obj, void *data)
{
    *(int *)data = ((AWE_SLIDER *)obj)->pos;
}


//sets the slider position
static void _slider_set_position(AWE_OBJECT *obj, void *data)
{
    ((AWE_SLIDER *)obj)->pos = MID(0, *(int *)data, ((AWE_SLIDER *)obj)->max_val);
    awe_set_widget_dirty((AWE_WIDGET *)obj);
}


//gets the slider max
static void _slider_get_max(AWE_OBJECT *obj, void *data)
{
    *(int *)data = ((AWE_SLIDER *)obj)->max_val;
}


//sets the slider max
static void _slider_set_max(AWE_OBJECT *obj, void *data)
{
    ((AWE_SLIDER *)obj)->max_val = MAX(0, *(int *)data);
    awe_set_widget_dirty((AWE_WIDGET *)obj);
}


//gets the handle width
static void _slider_get_width(AWE_OBJECT *obj, void *data)
{
    *(int *)data = ((AWE_SLIDER *)obj)->handle_width;
}


//sets the handle width
static void _slider_set_width(AWE_OBJECT *obj, void *data)
{
    ((AWE_SLIDER *)obj)->handle_width = MAX(0, *(int *)data);
    awe_set_widget_dirty((AWE_WIDGET *)obj);
}


//gets the handle type
static void _slider_get_type(AWE_OBJECT *obj, void *data)
{
    *(AWE_SLIDER_HANDLE_TYPE *)data = ((AWE_SLIDER *)obj)->handle_type;
}


//sets the handle type
static void _slider_set_type(AWE_OBJECT *obj, void *data)
{
    ((AWE_SLIDER *)obj)->handle_type = *(AWE_SLIDER_HANDLE_TYPE *)data;
    awe_set_widget_dirty((AWE_WIDGET *)obj);
}


//gets the slider step
static void _slider_get_step(AWE_OBJECT *obj, void *data)
{
    *(int *)data = ((AWE_SLIDER *)obj)->step;
}


//sets the slider step
static void _slider_set_step(AWE_OBJECT *obj, void *data)
{
    ((AWE_SLIDER *)obj)->step = *(int *)data;
}


//slider properties
static AWE_CLASS_PROPERTY _slider_properties[] = {
    { AWE_ID_ORIENTATION, "AWE_SLIDER_ORIENTATION", sizeof(AWE_SLIDER_ORIENTATION), _slider_get_orientation, _slider_set_orientation, 0 },
    { AWE_ID_POSITION, "int", sizeof(int), _slider_get_position, _slider_set_position, 0 },
    { AWE_ID_MAX, "int", sizeof(int), _slider_get_max, _slider_set_max, 0 },
    { AWE_ID_HANDLE_WIDTH, "int", sizeof(int), _slider_get_width, _slider_set_width, 0 },
    { AWE_ID_HANDLE_TYPE,  "AWE_SLIDER_HANDLE_TYPE", sizeof(AWE_SLIDER_HANDLE_TYPE), _slider_get_type, _slider_set_type, 0 },
    { AWE_ID_STEP,  "int", sizeof(int), _slider_get_step, _slider_set_step, 0 },
    { 0 }
};


/*****************************************************************************
    PUBLIC
 *****************************************************************************/


//slider
AWE_SLIDER_VTABLE awe_slider_vtable = {
    //widget
    {
        //object
        {
            awe_slider_get_interface,
            awe_widget_properties_changed,
            awe_slider_clone
        },
        awe_slider_paint,
        awe_slider_button_down,
        awe_slider_button_up,
        awe_slider_mouse_enter,
        awe_slider_mouse_move,
        awe_slider_mouse_leave,
        awe_slider_mouse_wheel,
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
    //control
    {
        0
    }
    ,
    //geometry
    {
        0,
        0,
        0
    }
};


//slider class
AWE_CLASS awe_slider_class = {
    AWE_ID_SLIDER,
    AWE_ID_AWE,
    &awe_widget_class,
    sizeof(AWE_SLIDER),
    _slider_properties,
    _slider_events,
    &awe_slider_vtable.widget.object,
    _slider_constructor,
    _slider_destructor
};


void *awe_slider_get_interface(AWE_OBJECT *obj, const char *name, const char *pnamespace)
{
    if (!strcmp(pnamespace, AWE_ID_AWE)) {
        if (!strcmp(name, AWE_ID_SLIDER)) {
            return (AWE_SLIDER_VTABLE *)obj->pclass->vtable;
        }
        if (!strcmp(name, AWE_ID_CONTROL)) {
            return &((AWE_SLIDER_VTABLE *)obj->pclass->vtable)->control;
        }
        if (!strcmp(name, AWE_ID_GEOMETRY_MANAGER)) {
            return &((AWE_SLIDER_VTABLE *)obj->pclass->vtable)->geometry_manager;
        }
    }
    return awe_widget_get_interface(obj, name, pnamespace);
}


//clone slider
AWE_OBJECT *awe_slider_clone(AWE_OBJECT *wgt)
{
    return awe_create_object(&awe_slider_class,
        AWE_ID_POSITION, ((AWE_SLIDER *)wgt)->pos,
        AWE_ID_MAX, ((AWE_SLIDER *)wgt)->max_val,
        AWE_ID_ORIENTATION, ((AWE_SLIDER *)wgt)->orientation,
        AWE_ID_HANDLE_WIDTH, ((AWE_SLIDER *)wgt)->handle_width,
        AWE_ID_HANDLE_TYPE, ((AWE_SLIDER *)wgt)->handle_type,
        AWE_ID_STEP, ((AWE_SLIDER *)wgt)->step,
        0);
}


//slider paint
void awe_slider_paint(AWE_WIDGET *wgt, AWE_CANVAS *canvas, const AWE_RECT *clip)
{
    AWE_SLIDER *sldr = (AWE_SLIDER *)wgt;
    int pos = (((sldr->orientation ? wgt->height : wgt->width) - sldr->handle_width) * sldr->pos) / sldr->max_val;
    int state;

    if(!awe_is_enabled_widget_tree(wgt))
        state = AWE_SLIDER_TEXTURE_DISABLED;
    else if(sldr->pressed)
        state = AWE_SLIDER_TEXTURE_PRESSED;
    else if(sldr->highlighted)
        state = AWE_SLIDER_TEXTURE_HIGHLIGHTED;
    else
        state = AWE_SLIDER_TEXTURE_ENABLED;

    awe_fill_rect_s(canvas, 0, 0, wgt->width, wgt->height, makecol(212, 208, 200));
    if(sldr->orientation){
        awe_draw_3d_rect_s(canvas, (wgt->width >> 1) - 2, 0, 4, wgt->height,
            AWE_MAKE_COLOR(sldr->bar_col[0]),
            AWE_MAKE_COLOR(sldr->bar_col[1]),
            2);
    }
    else{
        awe_draw_3d_rect_s(canvas, 0, (wgt->height >> 1) - 2, wgt->width, 4, 
            AWE_MAKE_COLOR(sldr->bar_col[0]),
            AWE_MAKE_COLOR(sldr->bar_col[1]),
            2);
    }
    if (awe_get_focus_widget() == wgt && state != AWE_SLIDER_TEXTURE_DISABLED)
        awe_draw_rect_pattern_s(canvas, 0, 0, wgt->width, wgt->height, makecol(0, 0, 0), AWE_PATTERN_DOT_DOT);
    if(sldr->orientation){
        awe_fill_rect_s(canvas, 2, pos + 2, wgt->width - 4, sldr->handle_width - 4, 
            AWE_MAKE_COLOR(sldr->face_col[state]));
        switch(sldr->handle_type){
            case AWE_SLIDER_HANDLE_LEFT:
                awe_draw_3d_slider_left_s(canvas, 0, pos, wgt->width, sldr->handle_width, 
                    AWE_MAKE_COLOR(sldr->edge_col[state][AWE_SLIDER_EDGE_TOP_LEFT]),
                    AWE_MAKE_COLOR(sldr->edge_col[state][AWE_SLIDER_EDGE_BOTTOM_RIGHT]), 
                    2);
            break;
            case AWE_SLIDER_HANDLE_RIGHT:
                awe_draw_3d_slider_right_s(canvas, 0, pos, wgt->width, sldr->handle_width, 
                    AWE_MAKE_COLOR(sldr->edge_col[state][AWE_SLIDER_EDGE_TOP_LEFT]),
                    AWE_MAKE_COLOR(sldr->edge_col[state][AWE_SLIDER_EDGE_BOTTOM_RIGHT]), 
                    2);
            break;
            default:
                awe_draw_3d_rect_s(canvas, 0, pos, wgt->width, sldr->handle_width, 
                    AWE_MAKE_COLOR(sldr->edge_col[state][AWE_SLIDER_EDGE_TOP_LEFT]),
                    AWE_MAKE_COLOR(sldr->edge_col[state][AWE_SLIDER_EDGE_BOTTOM_RIGHT]), 
                    2);
        }       
    }
    else{
        awe_fill_rect_s(canvas, pos + 2, 2, sldr->handle_width - 4, wgt->height - 4, 
            AWE_MAKE_COLOR(sldr->face_col[state]));
        switch(sldr->handle_type){
            case AWE_SLIDER_HANDLE_UP:
                awe_draw_3d_slider_up_s(canvas, pos, 0, sldr->handle_width, wgt->height, 
                    AWE_MAKE_COLOR(sldr->edge_col[state][AWE_SLIDER_EDGE_TOP_LEFT]),
                    AWE_MAKE_COLOR(sldr->edge_col[state][AWE_SLIDER_EDGE_BOTTOM_RIGHT]), 
                    2);
            break;
            case AWE_SLIDER_HANDLE_DOWN:
                awe_draw_3d_slider_down_s(canvas, pos, 0, sldr->handle_width, wgt->height, 
                    AWE_MAKE_COLOR(sldr->edge_col[state][AWE_SLIDER_EDGE_TOP_LEFT]),
                    AWE_MAKE_COLOR(sldr->edge_col[state][AWE_SLIDER_EDGE_BOTTOM_RIGHT]), 
                    2);
            break;
            default:
                awe_draw_3d_rect_s(canvas, pos, 0, sldr->handle_width, wgt->height, 
                    AWE_MAKE_COLOR(sldr->edge_col[state][AWE_SLIDER_EDGE_TOP_LEFT]),
                    AWE_MAKE_COLOR(sldr->edge_col[state][AWE_SLIDER_EDGE_BOTTOM_RIGHT]), 
                    2);
        }
    }
}


void awe_slider_button_down(AWE_WIDGET *wgt, const AWE_EVENT *event)
{
    AWE_SLIDER *sldr = (AWE_SLIDER *)wgt;
    int mx, my;
    int pos = (((sldr->orientation ? wgt->height : wgt->width) - sldr->handle_width) * sldr->pos) / sldr->max_val;
    
    if (!awe_set_focus_widget(wgt)) return;
    awe_enter_event_mode(awe_grab_event_proc, wgt);

    awe_map_point(NULL, wgt, mouse_x, mouse_y, &mx, &my);
    if(sldr->orientation){
        if(my >= pos && my < pos + sldr->handle_width)
            sldr->pressed = 1;
    }
    else{
        if(mx >= pos && mx < pos + sldr->handle_width)
            sldr->pressed = 1;
    }
    awe_set_widget_dirty(wgt);
}


void awe_slider_button_up(AWE_WIDGET *wgt, const AWE_EVENT *event)
{
    awe_leave_event_mode();
    ((AWE_SLIDER *)wgt)->pressed = 0;
    if(((AWE_SLIDER *)wgt)->lostmouse){
        ((AWE_SLIDER *)wgt)->lostmouse = 0;
        ((AWE_SLIDER *)wgt)->highlighted = 0;
    }
    awe_set_widget_dirty(wgt);
}


void awe_slider_mouse_enter(AWE_WIDGET *wgt, const AWE_EVENT *event)
{
    awe_enter_event_mode(awe_grab_event_proc, wgt);
    ((AWE_SLIDER *)wgt)->oz = event->mouse.z;
    if(((AWE_SLIDER *)wgt)->lostmouse){
       ((AWE_SLIDER *)wgt)->pressed = 1;
       ((AWE_SLIDER *)wgt)->lostmouse = 0;
       awe_set_widget_dirty(wgt);
    }
}


void awe_slider_mouse_leave(AWE_WIDGET *wgt, const AWE_EVENT *event)
{
    awe_leave_event_mode();
    if(((AWE_SLIDER *)wgt)->pressed)
        ((AWE_SLIDER *)wgt)->lostmouse = 1;
    if(((AWE_SLIDER *)wgt)->highlighted){
        ((AWE_SLIDER *)wgt)->highlighted = 0;
        awe_set_widget_dirty(wgt);
    }
}


void awe_slider_mouse_move(AWE_WIDGET *wgt, const AWE_EVENT *event)
{
    AWE_SLIDER *sldr = (AWE_SLIDER *)wgt;
    int mx, my;
    int pos;
    int mp;
    int newpos;
    int irange = sldr->orientation ? wgt->height : wgt->width;
    fixed slratio = itofix(irange - sldr->handle_width) / (sldr->max_val);
    fixed slpos = slratio * sldr->pos;
    awe_map_point(NULL, wgt, mouse_x, mouse_y, &mx, &my);
    if(sldr->pressed){
        mp = sldr->orientation ? mouse_y - (wgt->y + (sldr->handle_width >> 1)) : mouse_x - (wgt->x + (sldr->handle_width >> 1));
        mp = MID(0, mp, irange - sldr->handle_width);
        slpos = itofix(mp);
        newpos = fixtoi(fixdiv(slpos, slratio));
        if(sldr->step > 0){
            float steprange = sldr->max_val / (float)sldr->step;
            newpos = (int)((newpos + (steprange / 2)) / steprange) * steprange;
            sldr->step_pos = newpos / (sldr->max_val / sldr->step);
        }
        if(newpos != sldr->pos){
            sldr->pos = newpos;
            awe_do_widget_event1(wgt, AWE_ID_SLIDER_MOVED, newpos);
            awe_set_widget_dirty(wgt);
        }
    }
    pos = (((sldr->orientation ? wgt->height : wgt->width) - sldr->handle_width) * sldr->pos) / sldr->max_val;
    if(sldr->orientation){
        if(my >= pos && my < pos + sldr->handle_width && sldr->highlighted == 0){
            sldr->highlighted = 1;
            awe_set_widget_dirty(wgt);
        }
        else if(!(my >= pos && my < pos + sldr->handle_width) && sldr->highlighted){
            sldr->highlighted = 0;
            awe_set_widget_dirty(wgt);
        }
    }
    else{
        if(mx >= pos && mx < pos + sldr->handle_width && sldr->highlighted == 0){
            sldr->highlighted = 1;
            awe_set_widget_dirty(wgt);
        }
        else if(!(mx >= pos && mx < pos + sldr->handle_width) && sldr->highlighted){
            sldr->highlighted = 0;
            awe_set_widget_dirty(wgt);
        }
    }   
}


//slider wheel
void awe_slider_mouse_wheel(AWE_WIDGET *wgt, const AWE_EVENT *event){
    AWE_SLIDER *sldr = (AWE_SLIDER *)wgt;
    int newpos;

    if(sldr->step > 0){
        sldr->step_pos = MID(0, sldr->step_pos - (event->mouse.z - sldr->oz), sldr->step);
        newpos = (sldr->max_val / (float)sldr->step) * sldr->step_pos;
    }	
    else
        newpos = MID(0, sldr->pos - ((event->mouse.z - sldr->oz) * MAX(sldr->max_val >> 5, 1)), sldr->max_val);
    if(newpos != sldr->pos){
        sldr->pos = newpos;
        awe_do_widget_event1(wgt, AWE_ID_SLIDER_MOVED, newpos);
        awe_set_widget_dirty(wgt);
    }
    sldr->oz = event->mouse.z;
}
