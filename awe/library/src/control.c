#include "control.h"


/*****************************************************************************
    PRIVATE
 *****************************************************************************/


//dispatches the control.key_press message to all descentants of given widget
static int _dispatch_control_key_press(AWE_WIDGET *wgt, const AWE_EVENT *event)
{
    AWE_CONTROL_VTABLE *control;
    AWE_WIDGET *child;

    for(child = awe_get_first_child_widget(wgt);
        child;
        child = awe_get_next_sibling_widget(child)) {
        control = (AWE_CONTROL_VTABLE *)awe_get_widget_interface(child, 
            AWE_ID_CONTROL,
            AWE_ID_AWE);
        if (control && control->key_press(child, event)) return 1;
        if (_dispatch_control_key_press(child, event)) return 1;
    }
    return 0;
}


//returns topmost descentant
static AWE_WIDGET *_get_top(AWE_WIDGET *wgt)
{
    while (1) {
        AWE_WIDGET *child = awe_get_last_child_widget(wgt);
        if (!child) return wgt;
        wgt = child;
    }
    return wgt;
}


//returns the previous widget as if widgets were a flat list
static AWE_WIDGET *_get_prev(AWE_WIDGET *wgt)
{
    AWE_WIDGET *prev = awe_get_prev_sibling_widget(wgt);
    if (prev) return _get_top(prev);
    return awe_get_parent_widget(wgt);
}


//returns the next widget as if widgets were a flat list
static AWE_WIDGET *_get_next(AWE_WIDGET *wgt)
{
    AWE_WIDGET *t = awe_get_first_child_widget(wgt);
    if (t) return t;
    for(; wgt; wgt = awe_get_parent_widget(wgt)) {
        t = awe_get_next_sibling_widget(wgt);
        if (t) return t;
    }
    return 0;
}


//actual moving of the focus; the widget tree is flattened
static void _move_focus(AWE_WIDGET *begin,
                        AWE_WIDGET *end, 
                        AWE_WIDGET *restart,
                        AWE_WIDGET *(*next)(AWE_WIDGET *))
{
    AWE_WIDGET *wgt;

    //try until end
    for(wgt = begin; wgt != end; wgt = next(wgt)) {
        if (awe_set_focus_widget(wgt)) return;
    }

    //recycle
    for(wgt = restart; wgt != begin; wgt = next(wgt)) {
        if (awe_set_focus_widget(wgt)) return;
    }
}


//moves the dialog focus
static void _move_dialog_focus(AWE_WIDGET *wgt, const AWE_EVENT *event)
{
    AWE_WIDGET *focus_wgt, *begin, *end, *restart;
    AWE_WIDGET *(*next_proc)(AWE_WIDGET *) = 0;

    //find direction to move the focus
    switch (event->key.scancode) {
        case KEY_LEFT:
        case KEY_UP:
            next_proc = _get_prev;
            break;

        case KEY_RIGHT:
        case KEY_DOWN:
            next_proc = _get_next;
            break;

        case KEY_TAB:
            next_proc = (event->key.shifts & KB_SHIFT_FLAG) ? _get_prev : _get_next;
            break;

        default:
            return;
    }
    
    /* move the focus away from the current focus widget; if the focus not
       inside the dialog, then move the focus as if no focus widget exists.
     */
    focus_wgt = awe_get_focus_widget();
    if (focus_wgt && !awe_is_ancestor_widget(wgt, focus_wgt)) focus_wgt = 0;

    //move the focus according to selected direction
    if (next_proc == _get_next) {
        restart = wgt;
        begin = focus_wgt ? _get_next(focus_wgt) : wgt;
        end = _get_next(_get_top(wgt));
    }
    else {
        restart = _get_top(wgt);
        begin = focus_wgt ? _get_prev(focus_wgt) : restart;
        end = _get_prev(wgt);
    }
    _move_focus(begin, end, restart, next_proc);
}


/*****************************************************************************
    PUBLIC
 *****************************************************************************/


//a dialog's key down implementation
void awe_dialog_key_down(AWE_WIDGET *wgt, const AWE_EVENT *event)
{
    if (_dispatch_control_key_press(wgt, event)) return;
    _move_dialog_focus(wgt, event);
}
