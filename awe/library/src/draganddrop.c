#include "draganddrop.h"


/*****************************************************************************
    PRIVATE
 *****************************************************************************/


//call a widget method
#define _DO(WGT, NAME, STRUCT, METHOD, PARAMS)\
    AWE_CALL_METHOD(WGT, NAME, AWE_ID_AWE, STRUCT, METHOD, PARAMS)


//does a mouse event; takes care of mapping event coordinates to widget space
#define _DO_MOUSE_EVENT(WGT, NAME, STRUCT, METHOD, EVENT, DATA) {\
    if (WGT) {\
        int x = EVENT->mouse.x, y = EVENT->mouse.y;\
        int tx, ty;\
        awe_map_point(0, WGT, x, y, &tx, &ty);\
        EVENT->mouse.x = tx;\
        EVENT->mouse.y = ty;\
        _DO(WGT, NAME, STRUCT, METHOD, (WGT, EVENT, DATA));\
        EVENT->mouse.x = x;\
        EVENT->mouse.y = y;\
    }\
}


//call a widget drag-n-drop target method
#define _DO_TARGET(WGT, METHOD, PARAMS)\
    _DO(WGT, AWE_ID_DRAG_AND_DROP_TARGET, AWE_DRAG_AND_DROP_TARGET_VTABLE, METHOD, PARAMS)


//does a mouse event; takes care of mapping event coordinates to widget space
#define _DO_MOUSE_EVENT_TARGET(WGT, METHOD, EVENT, DATA)\
    _DO_MOUSE_EVENT(WGT, AWE_ID_DRAG_AND_DROP_TARGET, AWE_DRAG_AND_DROP_TARGET_VTABLE, METHOD, EVENT, DATA)


//call a widget drag-n-drop source method
#define _DO_SOURCE(WGT, METHOD, PARAMS)\
    _DO(WGT, AWE_ID_DRAG_AND_DROP_SOURCE, AWE_DRAG_AND_DROP_SOURCE_VTABLE, METHOD, PARAMS)


//does a mouse event; takes care of mapping event coordinates to widget space
#define _DO_MOUSE_EVENT_SOURCE(WGT, METHOD, EVENT, DATA)\
    _DO_MOUSE_EVENT(WGT, AWE_ID_DRAG_AND_DROP_SOURCE, AWE_DRAG_AND_DROP_SOURCE_VTABLE, METHOD, EVENT, DATA)


//propagate event to closest ancestor
#define _PROPAGATE_EVENT(WGT, EVENT, DATA, METHOD)\
    AWE_WIDGET *parent = awe_get_parent_widget(WGT);\
    if (parent) {\
        parent = _get_drag_and_drop_target(parent);\
        if (parent) _DO_TARGET(parent, METHOD, (parent, EVENT, DATA));\
    }


//variables
static int _dnd = 0;
static AWE_WIDGET *_source = 0;
static AWE_OBJECT *_data = 0;
static int _last_x = 0;
static int _last_y = 0;


//finds an enabled widget from screen coordinates
static INLINE AWE_WIDGET *_widget_from_point(AWE_WIDGET *wgt, int x, int y)
{
    AWE_WIDGET *result;

    awe_map_point(0, wgt, x, y, &x, &y);
    result = awe_get_widget_from_point(wgt, x, y);
    return result && awe_is_enabled_widget_tree(result) ? result : 0;
}


//returns the closest widget that implements the target interface
static AWE_WIDGET *_get_drag_and_drop_target(AWE_WIDGET *wgt)
{
    for(; wgt; wgt = awe_get_parent_widget(wgt)) {
        if (awe_get_widget_interface(wgt, AWE_ID_DRAG_AND_DROP_TARGET, AWE_ID_AWE))
            return wgt;
    }
    return 0;
}


/*****************************************************************************
    PUBLIC
 *****************************************************************************/


//sends the event to the closest drag-and-drop ancestor
void awe_drag_and_drop_target_button_up(AWE_WIDGET *wgt, const AWE_EVENT *event, AWE_OBJECT *data)
{
    _PROPAGATE_EVENT(wgt, event, data, button_up);
}


//sends the event to the closest drag-and-drop ancestor
void awe_drag_and_drop_target_mouse_enter(AWE_WIDGET *wgt, const AWE_EVENT *event, AWE_OBJECT *data)
{
    _PROPAGATE_EVENT(wgt, event, data, mouse_enter);
}


//sends the event to the closest drag-and-drop ancestor
void awe_drag_and_drop_target_mouse_move(AWE_WIDGET *wgt, const AWE_EVENT *event, AWE_OBJECT *data)
{
    _PROPAGATE_EVENT(wgt, event, data, mouse_move);
}


//sends the event to the closest drag-and-drop ancestor
void awe_drag_and_drop_target_mouse_leave(AWE_WIDGET *wgt, const AWE_EVENT *event, AWE_OBJECT *data)
{
    _PROPAGATE_EVENT(wgt, event, data, mouse_leave);
}


//sends the event to the closest drag-and-drop ancestor
void awe_drag_and_drop_target_mouse_wheel(AWE_WIDGET *wgt, const AWE_EVENT *event, AWE_OBJECT *data)
{
    _PROPAGATE_EVENT(wgt, event, data, mouse_wheel);
}


//sends the event to the closest drag-and-drop ancestor
void awe_drag_and_drop_target_key_down(AWE_WIDGET *wgt, const AWE_EVENT *event, AWE_OBJECT *data)
{
    _PROPAGATE_EVENT(wgt, event, data, key_down);
}


//sends the event to the closest drag-and-drop ancestor
void awe_drag_and_drop_target_key_up(AWE_WIDGET *wgt, const AWE_EVENT *event, AWE_OBJECT *data)
{
    _PROPAGATE_EVENT(wgt, event, data, key_up);
}


//drag-and-drop event procedure
int awe_drag_and_drop_event_proc(AWE_EVENT_MODE_ACTION_TYPE action, AWE_EVENT *event, void *data)
{
    AWE_WIDGET *root, *wgt, *prev;

    //handle other actions
    switch (action) {
        case AWE_EVENT_MODE_ACTION_BEGIN:
            return 0;

        case AWE_EVENT_MODE_ACTION_END:
            awe_end_drag_and_drop();
            return 0;

        //avoid GCC warning
        default:
            break;
    }

    //must have root because events will be 'played' from root
    root = awe_get_root_widget();
    if (!root) return 0;

    //send event to widgets
    switch (event->type) {
        //mouse down not handled (drag-n-drop starts from mouse down)
        case AWE_EVENT_BUTTON_DOWN:
            return 0;

        //event is sent to the widget under mouse; drag-and-drop ends
        case AWE_EVENT_BUTTON_UP:
            wgt = _widget_from_point(root, event->mouse.x, event->mouse.y);
            if (wgt) wgt = _get_drag_and_drop_target(wgt);
            _DO_MOUSE_EVENT_TARGET(wgt, button_up, event, data);
            awe_end_drag_and_drop();
            return 1;

        //events are sent to the widgets under mouse
        case AWE_EVENT_MOUSE_MOVE:
            prev = _widget_from_point(root, _last_x, _last_y);
            if (prev) prev = _get_drag_and_drop_target(prev);
            wgt = _widget_from_point(root, event->mouse.x, event->mouse.y);
            if (wgt) wgt = _get_drag_and_drop_target(wgt);
            if (prev != wgt) {
                _DO_MOUSE_EVENT_TARGET(prev, mouse_leave, event, data);
                _DO_MOUSE_EVENT_TARGET(wgt, mouse_enter, event, data);
            }
            else {
                _DO_MOUSE_EVENT_TARGET(wgt, mouse_move, event, data);
            }
            _last_x = event->mouse.x;
            _last_y = event->mouse.y;

            //call the source widget
            _DO_MOUSE_EVENT_SOURCE(_source, mouse_move, event, data);

            return 1;

        //event is sent to the widget under mouse
        case AWE_EVENT_MOUSE_WHEEL:
            wgt = _widget_from_point(root, event->mouse.x, event->mouse.y);
            if (wgt) wgt = _get_drag_and_drop_target(wgt);
            _DO_MOUSE_EVENT_TARGET(wgt, mouse_wheel, event, data);
            return 1;

        //event is sent to the widget under mouse
        case AWE_EVENT_KEY_DOWN:
            wgt = _widget_from_point(root, _last_x, _last_y);
            if (wgt) wgt = _get_drag_and_drop_target(wgt);
            if (wgt) _DO_TARGET(wgt, key_down, (wgt, event, data));
            return 1;

        //event is sent to the widget under mouse
        case AWE_EVENT_KEY_UP:
            wgt = _widget_from_point(root, _last_x, _last_y);
            if (wgt) wgt = _get_drag_and_drop_target(wgt);
            if (wgt) _DO_TARGET(wgt, key_up, (wgt, event, data));
            return 1;

        //event is sent to the widget it belongs to
        case AWE_EVENT_TIMER:
            return awe_def_event_proc(action, event, data);

        //avoid GCC warnings
        default:
            break;
    }

    return 0;
}


//begins a drag-and-drop session
int awe_begin_drag_and_drop(AWE_WIDGET *source, AWE_OBJECT *data)
{
    //don't start another drag-and-drop session
    if (_dnd) return 0;

    //init context
    _dnd = 1;
    _source = source;
    _data = data;

    //call the 'begin' method of the source
    AWE_CALL_METHOD(source, AWE_ID_DRAG_AND_DROP_SOURCE, AWE_ID_AWE, AWE_DRAG_AND_DROP_SOURCE_VTABLE, begin, (source));

    //set new event mode
    awe_enter_event_mode(awe_drag_and_drop_event_proc, data);

    //successful begin
    return 1;
}


//ends a drag-and-drop session
int awe_end_drag_and_drop()
{
    //if drag-and-drop not started yet, don't do anything
    if (!_dnd) return 0;

    //notify the source object
    AWE_CALL_METHOD(_source, AWE_ID_DRAG_AND_DROP_SOURCE, AWE_ID_AWE, AWE_DRAG_AND_DROP_SOURCE_VTABLE, end, (_source));

    //delete resources
    awe_destroy_object(_data);

    //reset state
    _dnd = 0;
    _source = 0;
    _data = 0;
    _last_x = 0;
    _last_y = 0;

    //remove proc
    awe_leave_event_mode();

    //successful end
    return 1;
}


//sends the 'clear' message to the source widget
int awe_clear_drag_and_drop_source()
{
    if (!_dnd) return 0;
    AWE_CALL_METHOD(_source, AWE_ID_DRAG_AND_DROP_SOURCE, AWE_ID_AWE, AWE_DRAG_AND_DROP_SOURCE_VTABLE, clear, (_source));
    return 1;
}
