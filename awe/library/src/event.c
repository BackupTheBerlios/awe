#include "event.h"


/*****************************************************************************
    PRIVATE
 *****************************************************************************/


//call a widget method
#define _DO(WGT, METHOD, PARAMS)\
    AWE_CALL_METHOD(WGT, AWE_ID_WIDGET, AWE_ID_AWE, AWE_WIDGET_VTABLE, METHOD, PARAMS)


//does a mouse event; takes care of mapping event coordinates to widget space
#define _DO_MOUSE_EVENT(WGT, METHOD, EVENT) {\
    if (WGT) {\
        int x = EVENT->mouse.x, y = EVENT->mouse.y, tx, ty;\
        awe_map_point(0, WGT, x, y, &tx, &ty);\
        EVENT->mouse.x = tx;\
        EVENT->mouse.y = ty;\
        _DO(WGT, METHOD, (WGT, EVENT));\
        EVENT->mouse.x = x;\
        EVENT->mouse.y = y;\
    }\
}


//event proc
typedef struct _EVENT_PROC {
    AWE_DL_NODE node;
    AWE_EVENT_PROC proc;
    void *data;
} _EVENT_PROC;


//event mode
typedef struct _EVENT_MODE {
    AWE_DL_NODE node;
    AWE_DL_LIST procs;
    int processed:1;
    int removed:1;
} _EVENT_MODE;


//variables
static AWE_DL_LIST _event_mode_stack = {0, 0};
static int _last_x = INT_MIN;
static int _last_y = INT_MIN;


//installs the stuff
static void _install()
{
    if (_event_mode_stack.first) return;
    awe_enter_event_mode(awe_def_event_proc, 0);
}


//finds an event proc
static _EVENT_PROC *_find_event_proc(_EVENT_MODE *mode, AWE_EVENT_PROC proc_, void *data)
{
    _EVENT_PROC *proc = (_EVENT_PROC *)mode->procs.first;

    for(; proc; proc = (_EVENT_PROC *)proc->node.next) {
        if (proc->proc == proc_ && proc->data == data) return proc;
    }
    return 0;
}


//deletes an event mode
static void _delete_event_mode(_EVENT_MODE *mode)
{
    AWE_DL_NODE *node, *next;

    //call all procs
    for(node = mode->procs.first; node; node = node->next) {
        ((_EVENT_PROC *)node)->proc(AWE_EVENT_MODE_ACTION_END, 0, ((_EVENT_PROC *)node)->data);
    }

    //free procs
    node = mode->procs.first;
    while (node) {
        next = node->next;
        free(node);
        node = next;
    }

    //remove and free mode
    awe_list_remove(&_event_mode_stack, &mode->node);
    free(mode);
}


//finds an enabled widget from screen coordinates
static INLINE AWE_WIDGET *_widget_from_point(AWE_WIDGET *wgt, int x, int y)
{
    AWE_WIDGET *result;

    awe_map_point(0, wgt, x, y, &x, &y);
    result = awe_get_widget_from_point(wgt, x, y);
    return result && awe_is_enabled_widget_tree(result) ? result : 0;
}


//checks if a widget includes a screen point
static INLINE int _widget_includes_point(AWE_WIDGET *wgt, int x, int y)
{
    awe_map_point(0, wgt, x, y, &x, &y);
    return awe_widget_includes_point(wgt, x, y);
}


//does a timer event
static int _do_timer_event(AWE_EVENT *event)
{
    AWE_WIDGET *wgt = (AWE_WIDGET *)event->timer.data;
    if (!wgt) return 0;
    _DO(wgt, timer, (wgt, event));
    return 1;
}


//does an event, starting from given 'root'
static int _do_event(AWE_WIDGET *root, AWE_EVENT *event, int force)
{
    AWE_WIDGET *curr, *prev, *focus;

    switch (event->type) {
        //button down event
        case AWE_EVENT_MOUSE_DOWN:
            curr = _widget_from_point(root, event->mouse.x, event->mouse.y);
            if (!curr) {
                if (!force) return 0;
                curr = root;
            }
            _DO_MOUSE_EVENT(curr, button_down, event);
            _last_x = event->mouse.x;
            _last_y = event->mouse.y;
            return 1;

        //button up event
        case AWE_EVENT_MOUSE_UP:
            curr = _widget_from_point(root, event->mouse.x, event->mouse.y);
            if (!curr) {
                if (!force) return 0;
                curr = root;
            }
            _DO_MOUSE_EVENT(curr, button_up, event);
            _last_x = event->mouse.x;
            _last_y = event->mouse.y;
            return 0;

        //mouse moved event
        case AWE_EVENT_MOUSE_MOVE:
            prev = _widget_from_point(root, _last_x, _last_y);
            curr = _widget_from_point(root, event->mouse.x, event->mouse.y);
            if (prev == 0 && curr == 0) {
                if (!force) {
                    _last_x = event->mouse.x;
                    _last_y = event->mouse.y;
                    return 0;
                }
                curr = prev = root;
            }
            if (prev != curr) {
                _DO_MOUSE_EVENT(prev, mouse_leave, event);
                _DO_MOUSE_EVENT(curr, mouse_enter, event);
            }
            else _DO_MOUSE_EVENT(curr, mouse_move, event);
            _last_x = event->mouse.x;
            _last_y = event->mouse.y;
            return 1;

        //mouse wheel event
        case AWE_EVENT_MOUSE_WHEEL:
            focus = awe_get_focus_widget();
            if (!focus) {
                if (!force) return 0;
                focus = root;
            }
            _DO_MOUSE_EVENT(focus, mouse_wheel, event);
            _last_x = event->mouse.x;
            _last_y = event->mouse.y;
            return 1;

        //key down
        case AWE_EVENT_KEY_DOWN:
            focus = awe_get_focus_widget();
            if (!focus) {
                if (!force) return 0;
                focus = root;
            }
            _DO(focus, key_down, (focus, event));
            return 1;

        //key press
        case AWE_EVENT_KEY_UP:
            focus = awe_get_focus_widget();
            if (!focus) {
                if (!force) return 0;
                focus = root;
            }
            _DO(focus, key_up, (focus, event));
            return 1;

        //timer event
        case AWE_EVENT_TIMER:
            return _do_timer_event(event);
    }

    return 0;
}


/*****************************************************************************
    PUBLIC
 *****************************************************************************/


//registers an event procedure in the current event mode
void awe_add_event_proc(AWE_EVENT_PROC proc_, void *data)
{
    _EVENT_MODE *mode;
    _EVENT_PROC *proc;

    //install first mode, if needed
    _install();

    //do not re-install proc if it already exists
    mode = (_EVENT_MODE *)_event_mode_stack.last;
    proc = _find_event_proc(mode, proc_, data);
    if (proc) return;

    //install new proc
    proc = (_EVENT_PROC *)calloc(1, sizeof(_EVENT_PROC));
    if (!proc) return;
    awe_list_insert(&mode->procs, &proc->node, 0);
    proc->proc = proc_;
    proc->data = data;

    //notify proc
    proc_(AWE_EVENT_MODE_ACTION_BEGIN, 0, data);
}


//replaces an event procedure with another event procedure
void awe_replace_event_proc(AWE_EVENT_PROC new_proc, AWE_EVENT_PROC old_proc)
{
    _EVENT_MODE *mode;
    _EVENT_PROC *proc;

    //get current mode
    mode = (_EVENT_MODE *)_event_mode_stack.last;
    if (!mode) return;

    //set proc
    for(proc = (_EVENT_PROC *)mode->procs.first; proc; proc = (_EVENT_PROC *)proc->node.next) {
        if (proc->proc == old_proc) {
            proc->proc(AWE_EVENT_MODE_ACTION_END, 0, proc->data);
            proc->proc = new_proc;
            proc->proc(AWE_EVENT_MODE_ACTION_BEGIN, 0, proc->data);
        }
    }
}


//unregisters an event procedure from the current event mode
void awe_remove_event_proc(AWE_EVENT_PROC proc_, void *data)
{
    _EVENT_MODE *mode;
    _EVENT_PROC *proc;

    //get current mode
    mode = (_EVENT_MODE *)_event_mode_stack.last;
    if (!mode) return;

    //find proc
    proc = _find_event_proc(mode, proc_, data);
    if (!proc) return;

    //remove proc
    awe_list_remove(&mode->procs, &proc->node);
    free(proc);

    //notify proc
    proc_(AWE_EVENT_MODE_ACTION_END, 0, data);
}


//enters a new event mode
void awe_enter_event_mode(AWE_EVENT_PROC proc, void *data)
{
    _EVENT_MODE *mode;

    //create a mode
    mode = (_EVENT_MODE *)calloc(1, sizeof(_EVENT_MODE));
    if (!mode) return;

    //add mode and register the given proc
    awe_list_insert(&_event_mode_stack, &mode->node, 0);
    if (proc) awe_add_event_proc(proc, data);
}


//leaves the current event mode
void awe_leave_event_mode()
{
    _EVENT_MODE *mode = (_EVENT_MODE *)_event_mode_stack.last;

    //make sure the first mode is not removed
    if (!mode || !mode->node.prev) return;

    //free the mode (later if it is being processed)
    if (!mode->processed) _delete_event_mode(mode); else mode->removed = 1;
}


//does events
void awe_do_events()
{
    _EVENT_MODE *mode;
    _EVENT_PROC *proc;
    AWE_EVENT event;

    //install first mode, if needed
    _install();

    //get input event
    if (!awe_get_event(&event)) return;

    //call all procedures of current mode
    mode = (_EVENT_MODE *)_event_mode_stack.last;
    if (!mode) return;
    mode->processed = 1;
    for(proc = (_EVENT_PROC *)mode->procs.first; proc; proc = (_EVENT_PROC *)proc->node.next) {
        if (proc->proc(AWE_EVENT_MODE_ACTION_DO, &event, proc->data)) break;
    }
    mode->processed = 0;
    if (mode->removed) _delete_event_mode(mode);
}


//event manager procedure that does normal event handling
int awe_def_event_proc(AWE_EVENT_MODE_ACTION_TYPE action, AWE_EVENT *event, void *data)
{
    AWE_WIDGET *root;

    //only the 'do event' action is interesting
    if (!event) return 0;

    //check if there is a gui installed
    root = awe_get_root_widget();
    if (!root) return 0;

    //do event
    return _do_event(root, event, 0);
}


//grab event procedure
int awe_grab_event_proc(AWE_EVENT_MODE_ACTION_TYPE action, AWE_EVENT *event, void *data)
{
    AWE_WIDGET *wgt;
    int p, c;

    //only the 'do event' action is interesting
    if (!event) return 0;

    //'grab widget' is stored in 'data'
    wgt = (AWE_WIDGET *)data;
    if (!wgt) return 0;

    switch (event->type) {
        //button down event
        case AWE_EVENT_MOUSE_DOWN:
            _DO_MOUSE_EVENT(wgt, button_down, event);
            _last_x = event->mouse.x;
            _last_y = event->mouse.y;
            return 1;

        //button up event
        case AWE_EVENT_MOUSE_UP:
            _DO_MOUSE_EVENT(wgt, button_up, event);
            _last_x = event->mouse.x;
            _last_y = event->mouse.y;
            return 1;

        //mouse moved event
        case AWE_EVENT_MOUSE_MOVE:
            p = _widget_includes_point(wgt, _last_x, _last_y);
            c = _widget_includes_point(wgt, event->mouse.x, event->mouse.y);
            if (p == c) _DO_MOUSE_EVENT(wgt, mouse_move, event)
            else if (c) _DO_MOUSE_EVENT(wgt, mouse_enter, event)
            else _DO_MOUSE_EVENT(wgt, mouse_leave, event);
            _last_x = event->mouse.x;
            _last_y = event->mouse.y;
            return 1;

        //mouse wheel event
        case AWE_EVENT_MOUSE_WHEEL:
            _DO_MOUSE_EVENT(wgt, mouse_wheel, event);
            _last_x = event->mouse.x;
            _last_y = event->mouse.y;
            return 1;

        //key down
        case AWE_EVENT_KEY_DOWN:
            _DO(wgt, key_down, (wgt, event));
            return 1;

        //key press
        case AWE_EVENT_KEY_UP:
            _DO(wgt, key_up, (wgt, event));
            return 1;

        //timer event
        case AWE_EVENT_TIMER:
            return _do_timer_event(event);
    }

    return 0;
}


//modal event procedure
int awe_modal_event_proc(AWE_EVENT_MODE_ACTION_TYPE action, AWE_EVENT *event, void *data)
{
    //only the 'do event' action is interesting
    if (!event) return 0;

    //check if there is a modal widget
    if (!data) return 0;

    //do event
    return _do_event((AWE_WIDGET *)data, event, 1);
}
