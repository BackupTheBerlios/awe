#ifndef AWE_EVENT_H
#define AWE_EVENT_H


#include "widget.h"


#ifdef __cplusplus
   extern "C" {
#endif


/**@name Event
    <p>The purpose of the Event module is to provide a flexible mechanism
       for managing events (mouse, keyboard and timers). The user has to 
       implement an event loop; all event handling is done from there, so
       this mechanism can be embedded in game and application control loops.
    </p>
    <p>Events are handled by event manager procedures that can be registered/
       unregistered with the module. An unlimited amount of event manager
       procedures can be registered. When an event happens, all registered
       procedures are called in the order that they are registered. Event
       processing stops if an event manager procedure returns 0.
    </p>
    <p> The event module also has another capability: the event mode stack.
        This stack consists of event modes; each event mode contains a list
        of event manager procedures that handles events. It is only the top
        stack event mode that manages events. This capability allows the
        programmer to implement various event modes, without ever blocking
        the main event loop. The library itself uses this mechanism to
        implement various event modes, like drag-n-drop, modal widgets, 
        event grabbing, menus etc. Event manager procs can actually be
        registered only to the current event mode.
    </p>
    <p> The following diagram shows a typical instance of the event system:
    </p>
    <pre>
                     EVENT MODE STACK
              top ----------------------
                  |                    |
          input   | current event mode |  widget methods
         ------>  |                    |  -------------->
                  ----------------------
                  |                    |
                  | previous event mode|
                  |                    |
                  ----------------------
                           ...
                  ----------------------
                  |                    |
                  |  first event mode  |
                  |                    |
           bottom ----------------------
    </pre>
    <p> Each event manager procedure uses the COM interface to actually call
        methods of objects. For example, widgets that wish to provide drag-n-drop
        must implement the relevant interface.
    </p>
    <p> Widget events are generated directly from inside the normal event
        management procedure; in other words, widget event methods are
        called directly from event manager procedures.
    </p>
    <p> When an event mode is popped from the stack, the event manager procedures
        of the current event mode are automatically removed from the system.
    </p>
 */
/*@{*/


///event action type
enum AWE_EVENT_MODE_ACTION_TYPE {
    ///begin event mode
    AWE_EVENT_MODE_ACTION_BEGIN,

    ///do event
    AWE_EVENT_MODE_ACTION_DO,

    ///end event mode
    AWE_EVENT_MODE_ACTION_END
};
typedef enum AWE_EVENT_MODE_ACTION_TYPE AWE_EVENT_MODE_ACTION_TYPE;


/** type of event manager procedure
    @param action action type
    @param event input event; may be null, depending on action
    @param data user-defined data
    @return 1 if procedure processed the event and event processing should stop
 */
typedef int (*AWE_EVENT_PROC)(AWE_EVENT_MODE_ACTION_TYPE action, AWE_EVENT *event, void *data);


/** registers an event procedure in the current event mode. If the combination
    of procedure and data is already registered, it is not registered again.
    @param proc procedure to add to the current event mode
    @param data user-defined data to pass to the procedure when it is called
 */
void awe_add_event_proc(AWE_EVENT_PROC proc, void *data);


/** replaces an event procedure with another event procedure. It allows for
    procedures to nest. All entries equal to old proc are changed.
    @param new_proc new procedure
    @param old_proc old procedure
 */
void awe_replace_event_proc(AWE_EVENT_PROC new_proc, AWE_EVENT_PROC old_proc);


/** unregisters an event procedure from the current event mode.
    @param proc procedure to remove from the current event mode
    @param data user-defined data as defined in the time of registration
 */
void awe_remove_event_proc(AWE_EVENT_PROC proc, void *data);


/** enters a new event mode. The event mode begins with the given event
    procedure, which is automatically unregistered when the mode is exited.
    @param proc initial event manager procedure
    @param data user-defined data to pass to the procedure when it is called
 */
void awe_enter_event_mode(AWE_EVENT_PROC proc, void *data);


/** leaves the current event mode, popping the previous one from the 
    event mode stack. If the current event mode is the first one, then
    nothing happens (the first mode can't be removed). Event procedures
    are notified of the event mode end and then removed from the system.
 */
void awe_leave_event_mode();


/** does events by reading the input (using the input module), then calling
    all procedures of the current event mode. It must be put in a loop.
 */
void awe_do_events();


/** event manager procedure that does normal event handling:
    <li>it sends mouse events to the widget under mouse</li>
    <li>it sends keyboard events to the focus widget</li>
    <li>it sends timer events to the widget that has registered the timer</li>
    @param action current event mode action
    @param event event data
    @param data unused
    @return 0 if event is sent to widget, 1 otherwise
 */
int awe_def_event_proc(AWE_EVENT_MODE_ACTION_TYPE action, AWE_EVENT *event, void *data);


/** grab event procedure; all events are sent to the the widget that has
    grabbed events
    @param action current event mode action
    @param event event data
    @param data the destination widget
    @return 0 if event is sent to widget, 1 otherwise
 */
int awe_grab_event_proc(AWE_EVENT_MODE_ACTION_TYPE action, AWE_EVENT *event, void *data);


/** modal event procedure; events are sent to the proper widget as in the
    default event procedure, but only if the widget belongs in the modal 
    widget tree.
    @param action current event mode action
    @param event event data
    @param data the modal widget
    @return 0 if event is sent to widget, 1 otherwise
 */
int awe_modal_event_proc(AWE_EVENT_MODE_ACTION_TYPE action, AWE_EVENT *event, void *data);


/*@}*/


#ifdef __cplusplus
   }
#endif


#endif //AWE_EVENT_H
