#ifndef AWE_DRAGANDDROP_H
#define AWE_DRAGANDDROP_H


#include "event.h"


#ifdef __cplusplus
    extern "C" {
#endif


/**@name Drag-And-Drop
    <p> The purpose of the drag-n-drop module is to provide the interfaces
        and functionality that is needed for drag-and-drop. The module
        implements drag-and-drop inside AWE widgets; it is not intergrated
        with the host window system (it might be intergrated in a future version,
        though).
    </p>
    <p> Widgets that wish to accept drag-n-drop events must implement the
        DRAG_AND_DROP_TARGET_VTABLE interface; it contains the relevant
        event methods that are called from the drag-and-drop event handler.
    </p>
    <p> Widgets that wish to provide drag-n-drop to other widgets (i.e. widgets
        that contain draggable content) should implement the DRAG_AND_DROP_SOURCE_VTABLE
        interface. It contains event methods relative to the drag-and-drop
        session, as well as notifications for the mouse actions.
    </p>
    <p> Drag-and-drop data are managed as COM objects, i.e. as AWE_OBJECT-derived
        instances. The source widget registers an AWE_OBJECT when the drag-and-drop
        starts. This object must contain the data that are to be dropped.
        Drag-and-drop targets can query the data object for support of a 
        specific interface, according to the data type they want. When the
        drag-and-drop session ends (there can be only one), the data object
        will be automatically deleted.
    </p>
    <p> Widgets should implement the following guidelines regarding drag-and-drop:
        <li> a drag-and-drop session must begin only from the button down event.            
        </li>
        <li> when no modifier key is held down, data are copied.
        </li>
        <li> data are moved only when the shift key is held down during a drop.
        </li>
    </p>
 */
/*@{*/


///id of target interface
#define AWE_ID_DRAG_AND_DROP_TARGET    "DragAndDropTarget"


///id of source interface
#define AWE_ID_DRAG_AND_DROP_SOURCE    "DragAndDropSource"


/** The drag-and-drop source widget interface.
 */
struct AWE_DRAG_AND_DROP_SOURCE_VTABLE {
    /** called when the drag-and-drop session begins; optional method
        @param wgt widget to be called
     */
    void (*begin)(AWE_WIDGET *wgt);

    /** called when the drag-and-drop session ends; optional method
        @param wgt widget to be called
     */
    void (*end)(AWE_WIDGET *wgt);

    /** called when the dragged data selection must be cleared (data must be deleted);
        optional method
        @param wgt widget to be called
     */
    void (*clear)(AWE_WIDGET *wgt);

    /** called when the mouse cursor moves over the widget; optional method
        @param wgt widget to be called 
        @param event input event
        @param data data object
     */
    void (*mouse_move)(AWE_WIDGET *wgt, const AWE_EVENT *event, AWE_OBJECT *data);
};
typedef struct AWE_DRAG_AND_DROP_SOURCE_VTABLE AWE_DRAG_AND_DROP_SOURCE_VTABLE;


/** the drag-and-drop target widget interface.
 */
struct AWE_DRAG_AND_DROP_TARGET_VTABLE {
    /** called when data are dropped onto the widget. Optional method;
        the default implementation sends the event to the first ancestor that
        implements the drag-and-drop target interface.
        @param wgt widget to be called
        @param event input event
        @param data data object
     */
    void (*button_up)(AWE_WIDGET *wgt, const AWE_EVENT *event, AWE_OBJECT *data);

    /** called when the mouse cursor enters the widget. Optional method;
        the default implementation sends the event to the first ancestor that
        implements the drag-and-drop target interface.
        @param wgt widget to be called 
        @param event input event
        @param data data object
     */
    void (*mouse_enter)(AWE_WIDGET *wgt, const AWE_EVENT *event, AWE_OBJECT *data);

    /** called when the mouse cursor moves over the widget. Optional method;
        the default implementation sends the event to the first ancestor that
        implements the drag-and-drop target interface.
        @param wgt widget to be called 
        @param event input event
        @param data data object
     */
    void (*mouse_move)(AWE_WIDGET *wgt, const AWE_EVENT *event, AWE_OBJECT *data);

    /** called when the mouse cursor leaves the widget. Optional method;
        the default implementation sends the event to the first ancestor that
        implements the drag-and-drop target interface.
        @param wgt widget to be called 
        @param event input event
        @param data data object
     */
    void (*mouse_leave)(AWE_WIDGET *wgt, const AWE_EVENT *event, AWE_OBJECT *data);

    /** called when the mouse wheel is rolled over the widget. Optional method;
        the default implementation sends the event to the first ancestor that
        implements the drag-and-drop target interface.
        @param wgt widget to be called 
        @param event input event
        @param data data object
     */
    void (*mouse_wheel)(AWE_WIDGET *wgt, const AWE_EVENT *event, AWE_OBJECT *data);

    /** called when a key is pressed. Optional method;
        the default implementation sends the event to the first ancestor that
        implements the drag-and-drop target interface.
        @param wgt widget to be called 
        @param event input event
        @param data data object
     */
    void (*key_down)(AWE_WIDGET *wgt, const AWE_EVENT *event, AWE_OBJECT *data);

    /** called when a key is released. Optional method;
        the default implementation sends the event to the first ancestor that
        implements the drag-and-drop target interface.
        @param wgt widget to be called 
        @param event input event
        @param data data object
     */
    void (*key_up)(AWE_WIDGET *wgt, const AWE_EVENT *event, AWE_OBJECT *data);
};
typedef struct AWE_DRAG_AND_DROP_TARGET_VTABLE AWE_DRAG_AND_DROP_TARGET_VTABLE;


/** the default implementation of the relevant drag-and-drop target event;
    it propagates the event to the closest ancestor that implements the
    drag-and-drop target interface
    @param wgt widget to call
    @param event input event
    @param managed data
 */
void awe_drag_and_drop_target_button_up(AWE_WIDGET *wgt, const AWE_EVENT *event, AWE_OBJECT *data);


/** the default implementation of the relevant drag-and-drop target event;
    it propagates the event to the closest ancestor that implements the
    drag-and-drop target interface
    @param wgt widget to call
    @param event input event
    @param managed data
 */
void awe_drag_and_drop_target_mouse_enter(AWE_WIDGET *wgt, const AWE_EVENT *event, AWE_OBJECT *data);


/** the default implementation of the relevant drag-and-drop target event;
    it propagates the event to the closest ancestor that implements the
    drag-and-drop target interface
    @param wgt widget to call
    @param event input event
    @param managed data
 */
void awe_drag_and_drop_target_mouse_move(AWE_WIDGET *wgt, const AWE_EVENT *event, AWE_OBJECT *data);


/** the default implementation of the relevant drag-and-drop target event;
    it propagates the event to the closest ancestor that implements the
    drag-and-drop target interface
    @param wgt widget to call
    @param event input event
    @param managed data
 */
void awe_drag_and_drop_target_mouse_leave(AWE_WIDGET *wgt, const AWE_EVENT *event, AWE_OBJECT *data);


/** the default implementation of the relevant drag-and-drop target event;
    it propagates the event to the closest ancestor that implements the
    drag-and-drop target interface
    @param wgt widget to call
    @param event input event
    @param managed data
 */
void awe_drag_and_drop_target_mouse_wheel(AWE_WIDGET *wgt, const AWE_EVENT *event, AWE_OBJECT *data);


/** the default implementation of the relevant drag-and-drop target event;
    it propagates the event to the closest ancestor that implements the
    drag-and-drop target interface
    @param wgt widget to call
    @param event input event
    @param managed data
 */
void awe_drag_and_drop_target_key_down(AWE_WIDGET *wgt, const AWE_EVENT *event, AWE_OBJECT *data);


/** the default implementation of the relevant drag-and-drop target event;
    it propagates the event to the closest ancestor that implements the
    drag-and-drop target interface
    @param wgt widget to call
    @param event input event
    @param managed data
 */
void awe_drag_and_drop_target_key_up(AWE_WIDGET *wgt, const AWE_EVENT *event, AWE_OBJECT *data);


/** drag-and-drop event procedure
    @param action current event mode action
    @param event event data
    @param data the data object
    @return 0 if event is sent to widget, 1 otherwise
 */
int awe_drag_and_drop_event_proc(AWE_EVENT_MODE_ACTION_TYPE action, AWE_EVENT *event, void *data);


/** begins a drag-and-drop session. The session will be ended upon next release
    of a mouse button. The source widget must not be deleted during a drag-and-drop
    session.
    @param source source widget; it is the widget that 'posts' the dragged data
    @param data the data object; it must not be null; it will be automatically
           deleted when the session ends.
    @return non-zero if the session started successfully, false otherwise.
 */
int awe_begin_drag_and_drop(AWE_WIDGET *source, AWE_OBJECT *data);


/** ends a drag-and-drop session. There is no need to call this manually, because
    it will be called automatically when the user releases a mouse button.
    @return non-zero if the session ended successfully.
 */
int awe_end_drag_and_drop();


/** sends the 'clear' message to the source widget. The source widget must
    then clear its selection, i.e. delete its data. This function must be
    called if the user requested a data move instead of data copy, by holding
    the shift down during dragging.
    @param non-zero if the clear was successful.
 */
int awe_clear_drag_and_drop_source();


/*@}*/


#ifdef __cplusplus
    }
#endif


#endif //AWE_DRAGANDDROP_H
