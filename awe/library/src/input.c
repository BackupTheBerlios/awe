#include "input.h"
#include "string.h"


/*****************************************************************************
    PRIVATE
 *****************************************************************************/


//miliseconds that the timer ticks
#define TIMER_BEAT           10


//maximum number of events
#define MAX_EVENT            256


//maximum number of timers
#define MAX_TIMER            8


//event queue type
typedef struct QUEUE {
    int free;
    int avail;
    int used;
    AWE_EVENT event[MAX_EVENT];
} QUEUE;


//timer
typedef struct TIMER {
    int valid;
    int counter;
    int interval;
    int id;
    void *data;
} TIMER;


//variables
static unsigned _timer = 0;
static void (*prev_mouse_callback)(int) = 0;
static int (*prev_keyboard_ucallback)(int, int*) = 0;
static void (*prev_keyboard_lowlevel_callback)(int) = 0;
static QUEUE event_queue = {0, 0, 0};
static int mouse_button = 0;
static int key_table[KEY_MAX];
static TIMER timer[MAX_TIMER];


//externals
extern void _install_event_lock();
extern void _lock_events();
extern void _unlock_events();


//allocates an event entry in the given queue
static INLINE AWE_EVENT *alloc_event(QUEUE *q)
{
    AWE_EVENT *r;

    if (q->used == MAX_EVENT) return 0;
    r = q->event + q->free;
    q->free++;
    if (q->free == MAX_EVENT) q->free = 0;
    q->used++;
    return r;
}
END_OF_STATIC_FUNCTION(alloc_event);


//frees an available event entry in the given queue
static INLINE AWE_EVENT *free_event(QUEUE *q)
{
    AWE_EVENT *r;

    if (!q->used) return 0;
    r = q->event + q->avail;
    q->avail++;
    if (q->avail == MAX_EVENT) q->avail = 0;
    q->used--;
    return r;
}
END_OF_STATIC_FUNCTION(free_event);


//puts a timer event
static INLINE void put_timer_event(AWE_EVENT_TYPE type, int id, void *data)
{
    AWE_EVENT *e;

    _lock_events();
    e = alloc_event(&event_queue);
    if (e) {
        e->timer.type = type;
        e->timer.time = _timer;
        e->timer.id = id;
        e->timer.data = data;
    }
    _unlock_events();
}
END_OF_STATIC_FUNCTION(put_timer_event);


//puts a mouse event
static INLINE void put_mouse_event(AWE_EVENT_TYPE type)
{
    AWE_EVENT *e;

    _lock_events();
    e = alloc_event(&event_queue);
    if (e) {
        e->mouse.type = type;
        e->mouse.time = _timer;
        e->mouse.shifts = key_shifts;
        e->mouse.x = mouse_x;
        e->mouse.y = mouse_y;
        e->mouse.z = mouse_z;
        e->mouse.button = mouse_button;
    }
    _unlock_events();
}
END_OF_STATIC_FUNCTION(put_mouse_event);


//puts a key event
static INLINE void put_key_event(AWE_EVENT_TYPE type, int key, int scancode)
{
    AWE_EVENT *e;

    _lock_events();
    e = alloc_event(&event_queue);
    if (e) {
        e->key.type = type;
        e->key.time = _timer;
        e->key.shifts = key_shifts;
        e->key.key = key;
        e->key.scancode = scancode;
    }
    _unlock_events();
}
END_OF_STATIC_FUNCTION(put_key_event);


//timer proc
static void timer_proc(void)
{
    TIMER *t;

    //generic timer
    _timer += TIMER_BEAT;

    //play timers
    for(t = timer; t < timer + MAX_TIMER; t++) {
        if (t->valid) {
            t->counter += TIMER_BEAT;
            if (t->counter >= t->interval) {
                t->counter = 0;
                put_timer_event(AWE_EVENT_TIMER, t->id, t->data);
            }
        }
    }
}
END_OF_STATIC_FUNCTION(timer_proc);


//mouse proc
static void mouse_proc(int flags)
{
    //mouse down
    if (mouse_b && !mouse_button) {
        mouse_button = mouse_b;
        put_mouse_event(AWE_EVENT_BUTTON_DOWN);
    }

    //else mouse up
    else if (!mouse_b && mouse_button) {
        put_mouse_event(AWE_EVENT_BUTTON_UP);
        mouse_button = 0;
    }

    //mouse move
    if (flags & MOUSE_FLAG_MOVE) put_mouse_event(AWE_EVENT_MOUSE_MOVE);

    //mouse wheel
    if (flags & MOUSE_FLAG_MOVE_Z) put_mouse_event(AWE_EVENT_MOUSE_WHEEL);

    //call previous handler
    if (prev_mouse_callback) prev_mouse_callback(flags);
}
END_OF_STATIC_FUNCTION(mouse_proc);


//keyboard proc
static int keyboard_proc(int key, int *scancode)
{
    //store unicode character for key up
    key_table[*scancode] = key;

    //put event
    put_key_event(AWE_EVENT_KEY_DOWN, key, *scancode);

    //call previous handler
    return prev_keyboard_ucallback ? prev_keyboard_ucallback(key, scancode) : key;
}
END_OF_STATIC_FUNCTION(keyboard_proc);


//keyboard lowlevel proc
static void keyboard_lowlevel_proc(int key)
{
    //get scancode and release flag
    int scancode = key & 127;
    int released = key & 128;

    //put special modifier key as separate key press
    switch (scancode) {
        case KEY_LCONTROL:
        case KEY_RCONTROL:
        case KEY_LSHIFT:
        case KEY_RSHIFT:
        case KEY_ALT:
        case KEY_ALTGR:
            put_key_event(released ? AWE_EVENT_KEY_UP : AWE_EVENT_KEY_DOWN, 0, scancode);
            break;

        //put key up only for other keys
        default:
            if (released) put_key_event(AWE_EVENT_KEY_UP, key_table[scancode], scancode);
    }

    //call previous handler
    if (prev_keyboard_lowlevel_callback) prev_keyboard_lowlevel_callback(key);
}
END_OF_STATIC_FUNCTION(keyboard_lowlevel_proc);


//finds a timer
static TIMER *_find_timer(int id, void *data)
{
    TIMER *t;

    for(t = timer; t < timer + MAX_TIMER; t++) {
        if (t->valid && t->id == id && t->data == data) return t;
    }
    return 0;
}


//finds an empty timer
static TIMER *_find_empty_timer()
{
    TIMER *t;

    for(t = timer; t < timer + MAX_TIMER; t++) {
        if (!t->valid) return t;
    }
    return 0;
}


/*****************************************************************************
    PUBLIC
 *****************************************************************************/


//installs the input system
void awe_install_input()
{
    static int installed = FALSE;

    if (installed) return;
    installed = TRUE;

    _install_event_lock();

    //lock resources
    LOCK_VARIABLE(_timer);
    LOCK_VARIABLE(prev_mouse_callback);
    LOCK_VARIABLE(prev_keyboard_ucallback);
    LOCK_VARIABLE(prev_keyboard_lowlevel_callback);
    LOCK_VARIABLE(event_queue);
    LOCK_VARIABLE(mouse_button);
    LOCK_VARIABLE(key_table);
    LOCK_VARIABLE(timer);
    LOCK_FUNCTION(alloc_event);
    LOCK_FUNCTION(free_event);
    LOCK_FUNCTION(put_timer_event);
    LOCK_FUNCTION(put_mouse_event);
    LOCK_FUNCTION(put_key_event);
    LOCK_FUNCTION(timer_proc);
    LOCK_FUNCTION(mouse_proc);
    LOCK_FUNCTION(keyboard_proc);
    LOCK_FUNCTION(keyboard_lowlevel_proc);

    //save previous resources
    prev_mouse_callback = mouse_callback;
    prev_keyboard_ucallback = keyboard_ucallback;
    prev_keyboard_lowlevel_callback = keyboard_lowlevel_callback;

    //set resources
    install_int(timer_proc, TIMER_BEAT);
    memset(timer, 0, sizeof(timer));
    mouse_callback = mouse_proc;
    keyboard_ucallback = keyboard_proc;
    keyboard_lowlevel_callback = keyboard_lowlevel_proc;
}


//puts an event to the event queue
void awe_put_event(const AWE_EVENT *event)
{
    AWE_EVENT *e;

    _lock_events();
    e = alloc_event(&event_queue);
    if (e) *e = *event;
    _unlock_events();
}


//gets an event from the event queue
AWE_EVENT_TYPE awe_get_event(AWE_EVENT *event)
{
    AWE_EVENT *e;

    event->type = AWE_EVENT_NONE;

    //get event from queue
    _lock_events();
    e = free_event(&event_queue);
    if (e) *event = *e;
    _unlock_events();

    return event->type;
}


//adds a timer
int awe_add_timer(void *data, int id, int msecs)
{
    TIMER *t;

    t = _find_timer(id, data);
    if (!t) {
        t = _find_empty_timer();
        if (!t) return 0;
    }
    t->counter = 0;
    t->interval = msecs;
    t->id = id;
    t->data = data;
    t->valid = TRUE;

    return 1;
}


//deletes a timer
int awe_remove_timer(void *data, int id)
{
    TIMER *t;

    t = _find_timer(id, data);
    if (!t) return 0;
    t->valid = FALSE;
    return 1;
}


//enumerates the set timers
void awe_enum_timers(AWE_TIMER_ENUM_PROC proc, void *data)
{
    TIMER *t;

    for(t = timer; t < timer + MAX_TIMER; t++) {
        if (t->valid && !proc(t->data, t->id, data)) return;
    }
}


//enumerates the event queue
void awe_enum_events(AWE_EVENT_ENUM_PROC proc, void *data)
{
    AWE_EVENT *event;
    int i;

    _lock_events();
    event = event_queue.event + event_queue.avail;
    for(i = 0; i < event_queue.used; ++i) {
        if (!proc(event, data)) break;
        ++event;
        if (event == event_queue.event + MAX_EVENT) event = event_queue.event;
    }
    _unlock_events();
}
