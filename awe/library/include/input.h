#ifndef AWE_INPUT_H
#define AWE_INPUT_H


#include "allegro.h"


#ifdef __cplusplus
   extern "C" {
#endif


/**@name Input
    <p>The purpose of the Input module is to provide mouse, keyboard and timer
    events to the application. It offers the following additional functionality
    to Allegro:</p>
    <LL>
    <LI>Queued input; events are stored in a buffer, so as that there is 
        no possibility to miss them.
    </LI>
    <LI> Keyboard down and up events.
    </LI>
    <LI> Timer events.
    </LI>
    <LI> Events can be inserted into the queue; used for simulated input
         (robot programs etc).
    </LI>
    </LL>
    <p>The Input module is indepentent from widgets; it can also be used for
       managing the input of a game in the main game loop.</p>
 */
/*@{*/


/** event type
 */
enum AWE_EVENT_TYPE {
    ///no event type
    AWE_EVENT_NONE = 0,

    ///button down event
    AWE_EVENT_MOUSE_DOWN,

    ///button up event
    AWE_EVENT_MOUSE_UP,

    ///mouse moved event
    AWE_EVENT_MOUSE_MOVE,

    ///mouse wheel event
    AWE_EVENT_MOUSE_WHEEL,

    ///key down
    AWE_EVENT_KEY_DOWN,

    ///key press
    AWE_EVENT_KEY_UP,

    ///timer event
    AWE_EVENT_TIMER
};
typedef enum AWE_EVENT_TYPE AWE_EVENT_TYPE;


///mouse event structure
struct AWE_MOUSE_EVENT {
    ///event type
    AWE_EVENT_TYPE type;

    ///time in miliseconds that the event happened at, from start of input module
    unsigned time;

    ///allegro key shifts at the time of the event
    short shifts;

    ///x position of mouse relative to the screen
    short x;

    ///y position of mouse relative to the screen
    short y;

    ///mouse wheel value
    short z;

    ///allegro mouse button value
    short button;
};
typedef struct AWE_MOUSE_EVENT AWE_MOUSE_EVENT;


///keyboard event structure
struct AWE_KEY_EVENT {
    ///event type
    AWE_EVENT_TYPE type;

    ///time in miliseconds that the event happened at, from start of input module
    unsigned time;

    ///allegro key shifts at the time of the event
    short shifts;

    ///unicode character of the pressed/released key
    short key;

    ///keyboard scancode
    short scancode;
};
typedef struct AWE_KEY_EVENT AWE_KEY_EVENT;


///timer event structure
struct AWE_TIMER_EVENT {
    ///event type
    AWE_EVENT_TYPE type;

    ///time in miliseconds that the event happened at, from start of input module
    unsigned time;

    ///id of the timer event
    int id;

    ///user-defined data
    void *data;
};
typedef struct AWE_TIMER_EVENT AWE_TIMER_EVENT;


/** event structure; it is a union of the structures mentioned above. It must
    be used according to the event type.
 */
union AWE_EVENT {
    ///event type
    AWE_EVENT_TYPE type;

    ///mouse information
    AWE_MOUSE_EVENT mouse;

    ///key information
    AWE_KEY_EVENT key;

    ///timer information
    AWE_TIMER_EVENT timer;
};
typedef union AWE_EVENT AWE_EVENT;


/** type of procedure to use when enumerating timers
    @param timer_data data of timer as defined when the timer is set
    @param id id of timer as defined when the timer is set
    @param cb_data callback data passed at the timer enumeration procedure
    @return zero if enumeration should stop
 */
typedef int (*AWE_TIMER_ENUM_PROC)(void *timer_data, int id, void *cb_data);


/** type of procedure to use when enumerating events
    @param event current event
    @param cb_data callback data passed at the event enumeration procedure
    @return zero if enumeration should stop
 */
typedef int (*AWE_EVENT_ENUM_PROC)(AWE_EVENT *event, void *cb_data);


/** installs the input system; must be called after allegro is initialized.
    It requires allegro mouse, keyboard and timers. It hooks mouse and keyboard
    callbacks chaining previous handlers. It can be called more than once,
    but it will install stuff only the first time it is called.
 */
void awe_install_input();


/** puts an event to the event queue
    @param event event to copy to the internal queue
 */
void awe_put_event(AL_CONST AWE_EVENT *event);


/** gets an event from the event queue
    @param event buffer to receive the input event
    @return the event type or 'none' if there is no event
 */
AWE_EVENT_TYPE awe_get_event(AWE_EVENT *event);


/** sets or re-sets a timer event
    @param data event data; copied to the event structure
    @param id user-defined timer id
    @param msecs timer interval in miliseconds
    @return zero if there are no more timer resources left
 */
int awe_add_timer(void *data, int id, int msecs);


/** removes a timer event
    @param data event data, as given when the timer is set
    @param id user-defined timer id, as given when the timer is set
    @return zero if the timer was not found
 */
int awe_remove_timer(void *data, int id);


/** enumerates the set timers
    @param proc enumerate proc
    @param data callback data
 */
void awe_enum_timers(AWE_TIMER_ENUM_PROC proc, void *data);


/** enumerates the event queue
    @param proc enumerate proc
    @param data callback data
 */
void awe_enum_events(AWE_EVENT_ENUM_PROC proc, void *data);


/*@}*/


#ifdef __cplusplus
   }
#endif


#endif //AWE_INPUT_H
