#include "widget.h"
#include <string.h>
#include "mouse.h"


/*****************************************************************************
    PRIVATE
 *****************************************************************************/


//helpful macros
#define _WGT                 ((AWE_WIDGET *)wgt)
#define _FIRST               awe_get_first_child_widget
#define _LAST                awe_get_last_child_widget
#define _NEXT                awe_get_next_sibling_widget
#define _PREV                awe_get_prev_sibling_widget


//call a widget method
#define _DO(WGT, METHOD, PARAMS)\
    AWE_CALL_METHOD(WGT, AWE_ID_WIDGET, AWE_ID_AWE, AWE_WIDGET_VTABLE, METHOD, PARAMS)


//call a widget method that has a result
#define _DO_R(WGT, METHOD, PARAMS, RESULT, DEFAULT_RESULT)\
    AWE_CALL_METHOD_R(WGT, AWE_ID_WIDGET, AWE_ID_AWE, AWE_WIDGET_VTABLE, METHOD, PARAMS, RESULT, DEFAULT_RESULT)


//forward
static void _gui_update_changes();
static void _gui_update_full();


//variables
static BITMAP *_gui_screen = 0;
static AWE_WIDGET *_root_widget = 0;
static AWE_WIDGET *_focus_widget = 0;
static AWE_WIDGET_OUTPUT_TYPE _widget_output_type = AWE_WIDGET_OUTPUT_DIRECT;
static AWE_GUI_UPDATE_MODE _gui_update_mode = AWE_GUI_UPDATE_CHANGES;
static void (*_gui_update_proc)() = _gui_update_changes;
static AWE_DRS_PROC _gui_update_drs_proc = 0;


//returns widget from screen point
static AWE_WIDGET *_widget_from_point(AWE_WIDGET *wgt, int x, int y)
{
    AWE_WIDGET *child, *result;
    int r;

    if (!AWE_RECT_INCLUDE(wgt->clip, x, y)) return 0;
    for(child = _LAST(wgt); child; child = _PREV(child)) {
        result = _widget_from_point(child, x, y);
        if (result) return result;
    }
    awe_map_point(0, wgt, x, y, &x, &y);
    _DO_R(wgt, hit_test, (wgt, x, y), r, 1);
    return r ? wgt : 0;
}


//inserts a widget
static void _insert_widget(AWE_WIDGET *wgt, AWE_WIDGET *child, int z_order)
{
    AWE_DL_NODE *next_node;

    //get node from z-order
    if (z_order == 0)
        next_node = 0;
    else if (z_order < 0 || z_order >= wgt->children_count)
        next_node = wgt->children.first;
    else
        next_node = awe_list_get_node_from_last(&wgt->children, z_order - 1);

    //insert child
    awe_list_insert(&wgt->children, &child->node.node, next_node);
}


//returns true if widget is show through
static INLINE int _is_trans(AWE_WIDGET *wgt)
{
    return !wgt->opaque || wgt->translucency < 255;
}


//sets a widget's geometry to be updated
static void _set_update_geometry(AWE_WIDGET *wgt)
{
    AWE_WIDGET *par;

    if (wgt->update_geometry) return;
    wgt->update_geometry = 1;
    for(par = wgt->parent; par; par = par->parent) {
        if (par->update_geometry_children) return;
        par->update_geometry_children = 1;
    }
}


//sets a widget's drawable to be updated
static void _set_update_drawable(AWE_WIDGET *wgt)
{
    AWE_WIDGET *par;

    if (wgt->update_drawable) return;
    wgt->update_drawable = 1;
    for(par = wgt->parent; par; par = par->parent) {
        if (par->update_drawable_children) return;
        par->update_drawable_children = 1;
    }
}


//sets a widget's update redraw flag to be updated
static void _set_update_redraw(AWE_WIDGET *wgt)
{
    AWE_WIDGET *par;

    if (wgt->update_redraw) return;
    wgt->update_redraw = 1;
    for(par = wgt->parent; par; par = par->parent) {
        if (par->update_redraw_children) return;
        par->update_redraw_children = 1;
    }
}


//sets a widget's redraw flag and update area
static void _set_redraw(AWE_WIDGET *wgt, AWE_RECT *r)
{
    AWE_WIDGET *par;

    AWE_RECT_UNION(wgt->dirty, *r, wgt->dirty);
    if (wgt->redraw) return;
    wgt->redraw = 1;
    for(par = wgt->parent; par; par = par->parent) {
        if (par->redraw_children) return;
        par->redraw_children = 1;
    }
}


//sets the enabled tree flag
static void _set_enabled_tree(AWE_WIDGET *wgt)
{
    AWE_WIDGET *child;

    wgt->enabled_tree = wgt->enabled && (!wgt->parent || wgt->parent->enabled_tree);
    for(child = _FIRST(wgt); child; child = _NEXT(child)) {
        _set_enabled_tree(child);
    }
}


//sets a widget tree to be on screen
static void _set_on_screen(AWE_WIDGET *wgt)
{
    AWE_WIDGET *child;

    wgt->on_screen = 1;
    for(child = _FIRST(wgt); child; child = _NEXT(child)) {
        _set_on_screen(child);
    }
}


//resets a widget's state
static void _clean_up_widget_helper(AWE_WIDGET *wgt)
{
    AWE_WIDGET *child;

    if (wgt == _focus_widget) _focus_widget = 0;
    AWE_RECT_INVALIDATE(wgt->dirty);
    if (wgt->buffer) {
        destroy_bitmap(wgt->buffer);
        wgt->buffer = 0;
    }
    wgt->on_screen = 0;
    wgt->drawable = 0;
    wgt->geometry_changed = 0;
    wgt->visible_changed = 0;
    wgt->enabled_changed = 0;
    wgt->opaque_changed = 0;
    wgt->translucency_changed = 0;
    wgt->output_type_changed = 0;
    wgt->update_geometry = 0;
    wgt->update_geometry_children = 0;
    wgt->update_drawable = 0;
    wgt->update_drawable_children = 0;
    wgt->update_redraw = 0;
    wgt->update_redraw_children = 0;
    wgt->redraw = 0;
    wgt->redraw_children = 0;
    wgt->repaint = 0;
    wgt->has_mouse = 0;
    for(child = _FIRST(wgt); child; child = _NEXT(child)) {
        _clean_up_widget_helper(child);
    }
}


//removes widget timer events if 'wgt' is the root or descentant of root
static int _remove_widget_timer_events_proc(AWE_EVENT *event, void *root)
{
    if (event->type != AWE_EVENT_TIMER || !event->timer.data) return 1;
    if (event->timer.data == root ||
        awe_is_ancestor_widget((AWE_WIDGET *)root, (AWE_WIDGET *)event->timer.data)) {
        event->type = AWE_EVENT_NONE;
        event->timer.data = 0;
    }
    return 1;
}


//removes timers if 'wgt' is the root or descentant of root
static int _remove_widget_timers_proc(void *wgt, int id, void *root)
{
    if (wgt == root ||
        awe_is_ancestor_widget((AWE_WIDGET *)root, (AWE_WIDGET *)wgt))
        awe_remove_widget_timer((AWE_WIDGET *)wgt, id);
    return 1;
}


//resets a widget's state
static void _clean_up_widget(AWE_WIDGET *wgt)
{
    awe_enum_events(_remove_widget_timer_events_proc, wgt);
    awe_enum_timers(_remove_widget_timers_proc, wgt);
    _clean_up_widget_helper(wgt);
}


//calculates geometry of single widget
static void _calc_widget_geometry(AWE_WIDGET *wgt)
{
    wgt->repaint = (wgt->width  != AWE_RECT_WIDTH (wgt->pos) ||
                    wgt->height != AWE_RECT_HEIGHT(wgt->pos));
    wgt->update_geometry = 0;
    wgt->update_geometry_children = 0;
    wgt->update_drawable = 0;
    wgt->update_drawable_children = 0;
    wgt->update_redraw = 0;
    wgt->update_redraw_children = 0;
    AWE_RECT_INVALIDATE(wgt->dirty);

    if (wgt->parent) {
        wgt->pos.left   = wgt->x + wgt->parent->pos.left;
        wgt->pos.top    = wgt->y + wgt->parent->pos.top;
        wgt->pos.right  = wgt->pos.left + wgt->width  - 1;
        wgt->pos.bottom = wgt->pos.top  + wgt->height - 1;
        AWE_RECT_INTERSECTION(wgt->clip, wgt->pos, wgt->parent->clip);
        wgt->drawable = wgt->visible && wgt->parent->drawable && AWE_RECT_NORMAL(wgt->clip);
    }

    else {
        wgt->pos.left   = wgt->x;
        wgt->pos.top    = wgt->y;
        wgt->pos.right  = wgt->pos.left + wgt->width  - 1;
        wgt->pos.bottom = wgt->pos.top  + wgt->height - 1;
        wgt->clip = wgt->pos;
        wgt->drawable = 1;
    }
}


//calculates the geometry of a widget tree
static void _calc_geometry(AWE_WIDGET *wgt)
{
    AWE_WIDGET *child;

    _calc_widget_geometry(wgt);
    for(child = _FIRST(wgt); child; child = _NEXT(child)) {
        _calc_geometry(child);
    }
}


//calculates the drawable flag of a single widget
static void _calc_widget_drawable(AWE_WIDGET *wgt)
{
    wgt->update_drawable = 0;
    wgt->update_drawable_children = 0;
    wgt->update_redraw = 0;
    wgt->update_redraw_children = 0;
    AWE_RECT_INVALIDATE(wgt->dirty);

    if (wgt->parent)
        wgt->drawable = wgt->visible && wgt->parent->drawable && AWE_RECT_NORMAL(wgt->clip);
    else
        wgt->drawable = 1;
}


//calculates the drawable of a widget tree
static void _calc_drawable(AWE_WIDGET *wgt)
{
    AWE_WIDGET *child;

    _calc_widget_drawable(wgt);
    for(child = _FIRST(wgt); child; child = _NEXT(child)) {
        _calc_drawable(child);
    }
}


//checks if a widget is drawable and inside a rectangle
static INLINE int _is_drawable(AWE_WIDGET *wgt, AWE_RECT *r)
{
    return wgt->drawable && AWE_RECT_OVERLAP(wgt->clip, *r);
}


//sets a list of widgets to redraw
static INLINE void _set_redraw_list(AWE_WIDGET *wgt, AWE_WIDGET *end, AWE_RECT *r)
{
    for(; wgt != end; wgt = _NEXT(wgt)) {
        if (_is_drawable(wgt, r)) _set_redraw(wgt, r);
    }
}


//sets a widget's background widgets redraw flag
static INLINE void _set_redraw_background(AWE_WIDGET *wgt, AWE_RECT *r)
{
    if (!wgt->parent) return;
    if (_is_trans(wgt->parent)) _set_redraw_background(wgt->parent, r);
    _set_redraw(wgt->parent, r);
    _set_redraw_list(_FIRST(wgt->parent), wgt, r);
}


//sets a widget's foreground widgets redraw flag
static INLINE void _set_redraw_foreground(AWE_WIDGET *wgt, AWE_RECT *r)
{
    for(; wgt; wgt = wgt->parent) {
        _set_redraw_list(_NEXT(wgt), 0, r);
    }
}


//checks if a widget's bitmap buffer is valid
static INLINE int _is_valid_buffer(AWE_WIDGET *wgt, int color_depth, int (*proc)(BITMAP *))
{
    return wgt->buffer &&
           wgt->buffer->w == wgt->width &&
           wgt->buffer->h == wgt->height &&
           bitmap_color_depth(wgt->buffer) == color_depth &&
           proc(wgt->buffer);
}


//paints a single widget
static int _paint_widget(AWE_WIDGET *wgt, AWE_RECT *clip, AWE_RECT *t)
{
    AWE_CANVAS canvas;
    AWE_RECT r;

    //test if drawable inside given clip
    if (!_is_drawable(wgt, clip)) return 0;

    //calc new clip
    AWE_RECT_INTERSECTION(*t, wgt->clip, *clip);

    //map update rect to widget coordinate space
    r.left   = t->left   - wgt->pos.left;
    r.top    = t->top    - wgt->pos.top ;
    r.right  = t->right  - wgt->pos.left;
    r.bottom = t->bottom - wgt->pos.top ;

    //if translucent, try memory bitmap
    if (wgt->translucency < 255) wgt->output_type = AWE_WIDGET_OUTPUT_MEMORY_BITMAP;

    //manage buffer
    switch (wgt->output_type) {
        case AWE_WIDGET_OUTPUT_VIDEO_BITMAP:
            if (_is_valid_buffer(wgt, bitmap_color_depth(_gui_screen), is_video_bitmap)) break;
            wgt->repaint = 1;
            if (wgt->buffer) destroy_bitmap(wgt->buffer);
            do {
                wgt->buffer = create_video_bitmap(wgt->width, wgt->height);
            } while (wgt->buffer && is_same_bitmap(screen, wgt->buffer));
            if (wgt->buffer) break;
            wgt->output_type = AWE_WIDGET_OUTPUT_MEMORY_BITMAP;

        case AWE_WIDGET_OUTPUT_MEMORY_BITMAP:
            if (_is_valid_buffer(wgt, bitmap_color_depth(_gui_screen), is_memory_bitmap)) break;
            wgt->repaint = 1;
            if (wgt->buffer) destroy_bitmap(wgt->buffer);
            wgt->buffer = create_bitmap_ex(bitmap_color_depth(_gui_screen), wgt->width, wgt->height);
            if (wgt->buffer) break;
            wgt->output_type = AWE_WIDGET_OUTPUT_DIRECT;

        default:
            if (!wgt->buffer) break;
            destroy_bitmap(wgt->buffer);
            wgt->buffer = 0;
    }

    //draw unbufferred widget
    if (!wgt->buffer) {
        //prepare canvas
        awe_set_canvas(&canvas, _gui_screen, &wgt->pos);

        //clip canvas
        set_clip(_gui_screen, t->left, t->top, t->right, t->bottom);

        //paint
        _DO(wgt, paint, (wgt, &canvas, &r));
    }

    //draw bufferred widget
    else {
         acquire_bitmap(wgt->buffer);

        //paint, if needed
        if (wgt->repaint) {
            //prepare canvas
            AWE_RECT r = {0, 0, wgt->buffer->w - 1, wgt->buffer->h - 1};
            awe_set_canvas(&canvas, wgt->buffer, &r);

            if (!wgt->opaque) clear_to_color(wgt->buffer, bitmap_mask_color(wgt->buffer));
            _DO(wgt, paint, (wgt, &canvas, &r));
        }

        //blit the result to the screen, according to translucency
        set_clip(_gui_screen, t->left, t->top, t->right, t->bottom);
        if (wgt->translucency == 255) {
            if (wgt->opaque)
                blit(wgt->buffer, _gui_screen, r.left, r.top, t->left, t->top, AWE_RECT_WIDTH(*t), AWE_RECT_HEIGHT(*t));
            else
                masked_blit(wgt->buffer, _gui_screen, r.left, r.top, t->left, t->top, AWE_RECT_WIDTH(*t), AWE_RECT_HEIGHT(*t));
        }
        else {
            set_trans_blender(0, 0, 0, wgt->translucency);
            draw_trans_sprite(_gui_screen, wgt->buffer, wgt->pos.left, wgt->pos.top);
        }

        release_bitmap(wgt->buffer);
    }

    //reset state
    wgt->repaint = 0;
    wgt->update_redraw = 0;
    wgt->update_redraw_children = 0;
    wgt->redraw = 0;
    wgt->redraw_children = 0;
    AWE_RECT_INVALIDATE(wgt->dirty);

    return 1;
}


//paints a widget; takes care of the widget buffer and translucency effects
static void _paint_tree(AWE_WIDGET *wgt, AWE_RECT *clip)
{
    AWE_WIDGET *child;
    AWE_RECT t;

    if (!_paint_widget(wgt, clip, &t)) return;

    //paint children
    for(child = _FIRST(wgt); child; child = _NEXT(child)) {
        _paint_tree(child, &t);
    }
}


//redraws a widget and its children
static void _redraw_widget(AWE_WIDGET *wgt)
{
    int cl, ct, cr, cb;
    AWE_RECT t;
    BITMAP *tmp = _mouse_screen;  
  
    AWE_RECT_INTERSECTION(t, wgt->clip, wgt->dirty);

    //acquire bitmap
    acquire_bitmap(_gui_screen);

    //hide mouse
    if (tmp == _gui_screen)
        awe_scare_mouse_area(t.left, t.top, AWE_RECT_WIDTH(t), AWE_RECT_HEIGHT(t));

    //save screen clipping
    cl = _gui_screen->cl;
    ct = _gui_screen->ct;
    cr = _gui_screen->cr;
    cb = _gui_screen->cb;

    //paint
    _paint_tree(wgt, &t);

    //restore screen clipping
    _gui_screen->cl = cl;
    _gui_screen->ct = ct;
    _gui_screen->cr = cr;
    _gui_screen->cb = cb;

    //show mouse
    if (tmp == _gui_screen) 
        awe_unscare_mouse();

    //release bitmap
    release_bitmap(_gui_screen);

    //notify DRS system
    if (_gui_update_drs_proc) _gui_update_drs_proc(&t);
}


//updates geometry
static void _update_geometry(AWE_WIDGET *wgt)
{
    AWE_WIDGET *child;
    AWE_RECT t;

    if (wgt->update_geometry) {
        AWE_RECT_INVALIDATE(t);
        if (wgt->drawable) AWE_RECT_UNION(t, wgt->clip, t);
        _calc_geometry(wgt);
        if (wgt->drawable) AWE_RECT_UNION(t, wgt->clip, t);
        if (AWE_RECT_NORMAL(t)) {
            if (wgt->parent) {
                if (_is_trans(wgt->parent)) _set_redraw_background(wgt->parent, &t);
                _set_redraw(wgt->parent, &t);
                _set_redraw_foreground(wgt->parent, &t);
            }
            else _set_redraw(wgt, &t);
        }
    }

    else if (wgt->update_geometry_children) {
        wgt->update_geometry_children = 0;
        for(child = _FIRST(wgt); child; child = _NEXT(child)) {
            _update_geometry(child);
        }
    }
}


//updates drawable
static void _update_drawable(AWE_WIDGET *wgt)
{
    AWE_WIDGET *child;

    if (wgt->update_drawable) {
        _calc_drawable(wgt);

        //if shown, redraw background only if trans,
        //else if hidden, force background redraw
        if (wgt->drawable) {
            if (_is_trans(wgt)) _set_redraw_background(wgt, &wgt->clip);
            _set_redraw(wgt, &wgt->clip);
        }
        else {
            _set_redraw_background(wgt, &wgt->clip);
        }
        _set_redraw_foreground(wgt, &wgt->clip);
    }

    else if (wgt->update_drawable_children) {
        wgt->update_drawable_children = 0;
        for(child = _FIRST(wgt); child; child = _NEXT(child)) {
            _update_drawable(child);
        }
    }
}


/* returns the widget to apply the update redraw to; this is neccessary because
   a child widget is always drawn if the parent is drawn; but if the child's
   dirty area is not inside the parent's dirty area, then the child will not
   be redrawn.
 */
static INLINE AWE_WIDGET *_get_update_redraw_widget(AWE_WIDGET *wgt)
{
    AWE_WIDGET *t;

    for(t = wgt; t; t = t->parent) {
        if (t->redraw) return t;
    }
    return wgt;
}


//updates redraw
static void _update_redraw(AWE_WIDGET *wgt)
{
    AWE_WIDGET *child;

    if (wgt->update_redraw) {
        wgt = _get_update_redraw_widget(wgt);
        if (_is_trans(wgt)) _set_redraw_background(wgt, &wgt->clip);
        _set_redraw(wgt, &wgt->clip);
        _set_redraw_foreground(wgt, &wgt->clip);
    }
    else if (wgt->update_redraw_children) {
        wgt->update_redraw_children = 0;
        for(child = _FIRST(wgt); child; child = _NEXT(child)) {
            _update_redraw(child);
        }
    }
}


//redraws changes
static void _redraw_changes(AWE_WIDGET *wgt)
{
    AWE_WIDGET *child;

    if (wgt->redraw) {
        _redraw_widget(wgt);
    }
    else if (wgt->redraw_children) {
        wgt->redraw_children = 0;
        for(child = _FIRST(wgt); child; child = _NEXT(child)) {
            _redraw_changes(child);
        }
    }
}


//a widget's constructor
static void _widget_constructor(AWE_OBJECT *obj)
{
    AWE_WIDGET *wgt = (AWE_WIDGET *)obj;

    wgt->node.data = wgt;
    wgt->width = 32;
    wgt->height = 32;
    wgt->output_type = _widget_output_type;
    wgt->translucency = 255;
    wgt->visible = 1;
    wgt->enabled = 1;
    wgt->enabled_tree = 1;
    wgt->opaque = 1;
    wgt->repaint = 1;
}


//a widget's destructor
static void _widget_destructor(AWE_OBJECT *obj)
{
    AWE_WIDGET *wgt = (AWE_WIDGET *)obj, *child;

    //remove from parent/reset the root widget
    if (wgt->parent) awe_remove_widget(wgt);
    else if (wgt == _root_widget) awe_set_root_widget(0);

    //destroy children
    for(child = _FIRST(wgt); child; child = _NEXT(child)) {
        awe_destroy_widget(child);
    }
}


//retrieves the value of the relevant property
static void _widget_get_x(AWE_OBJECT *wgt, void *buffer)
{
    *(int *)buffer = _WGT->x;
}


//retrieves the value of the relevant property
static void _widget_get_y(AWE_OBJECT *wgt, void *buffer)
{
    *(int *)buffer = _WGT->y;
}


//retrieves the value of the relevant property
static void _widget_get_width(AWE_OBJECT *wgt, void *buffer)
{
    *(int *)buffer = _WGT->width;
}


//retrieves the value of the relevant property
static void _widget_get_height(AWE_OBJECT *wgt, void *buffer)
{
    *(int *)buffer = _WGT->height;
}


//retrieves the value of the relevant property
static void _widget_get_visible(AWE_OBJECT *wgt, void *buffer)
{
    *(unsigned *)buffer = _WGT->visible;
}


//retrieves the value of the relevant property
static void _widget_get_enabled(AWE_OBJECT *wgt, void *buffer)
{
    *(unsigned *)buffer = _WGT->enabled;
}


//retrieves the value of the relevant property
static void _widget_get_opaque(AWE_OBJECT *wgt, void *buffer)
{
    *(unsigned *)buffer = _WGT->opaque;
}


//retrieves the value of the relevant property
static void _widget_get_translucency(AWE_OBJECT *wgt, void *buffer)
{
    *(unsigned *)buffer = _WGT->translucency;
}


//retrieves the value of the relevant property
static void _widget_get_output_type(AWE_OBJECT *wgt, void *buffer)
{
    *(AWE_WIDGET_OUTPUT_TYPE *)buffer = _WGT->output_type;
}


//sets the value of the relevant property
static void _widget_set_x(AWE_OBJECT *wgt, void *buffer)
{    
    int value = *(int *)buffer;
    if (_WGT->x == value) return;
    _WGT->x = value;
    _WGT->geometry_changed = 1;
}


//sets the value of the relevant property
static void _widget_set_y(AWE_OBJECT *wgt, void *buffer)
{
    int value = *(int *)buffer;
    if (_WGT->y == value) return;
    _WGT->y = value;
    _WGT->geometry_changed = 1;
}


//sets the value of the relevant property
static void _widget_set_width(AWE_OBJECT *wgt, void *buffer)
{
    int value = *(int *)buffer;
    value = MAX(value, 0);
    if (_WGT->width == value) return;
    _WGT->width = value;
    _WGT->geometry_changed = 1;
}


//sets the value of the relevant property
static void _widget_set_height(AWE_OBJECT *wgt, void *buffer)
{
    int value = *(int *)buffer;
    value = MAX(value, 0);
    if (_WGT->height == value) return;
    _WGT->height = value;
    _WGT->geometry_changed = 1;
}


//sets the value of the relevant property
static void _widget_set_visible(AWE_OBJECT *wgt, void *buffer)
{
    unsigned value = *(int *)buffer;
    if (_WGT->visible == value) return;
    _WGT->visible = value;
    _WGT->visible_changed = 1;
}


//sets the value of the relevant property
static void _widget_set_enabled(AWE_OBJECT *wgt, void *buffer)
{
    unsigned value = *(int *)buffer;
    if (_WGT->enabled == value) return;
    _WGT->enabled = value;
    _WGT->enabled_changed = 1;
}


//sets the value of the relevant property
static void _widget_set_opaque(AWE_OBJECT *wgt, void *buffer)
{
    unsigned value = *(int *)buffer;
    if (_WGT->opaque == value) return;
    _WGT->opaque = value;
    _WGT->opaque_changed = 1;
}


//sets the value of the relevant property
static void _widget_set_translucency(AWE_OBJECT *wgt, void *buffer)
{
    unsigned value = *(int *)buffer;
    if (_WGT->translucency == value) return;
    _WGT->translucency = value;
    _WGT->translucency_changed = 1;
}


//sets the value of the relevant property
static void _widget_set_output_type(AWE_OBJECT *wgt, void *buffer)
{
    AWE_WIDGET_OUTPUT_TYPE value = *(AWE_WIDGET_OUTPUT_TYPE *)buffer;
    if (_WGT->output_type == value) return;
    _WGT->output_type = value;
    _WGT->output_type_changed = 1;
}


//output type enumeration
static AWE_CLASS_ENUMERATION _output_type_enum[] = {
    {"AWE_WIDGET_OUTPUT_DIRECT"       , AWE_WIDGET_OUTPUT_DIRECT       },
    {"AWE_WIDGET_OUTPUT_VIDEO_BITMAP" , AWE_WIDGET_OUTPUT_VIDEO_BITMAP },
    {"AWE_WIDGET_OUTPUT_MEMORY_BITMAP", AWE_WIDGET_OUTPUT_MEMORY_BITMAP},
    {0}
};


//update changes
static void _gui_update_changes()
{
    _update_geometry(_root_widget);
    _update_drawable(_root_widget);
    _update_redraw(_root_widget);
    _redraw_changes(_root_widget);
}


//does a total widget update
static void _update_widget(AWE_WIDGET *wgt, int update_geometry, int update_drawable)
{
    AWE_WIDGET *child;
    AWE_RECT t;

    //recalculate stuff, if needed
    if (update_geometry || wgt->update_geometry) {
        update_geometry = 1;
        _calc_widget_geometry(wgt);
    }
    if (update_drawable || wgt->update_drawable) {
        update_drawable = 1;
        _calc_widget_drawable(wgt);
    }

    //paint widget
    _paint_widget(wgt, &wgt->clip, &t);

    //update children
    for(child = _FIRST(wgt); child; child = _NEXT(child)) {
        _update_widget(child, update_geometry, update_drawable);
    }
}


//full gui update
static void _gui_update_full()
{
    int cl, ct, cr, cb;

    //save screen clipping
    cl = _gui_screen->cl;
    ct = _gui_screen->ct;
    cr = _gui_screen->cr;
    cb = _gui_screen->cb;

    //update widget
    _update_widget(_root_widget, 0, 0);

    //restore screen clipping
    _gui_screen->cl = cl;
    _gui_screen->cl = ct;
    _gui_screen->cl = cr;
    _gui_screen->cl = cb;
}


/*****************************************************************************
    PUBLIC
 *****************************************************************************/


//widget properties
AWE_CLASS_PROPERTY awe_widget_properties[] = {
    {AWE_ID_X           , "int"                   , sizeof(int)                   , _widget_get_x           , _widget_set_x           , 0},
    {AWE_ID_Y           , "int"                   , sizeof(int)                   , _widget_get_y           , _widget_set_y           , 0},
    {AWE_ID_WIDTH       , "int"                   , sizeof(int)                   , _widget_get_width       , _widget_set_width       , 0},
    {AWE_ID_HEIGHT      , "int"                   , sizeof(int)                   , _widget_get_height      , _widget_set_height      , 0},
    {AWE_ID_VISIBLE     , "bool"                  , sizeof(int)                   , _widget_get_visible     , _widget_set_visible     , 0},
    {AWE_ID_ENABLED     , "bool"                  , sizeof(int)                   , _widget_get_enabled     , _widget_set_enabled     , 0},
    {AWE_ID_OPAQUE      , "bool"                  , sizeof(int)                   , _widget_get_opaque      , _widget_set_opaque      , 0},
    {AWE_ID_TRANSLUCENCY, "int"                   , sizeof(int)                   , _widget_get_translucency, _widget_set_translucency, 0},
    {AWE_ID_OUTPUT_TYPE , "AWE_WIDGET_OUTPUT_TYPE", sizeof(AWE_WIDGET_OUTPUT_TYPE), _widget_get_output_type , _widget_set_output_type , _output_type_enum},
    {0}
};


//widget vtable
AWE_WIDGET_VTABLE awe_widget_vtable = {
    {
        awe_widget_get_interface,
        awe_widget_properties_changed,
        awe_widget_clone
    },
    awe_widget_paint,
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
    0,
    0,
    0,
    0
};


//the one and only widget class; base class for all widgets
AWE_CLASS awe_widget_class = {
    AWE_ID_WIDGET,
    AWE_ID_AWE,
    &awe_object_class,
    sizeof(AWE_WIDGET),
    awe_widget_properties,
    0,
    &awe_widget_vtable.object,
    _widget_constructor,
    _widget_destructor
};


//returns an interface
void *awe_widget_get_interface(AWE_OBJECT *wgt, const char *name, const char *pnamespace)
{
    if (!strcmp(pnamespace, AWE_ID_AWE)) {
        if (!strcmp(name, AWE_ID_WIDGET)) return wgt->pclass->vtable;
    }
    return awe_object_class.vtable->get_interface(wgt, name, pnamespace);
}


//properties changed
void awe_widget_properties_changed(AWE_OBJECT *wgt)
{
    if (_WGT->geometry_changed) {
        _WGT->geometry_changed = 0;
        if (_WGT->on_screen) {
            _set_update_geometry(_WGT);
            _DO(wgt, geometry_changed, (_WGT, _WGT->x, _WGT->y, _WGT->width, _WGT->height));
        }
    }

    if (_WGT->visible_changed) {
        _WGT->visible_changed = 0;
        if (_WGT->on_screen) {
            _set_update_drawable(_WGT);
            _DO(wgt, visible_changed, (_WGT, _WGT->visible));
        }
    }

    if (_WGT->enabled_changed) {
        _WGT->enabled_changed = 0;
        _set_enabled_tree(_WGT);
        if (_WGT->on_screen) {
            if (!_WGT->enabled && _WGT == _focus_widget) _focus_widget = 0;
            _set_update_redraw(_WGT);
        }
    }

    if (_WGT->opaque_changed) {
        _WGT->opaque_changed = 0;
        if (_WGT->on_screen) _set_update_redraw(_WGT);
    }

    if (_WGT->translucency_changed) {
        _WGT->translucency_changed = 0;
        if (_WGT->on_screen) _set_update_redraw(_WGT);
    }

    if (_WGT->output_type_changed) {
        _WGT->output_type_changed = 0;
        if (_WGT->on_screen) _set_update_redraw(_WGT);
    }
}


//clone widget
AWE_OBJECT *awe_widget_clone(AWE_OBJECT *wgt)
{
    return awe_create_object(&awe_widget_class, 
        AWE_ID_X           , _WGT->x           ,
        AWE_ID_Y           , _WGT->y           ,
        AWE_ID_WIDTH       , _WGT->width       ,
        AWE_ID_HEIGHT      , _WGT->height      ,
        AWE_ID_VISIBLE     , _WGT->visible     ,
        AWE_ID_ENABLED     , _WGT->enabled     ,
        AWE_ID_OPAQUE      , _WGT->opaque      ,
        AWE_ID_TRANSLUCENCY, _WGT->translucency,
        AWE_ID_OUTPUT_TYPE , _WGT->output_type ,
        0);
}


//paint widget
void awe_widget_paint(AWE_WIDGET *wgt, AWE_CANVAS *canvas, const AWE_RECT *update_rect)
{
    int back_color = makecol(255, 255, 255);
    int fore_color = wgt->enabled_tree ? makecol(0, 0, 0) : makecol(128, 128, 128);
    int text_color = fore_color;
    int text_x;

    solid_mode();
    if (wgt->opaque) awe_fill_rect_s(canvas, 0, 0, _WGT->width, _WGT->height, back_color);
    awe_draw_rect_s(canvas, 0, 0, _WGT->width, _WGT->height, fore_color);
    text_x = awe_draw_text(canvas, font, wgt->object.pclass->pnamespace, 2, 2, text_color, -1);
    text_x = awe_draw_text(canvas, font, "::", text_x, 2, text_color, -1);
    awe_draw_text(canvas, font, wgt->object.pclass->name, text_x, 2, text_color, -1);
    if (_focus_widget == wgt) awe_draw_rect_s(canvas, 1, 1, _WGT->width - 2, _WGT->height - 2, fore_color);
}


//sends the event to its parent
void awe_widget_button_down(AWE_WIDGET *wgt, const AWE_EVENT *event)
{
    if (!wgt->parent) return;
    AWE_CALL_METHOD(wgt->parent, AWE_ID_WIDGET, AWE_ID_AWE, AWE_WIDGET_VTABLE, button_down, (wgt->parent, event));
}


//sends the event to its parent
void awe_widget_button_up(AWE_WIDGET *wgt, const AWE_EVENT *event)
{
    if (!wgt->parent) return;
    AWE_CALL_METHOD(wgt->parent, AWE_ID_WIDGET, AWE_ID_AWE, AWE_WIDGET_VTABLE, button_up, (wgt->parent, event));
}


//sends the event to its parent
void awe_widget_mouse_enter(AWE_WIDGET *wgt, const AWE_EVENT *event)
{
    if (!wgt->parent) return;
    AWE_CALL_METHOD(wgt->parent, AWE_ID_WIDGET, AWE_ID_AWE, AWE_WIDGET_VTABLE, mouse_enter, (wgt->parent, event));
}


//sends the event to its parent
void awe_widget_mouse_move(AWE_WIDGET *wgt, const AWE_EVENT *event)
{
    if (!wgt->parent) return;
    AWE_CALL_METHOD(wgt->parent, AWE_ID_WIDGET, AWE_ID_AWE, AWE_WIDGET_VTABLE, mouse_move, (wgt->parent, event));
}


//sends the event to its parent
void awe_widget_mouse_leave(AWE_WIDGET *wgt, const AWE_EVENT *event)
{
    if (!wgt->parent) return;
    AWE_CALL_METHOD(wgt->parent, AWE_ID_WIDGET, AWE_ID_AWE, AWE_WIDGET_VTABLE, mouse_leave, (wgt->parent, event));
}


//sends the event to its parent
void awe_widget_mouse_wheel(AWE_WIDGET *wgt, const AWE_EVENT *event)
{
    if (!wgt->parent) return;
    AWE_CALL_METHOD(wgt->parent, AWE_ID_WIDGET, AWE_ID_AWE, AWE_WIDGET_VTABLE, mouse_wheel, (wgt->parent, event));
}


//sends the event to its parent
void awe_widget_key_down(AWE_WIDGET *wgt, const AWE_EVENT *event)
{
    if (!wgt->parent) return;
    AWE_CALL_METHOD(wgt->parent, AWE_ID_WIDGET, AWE_ID_AWE, AWE_WIDGET_VTABLE, key_down, (wgt->parent, event));
}


//sends the event to its parent
void awe_widget_key_up(AWE_WIDGET *wgt, const AWE_EVENT *event)
{
    if (!wgt->parent) return;
    AWE_CALL_METHOD(wgt->parent, AWE_ID_WIDGET, AWE_ID_AWE, AWE_WIDGET_VTABLE, key_up, (wgt->parent, event));
}


//get focus
int awe_widget_get_focus(AWE_WIDGET *wgt)
{
    int r;

    if (!wgt->on_screen || !wgt->enabled_tree) return 0;
    if (wgt == _focus_widget) return 1;
    if (_focus_widget) {
        _DO_R(_focus_widget, lose_focus, (_focus_widget), r, 1);
        if (!r) return 0;
    }
    _focus_widget = wgt;
    _set_update_redraw(wgt);
    return 1;
}


//lose focus
int awe_widget_lose_focus(AWE_WIDGET *wgt)
{
    if (wgt != _focus_widget) return 0;
    _focus_widget = 0;
    _set_update_redraw(wgt);
    return 1;
}


//begin display
void awe_widget_begin_display(AWE_WIDGET *wgt)
{
    AWE_WIDGET *child;

    for(child = _FIRST(wgt); child; child = _NEXT(child)) {
        _DO(child, begin_display, (child));
    }
}


//end display
void awe_widget_end_display(AWE_WIDGET *wgt)
{
    AWE_WIDGET *child;

    for(child = _FIRST(wgt); child; child = _NEXT(child)) {
        _DO(child, end_display, (child));
    }
}


//insert child
int awe_widget_insert_widget(AWE_WIDGET *wgt, AWE_WIDGET *child, int z_order)
{
    //check if widget can be inserted
    if (child == wgt || 
        child->parent || 
        child->on_screen || 
        child == _root_widget ||
        awe_is_ancestor_widget(child, wgt))
        return 0;

    //insert widget
    _insert_widget(wgt, child, z_order);
    child->parent = wgt;
    wgt->children_count++;

    //set the enabled tree flag
    _set_enabled_tree(wgt);

    //update screen
    if (wgt->on_screen) {
        _set_on_screen(child);
        _set_update_geometry(child);
        _DO(child, begin_display, (child));
    }

    return 1;
}


//remove child
int awe_widget_remove_widget(AWE_WIDGET *wgt, AWE_WIDGET *child)
{
    //check if widget can be removed
    if (child->parent != wgt) return 0;

    //remove
    awe_list_remove(&wgt->children, &child->node.node);
    wgt->children_count--;

    //set the enabled tree flag
    _set_enabled_tree(wgt);

    //update screen
    if (wgt->on_screen) {
        if (child->drawable) {
            if (_is_trans(wgt)) _set_redraw_background(wgt, &child->clip);
            _set_redraw(wgt, &child->clip);
            _set_redraw_foreground(wgt, &child->clip);
        }
        _clean_up_widget(child);
        _DO(child, end_display, (child));
    }

    //set child's parent to null after the update screen step,
    //in order for the child to be able to notify its parent
    child->parent = 0;

    return 1;
}


//casts 'widget' to 'object'
AWE_OBJECT *awe_get_widget_object(AWE_WIDGET *wgt)
{
    return &wgt->object;
}


//returns the parent widget
AWE_WIDGET *awe_get_parent_widget(AWE_WIDGET *wgt)
{
    return wgt->parent;
}


//returns the previous sibling widget
AWE_WIDGET *awe_get_prev_sibling_widget(AWE_WIDGET *wgt)
{
    AWE_DL_DATA_NODE *node = (AWE_DL_DATA_NODE *)wgt->node.node.prev;
    return node ? (AWE_WIDGET *)node->data : 0;
}


//returns the next sibling widget
AWE_WIDGET *awe_get_next_sibling_widget(AWE_WIDGET *wgt)
{
    AWE_DL_DATA_NODE *node = (AWE_DL_DATA_NODE *)wgt->node.node.next;
    return node ? (AWE_WIDGET *)node->data : 0;
}


//returns the first child widget
AWE_WIDGET *awe_get_first_child_widget(AWE_WIDGET *wgt)
{
    AWE_DL_DATA_NODE *node = (AWE_DL_DATA_NODE *)wgt->children.first;
    return node ? (AWE_WIDGET *)node->data : 0;
}


//returns the last child widget
AWE_WIDGET *awe_get_last_child_widget(AWE_WIDGET *wgt)
{
    AWE_DL_DATA_NODE *node = (AWE_DL_DATA_NODE *)wgt->children.last;
    return node ? (AWE_WIDGET *)node->data : 0;
}


//returns the number of children widgets
int awe_get_widget_count(AWE_WIDGET *wgt)
{
    return wgt->children_count;
}


//retrieves the z-order of a widget
int awe_get_widget_z_order(AWE_WIDGET *wgt)
{
    if (!wgt->parent) return 0;
    return awe_list_get_node_index_from_last(&wgt->parent->children, &wgt->node.node);
}


//checks if given widget's ancestors are enabled
int awe_is_enabled_widget_tree(AWE_WIDGET *wgt)
{
    return wgt->enabled_tree;
}


//checks if the given widget is on the screen
int awe_is_on_screen_widget(AWE_WIDGET *wgt)
{
    return wgt->on_screen;
}


//checks if a widget is ancestor of another widget
int awe_is_ancestor_widget(AWE_WIDGET *a, AWE_WIDGET *d)
{
    for(d = d->parent; d; d = d->parent) {
        if (a == d) return 1;
    }
    return 0;
}


//returns a pointer to the screen rectangle of the widget
const AWE_RECT *awe_get_widget_rect(AWE_WIDGET *wgt)
{
    return &wgt->pos;
}


//checks if the widget has currently the mouse
int awe_widget_has_mouse(AWE_WIDGET *wgt)
{
    return wgt->has_mouse;
}


//tests if given coordinates are inside the given widget
int awe_widget_includes_point(AWE_WIDGET *wgt, int x, int y)
{
    int r;

    if (x < 0 || y < 0 || x >= wgt->width || y >= wgt->height) return 0;
    _DO_R(wgt, hit_test, (wgt, x, y), r, 1);
    return r;
}


//returns a widget from given point
AWE_WIDGET *awe_get_widget_from_point(AWE_WIDGET *wgt, int x, int y)
{
    if (!wgt->drawable) return 0;
    awe_map_point(wgt, 0, x, y, &x, &y);
    return _widget_from_point(wgt, x, y);
}


//checks if an widget is or derives from a specific class
int awe_widget_is_class(AWE_WIDGET *wgt, const char *name, const char *pnamespace)
{
    return awe_object_is_class(&wgt->object, name, pnamespace);
}


//retrieves an interface (a vtable pointer) from an widget
void *awe_get_widget_interface(AWE_WIDGET *wgt, const char *name, const char *pnamespace)
{
    return awe_get_object_interface(&wgt->object, name, pnamespace);
}


//returns the class of an widget
AWE_CLASS *awe_get_widget_class(AWE_WIDGET *wgt)
{
    return awe_get_object_class(&wgt->object);
}


//registers an widget as a listener to an event of another widget
int awe_add_widget_event(AWE_WIDGET *src, const char *name, AWE_WIDGET *dst, void *proc)
{
    return awe_add_object_event(&src->object, name, &dst->object, proc);
}


//unregisters an widget as listener of an event that is produced by another
int awe_remove_widget_event(AWE_WIDGET *src, const char *name, AWE_WIDGET *dst, void *proc)
{
    return awe_remove_object_event(&src->object, name, &dst->object, proc);
}


//creates a widget from given COM class and properties 
AWE_WIDGET *awe_create_widget_va(AWE_CLASS *pclass, AWE_WIDGET *parent, va_list params)
{
    AWE_WIDGET *wgt;

    wgt = (AWE_WIDGET *)awe_create_object_va(pclass, params);
    if (wgt) awe_add_widget(parent, wgt);
    return wgt;
}


//creates a widget from given COM class and properties. 
AWE_WIDGET *awe_create_widget(AWE_CLASS *pclass, AWE_WIDGET *parent, ...)
{
    AWE_WIDGET *wgt;
    va_list params;

    va_start(params, parent);
    wgt = (AWE_WIDGET *)awe_create_object_va(pclass, params);
    va_end(params);
    if (wgt && parent) awe_add_widget(parent, wgt);
    return wgt;
}


//creates a widget from named class and properties. 
AWE_WIDGET *awe_create_widget_by_name(const char *name, const char *pnamespace, AWE_WIDGET *parent, ...)
{
    AWE_WIDGET *wgt;
    va_list params;

    va_start(params, parent);
    wgt = (AWE_WIDGET *)awe_create_object_by_name_va(name, pnamespace, params);
    va_end(params);
    if (wgt) awe_add_widget(parent, wgt);
    return wgt;
}


//clones a widget
AWE_WIDGET *awe_clone_widget(AWE_WIDGET *wgt)
{
    return (AWE_WIDGET *)awe_clone_object(&wgt->object);
}


//destroys a widget
void awe_destroy_widget(AWE_WIDGET *wgt)
{
    awe_destroy_object(&wgt->object);
}


//retrieves the properties of a widget
int awe_get_widget_properties_va(AWE_WIDGET *wgt, va_list params)
{
    return awe_get_object_properties_va(&wgt->object, params);
}


//retrieves the properties of a widget
int awe_get_widget_properties(AWE_WIDGET *wgt, ...)
{
    va_list params;
    int result;

    va_start(params, wgt);
    result = awe_get_object_properties_va(&wgt->object, params);
    va_end(params);
    return result;
}


//sets the properties of a widget
int awe_set_widget_properties_va(AWE_WIDGET *wgt, va_list params)
{
    return awe_set_object_properties_va(&wgt->object, params);
}


//sets the properties of a widget
int awe_set_widget_properties(AWE_WIDGET *wgt, ...)
{
    va_list params;
    int result;

    va_start(params, wgt);
    result = awe_set_object_properties_va(&wgt->object, params);
    va_end(params);
    return result;
}


//sets the properties of a widget tree
int awe_set_widget_tree_properties_va(AWE_WIDGET *wgt, va_list params)
{
    AWE_WIDGET *child;
    int result = 0;

    result += awe_set_widget_properties_va(wgt, params);
    for(child = _FIRST(wgt); child; child = _NEXT(child)) {
        result += awe_set_widget_tree_properties_va(child, params);
    }
    return result;
}


//sets the properties of a widget tree
int awe_set_widget_tree_properties(AWE_WIDGET *wgt, ...)
{
    va_list params;
    int result;

    va_start(params, wgt);
    result = awe_set_widget_tree_properties_va(wgt, params);
    va_end(params);
    return result;
}


//returns the current root widget
AWE_WIDGET *awe_get_root_widget()
{
    return _root_widget;
}


//sets the root widget
int awe_set_root_widget(AWE_WIDGET *wgt)
{
    if (wgt == _root_widget) return 1;
    if (wgt && wgt->parent) return 0;
    if (_root_widget) {
        _clean_up_widget(_root_widget);
        _DO(_root_widget, end_display, (_root_widget));
    }
    _root_widget = wgt;
    if (!_root_widget) return 1;
    _set_on_screen(_root_widget);
    _set_update_geometry(_root_widget);
    _DO(_root_widget, begin_display, (wgt));
    awe_set_focus_widget(wgt);
    return 1;
}


//returns the current focus widget
AWE_WIDGET *awe_get_focus_widget()
{
    return _focus_widget;
}


//sets the focus widget
int awe_set_focus_widget(AWE_WIDGET *wgt)
{
    int result;
    _DO_R(wgt, get_focus, (wgt), result, 0);
    return result;
}


//sets up widgets that fall inside the given rectangle to be updated
void awe_set_widget_dirty_rect(AWE_WIDGET *wgt, AWE_RECT *r)
{
    AWE_RECT t;

    if (!wgt->on_screen) return;
    awe_map_rect(wgt, 0, r, &t);
    _set_redraw(wgt, &t);
    _set_update_redraw(wgt);
    wgt->repaint = 1;
}


//sets up the given widget to be updated the next time the GUI is updated
void awe_set_widget_dirty(AWE_WIDGET *wgt)
{
    if (!wgt->on_screen) return;
    _set_update_redraw(wgt);
    wgt->repaint = 1;
}


//inserts a widget as a child into another widget
int awe_insert_widget(AWE_WIDGET *parent, AWE_WIDGET *child, int z_order)
{
    int result;
    _DO_R(parent, insert_widget, (parent, child, z_order), result, 0);
    return result;
}


//same as 'insert', only that it places the child widget at z == 0
int awe_add_widget(AWE_WIDGET *parent, AWE_WIDGET *child)
{
    return awe_insert_widget(parent, child, 0);
}


//removes a widget from its parent
int awe_remove_widget(AWE_WIDGET *wgt)
{
    int result;

    if (!wgt->parent) return 0;
    _DO_R(wgt->parent, remove_widget, (wgt->parent, wgt), result, 0);
    return result;
}


//sets a widget's z-order
void awe_set_widget_z_order(AWE_WIDGET *wgt, int z_order)
{
    int prev_z;

    //widget must be child
    if (!wgt->parent) return;

    //get current z-order; check for change
    prev_z = awe_get_widget_z_order(wgt);
    if (prev_z == z_order) return;

    //set z-order
    awe_list_remove(&wgt->parent->children, &wgt->node.node);
    _insert_widget(wgt->parent, wgt, z_order);

    //check for change
    z_order = awe_get_widget_z_order(wgt);
    if (prev_z == z_order) return;

    //update the screen
    if (wgt->on_screen) {
        //set parent to be redrawn
        if (wgt->drawable) {
            _set_update_redraw(wgt->parent);
            _set_redraw(wgt->parent, &wgt->clip);
        }
        
        //notify the widget
        _DO(wgt, z_order_changed, (wgt, z_order));
    }
}


//maps a series of points from the coordinate system of one widget to the other
void awe_map_points(AWE_WIDGET *src, AWE_WIDGET *dst, int num_points, int src_points[], int dst_points[])
{
    int total = num_points * 2, i;

    if (src && src->on_screen) {
        for(i = 0; i < total; i += 2) {
            dst_points[i  ] = src_points[i  ] + src->pos.left;
            dst_points[i+1] = src_points[i+1] + src->pos.top ;
        }
    }
    else if (src_points != dst_points) 
        memcpy(dst_points, src_points, total * sizeof(int));

    if (dst && dst->on_screen) {
        for(i = 0; i < total; i += 2) {
            dst_points[i  ] -= dst->pos.left;
            dst_points[i+1] -= dst->pos.top ;
        }
    }
}


//maps a single point from the coordinate system of one widget to the other
void awe_map_point(AWE_WIDGET *src, AWE_WIDGET *dst, int in_x, int in_y, int *out_x, int *out_y)
{
    *out_x = in_x;
    *out_y = in_y;

    if (src && src->on_screen) {
        *out_x += src->pos.left;
        *out_y += src->pos.top;
    }

    if (dst && dst->on_screen) {
        *out_x -= dst->pos.left;
        *out_y -= dst->pos.top;
    }
}


//maps a rectangle from the coordinate system of one widget to the other
void awe_map_rect(AWE_WIDGET *src, AWE_WIDGET *dst, AWE_RECT *in_r, AWE_RECT *out_r)
{
    if (src && src->on_screen) {
        out_r->left   = in_r->left   + src->pos.left;
        out_r->top    = in_r->top    + src->pos.top ;
        out_r->right  = in_r->right  + src->pos.left;
        out_r->bottom = in_r->bottom + src->pos.top ;
    }
    else *out_r = *in_r;

    if (dst && dst->on_screen) {
        out_r->left   -= dst->pos.left;
        out_r->top    -= dst->pos.top ;
        out_r->right  -= dst->pos.left;
        out_r->bottom -= dst->pos.top ;
    }
}


//sets up the GUI to be dirty
void awe_set_gui_dirty_rect(AWE_RECT *r)
{
    AWE_RECT t;

    if (!_root_widget) return;
    awe_map_rect(0, _root_widget, r, &t);
    awe_set_widget_dirty_rect(_root_widget, &t);
}


//sets up the whole GUI to be updated
void awe_set_gui_dirty()
{
    if (_root_widget) awe_set_widget_dirty(_root_widget);
}


//returns the GUI screen, ie the screen to be used for drawing the GUI.
BITMAP *awe_get_gui_screen()
{
    return _gui_screen;
}


//sets the GUI screen
void awe_set_gui_screen(BITMAP *bmp)
{
    _gui_screen = bmp;
}


//returns the gui's update mode.
AWE_GUI_UPDATE_MODE awe_get_gui_update_mode()
{
    return _gui_update_mode;
}


//sets the gui's update mode
void awe_set_gui_update_mode(AWE_GUI_UPDATE_MODE mode, AWE_DRS_PROC drs_proc)
{
    switch (mode) {
        case AWE_GUI_UPDATE_FULL:
            _gui_update_mode = mode;
            _gui_update_proc = _gui_update_full;
            break;

        default:
            _gui_update_mode = AWE_GUI_UPDATE_CHANGES;
            _gui_update_proc = _gui_update_changes;
    }
    _gui_update_drs_proc = drs_proc;
}


//updates the GUI
void awe_update_gui()
{
    if (_root_widget) _gui_update_proc();
}


//returns the default output type for the widgets
AWE_WIDGET_OUTPUT_TYPE awe_get_widget_output_type()
{
    return _widget_output_type;
}


//sets the default output type of the widgets
void awe_set_widget_output_type(AWE_WIDGET_OUTPUT_TYPE type)
{
    switch (type) {
        case AWE_WIDGET_OUTPUT_VIDEO_BITMAP:
        case AWE_WIDGET_OUTPUT_MEMORY_BITMAP:
            _widget_output_type = type;
            break;

        default:
            _widget_output_type = AWE_WIDGET_OUTPUT_DIRECT;
    }
}


//sets or resets a widget timer
int awe_add_widget_timer(AWE_WIDGET *wgt, int id, int msecs)
{
    return awe_add_timer(wgt, id, msecs);
}


//removes a widget timer
int awe_remove_widget_timer(AWE_WIDGET *wgt, int id)
{
    return awe_remove_timer(wgt, id);
}


//special function for setting a widget's geometry directly
int awe_override_widget_geometry(AWE_WIDGET *wgt, int x, int y, int width, int height)
{
    if (wgt->x == x && wgt->y == y && wgt->width == width && wgt->height == height) return 0;
    wgt->x = x;
    wgt->y = y;
    wgt->width = width;
    wgt->height = height;
    if (wgt->on_screen) {
        if (wgt->object.setting_properties) wgt->geometry_changed = 1;
        else _set_update_geometry(wgt);
    }
    return 1;
}


/** same as 'awe_override_widget_geometry', but for position only
    @param wgt widget to set the geometry of
    @param x left coordinate relative to parent (or to screen if root)
    @param y top coordinate relative to parent (or to screen if root)
    @return zero if supplied geometry is the same as the current one; used
            in knowing when a widget's geometry is actually changed
 */
int awe_override_widget_position(AWE_WIDGET *wgt, int x, int y)
{
    return awe_override_widget_geometry(wgt, x, y, wgt->width, wgt->height);
}


/** same as 'awe_override_widget_geometry', but for size only
    @param wgt widget to set the geometry of
    @param width width of widget
    @param height height of widget
    @return zero if supplied geometry is the same as the current one; used
            in knowing when a widget's geometry is actually changed
 */
int awe_override_widget_size(AWE_WIDGET *wgt, int width, int height)
{
    return awe_override_widget_geometry(wgt, wgt->x, wgt->y, width, height);
}
