#ifndef AWE_COM_H
#define AWE_COM_H


#include <stdarg.h>
#include "linkedlist.h"


#ifdef __cplusplus
    extern "C" {
#endif


struct AWE_CLASS;


/**@name COM
    <p>The purpose of the COM module is to provide an API for managing the
    object-oriented nature of GUI development. It has many capabilities:</p>
    <LL>
    <LI>a class registry; one or more classes can be registered with the system
        in order for the system to be able to manage classes at run-time.
    </LI>
    <LI>object properties; an easy-to-use mechanism is provided for setting
        and retrieving properties of an object
    </LI>
    <LI>object events; an object can be registered as a listener to events
        produced by another object
    </LI>
    <LI>reflection; each class can be filled with reflection information on
        properties and events a class has. This information can be queried in
        run time.
    </LI>
    <LI>single inheritance with interfaces.
    </LI>
    </LL>
 */
/*@{*/


///AWE namespace name
#define AWE_ID_AWE              "AWE"


/** class registry event type; passed when a registry hook procedure is called.
 */
enum AWE_CLASS_REGISTRY_EVENT_TYPE {
    /** event type when a class is added to the system */
    AWE_CLASS_ADDED,

    /** event type when a class is removed from the system */
    AWE_CLASS_REMOVED,
};
typedef enum AWE_CLASS_REGISTRY_EVENT_TYPE AWE_CLASS_REGISTRY_EVENT_TYPE;


/** macro to call a method of an object; the method is called if it exists
    @param OBJ object to get the interface of
    @param NAME name of the interface
    @param NAMESPACE namespace of the interface
    @param INTERFACE interface struct name
    @param METHOD method member name 
    @param PARAMS parameters
 */
#define AWE_CALL_METHOD(OBJ, NAME, NAMESPACE, INTERFACE, METHOD, PARAMS) {\
    INTERFACE *vtable = (INTERFACE *)awe_get_object_interface((AWE_OBJECT *)OBJ, NAME, NAMESPACE);\
    if (vtable && vtable->METHOD) vtable->METHOD PARAMS;\
}   


/** macro to call a method of an object that has a result other than void; the method is called if it exists
    @param OBJ object to get the interface of
    @param NAME name of the interface
    @param NAMESPACE namespace of the interface
    @param INTERFACE interface struct name
    @param METHOD method member name 
    @param PARAMS parameters
    @param RESULT result variable
    @param DEFAULT_RESULT default result in case the method is not found
 */
#define AWE_CALL_METHOD_R(OBJ, NAME, NAMESPACE, INTERFACE, METHOD, PARAMS, RESULT, DEFAULT_RESULT) {\
    INTERFACE *vtable = (INTERFACE *)awe_get_object_interface((AWE_OBJECT *)OBJ, NAME, NAMESPACE);\
    if (vtable && vtable->METHOD) RESULT = vtable->METHOD PARAMS; else RESULT = DEFAULT_RESULT;\
}   


/** root 'class' for all objects. It must be the first member of inheriting
    classes.
 */
struct AWE_OBJECT {
    struct AWE_CLASS *pclass;
    AWE_DL_LIST signals;
    AWE_DL_LIST slots;
};
typedef struct AWE_OBJECT AWE_OBJECT;


/** root 'class' for all interfaces. It must be the first member of inheriting
    interfaces.
 */
typedef struct AWE_OBJECT_VTABLE {
    /** method for getting an interface of an object. It is optional. 
        @param obj object to get an interface of
        @param name NULL-terminated C string name of the interface
        @param pnamespace NULL-terminated C string namespace of the interface
        @return pointer to vtable struct that is the requested interface; if
                the interface is not supported, it must be NULL
     */
    void *(*get_interface)(AWE_OBJECT *obj, const char *name, const char *pnamespace);

    /** method that is called when the properties of an object changed. It is
        called after all properties of the object are changed in order to 
        allow the object to perform side effect actions (for example, redraw
        the widget). It is optional.
        @param obj the affected object
     */
    void (*properties_changed)(AWE_OBJECT *obj);

    /** method for creating a copy of the object. It is optional.
        @param obj object to clone
        @return pointer to the cloned object
     */
    AWE_OBJECT *(*clone)(AWE_OBJECT *obj);
} AWE_OBJECT_VTABLE;


/** struct that represents a value-key pair for an enumerated property.
 */
struct AWE_CLASS_ENUMERATION {
    ///NULL-terminated C string name of the value; use NULL for last entry
    const char *name;

    ///the enumeration's value
    int value;
};
typedef struct AWE_CLASS_ENUMERATION AWE_CLASS_ENUMERATION;


/** struct that represents an object's property. It is used in reflection.
    The AWE_CLASS structure contains a pointer to a NULL-terminated table of
    this struct.
 */
typedef struct AWE_CLASS_PROPERTY {
    /** NULL-terminated C string of the property's name. If this entry is the
        last in the class' table, then this member should be NULL.
     */
    const char *name;

    /** type of the property; NULL-terminated C string of the property's C type.
     */
    const char *type;

    /** byte size of the property when on the stack. It is used to retrieve the
        property's value from the stack. This may differ from the physical size
        of the property, depending on compiler. For example, many modern 
        compilers promote chars(1 byte) to ints (4 bytes), so care must be taken
        so as that this member contains the exact number of bytes that the
        property has when passed on the stack.
     */
    int stack_size;

    /** optional function which is called when an object's property is to
        be retrieved.
        @param obj object to get the property of
        @param data pointer to the buffer that the value must be copied to. 
               The maximum size of the buffer is 256 bytes.
     */
    void (*get)(AWE_OBJECT *obj, void *data);

    /** optional function which is called when an object's property is to
        be changed.
        @param obj object to set the property of
        @param data pointer to the buffer that contains the new value. 
               The maximum size of the buffer is 256 bytes.
     */
    void (*set)(AWE_OBJECT *obj, void *data);

    ///optional pointer to NULL-terminated enumeration information
    AWE_CLASS_ENUMERATION *enumeration;
} AWE_CLASS_PROPERTY;


/** structure used for reflecting events' arguments.
 */
struct AWE_CLASS_EVENT_ARGUMENT {
    /** NULL-terminated C string of the argument's name; if this entry is the
        last in the event's arguments table, it should be NULL */
    const char *name;

    /** NULL-terminated C string of the argument's C type */
    const char *type;
};
typedef struct AWE_CLASS_EVENT_ARGUMENT AWE_CLASS_EVENT_ARGUMENT;


/** structure used for reflecting events of an object.
 */
struct AWE_CLASS_EVENT {
    /** NULL-terminated C string of the event's name */
    const char *name;

    /** NULL-terminated array the event's arguments. */
    AWE_CLASS_EVENT_ARGUMENT *args;
};
typedef struct AWE_CLASS_EVENT AWE_CLASS_EVENT;


/** structure that holds class information for an object. It is used both for
    creating instances and for holding an object's class.
 */
struct AWE_CLASS {
    /** NULL-terminated C string of the object's class name. It should not be
        NULL.
     */
    const char *name;

    /** NULL-terminated C string of the object's namespace. If NULL, the object
        belongs to no namespace.
     */
    const char *pnamespace;

    /** pointer to the super class of the object. If the object has no super
        classes, then it must be derived from 'object', therefore it must
        point to 'awe_object_class'.
     */
    struct AWE_CLASS *super;

    /** size of the data structure that implements the class, in bytes. Use
        the 'sizeof' operator to fill this member.
     */
    int size;

    /** pointer to NULL-terminated array of the class' properties.
     */
    AWE_CLASS_PROPERTY *properties;

    /** pointer to NULL-terminated array of the class' events.
     */
    AWE_CLASS_EVENT *events;

    /** pointer to the class' vtable. This can be a pointer to a struct that
        'inherits' AWE_OBJECT_VTABLE (by having it as a first member). It
        should not be NULL.
     */
    AWE_OBJECT_VTABLE *vtable;

    /** the object's constructor; it is optional. It is called after the
        super class' constructor is called, but before the constructors of
        subclasses. It is provided so as that the object initializes itself
        to known values. Before constructors are called, the object's memory
        block is initialized to 0.
        @param obj pointer to object to initialize
     */
    void (*constructor)(AWE_OBJECT *obj);

    /** the object's destructor; it is optional. It is called before the 
        super class' destructor and after the destructors of subclasses.
        @param obj pointer to object to clean up
     */
    void (*destructor)(AWE_OBJECT *obj);
};
typedef struct AWE_CLASS AWE_CLASS;


//signal node
typedef struct _AWE_SIGNAL {
    AWE_DL_NODE node;
    AWE_DL_LIST slots;
    const char *name;
} _AWE_SIGNAL;


//slot
typedef struct _AWE_SLOT {
    AWE_DL_DATA_NODE signal_node;
    AWE_DL_DATA_NODE slot_node;
    AWE_DL_LIST *signal_list;
    AWE_DL_LIST *slot_list;
    AWE_OBJECT *object;
    void (*proc)(AWE_OBJECT *obj, ...);
} _AWE_SLOT;


/** type of procedure for class registry
    @param pclass pointer to the class struct that is being affected
    @param event event type
 */
typedef void (*AWE_CLASS_REGISTRY_PROC)(AWE_CLASS *pclass, AWE_CLASS_REGISTRY_EVENT_TYPE event);


/** type of procedure for class registry enumeration
    @param pclass pointer to the enumerated class
    @param user-defined data passed at the enum function
 */
typedef int (*AWE_CLASS_REGISTRY_ENUM_PROC)(AWE_CLASS *pclass, void *data);


/** base class for all objects. The 'AWE_CLASS->super' member must point to
    this variable if the class has no other super classes.
 */
extern AWE_CLASS awe_object_class;


/** the default implementation of the get_interface method
    @param obj object to be called
    @param name name of the interface
    @param pnamespace namespace of the interface
    @return pointer to interface or null if the interface is not supported
 */
void *awe_object_get_interface(AWE_OBJECT *obj, const char *name, const char *pnamespace);


/** the default implementation of the clone method
    @param obj object to clone
    @return the cloned object
 */
AWE_OBJECT *awe_object_clone(AWE_OBJECT *obj);


//finds a signal from name
_AWE_SIGNAL *_awe_find_signal(AWE_OBJECT *obj, const char *name);


/** searches the class registry for a class with given name and namespace
    @param name NULL-terminated C string of the class name; the search is
           case-sensitive
    @param pnamespace NULL-terminated C string of the class namespace
    @return pointer to the found class or NULL if the class is not found
 */
AWE_CLASS *awe_find_class(const char *name, const char *pnamespace);


/** enumerates the registered classes
    @param proc procedure to call for enumeration; enumeration stops if this
                procedure returns 0
    @param data user-defined parameter passed at the enumeration proc
 */
void awe_enum_classes(AWE_CLASS_REGISTRY_ENUM_PROC proc, void *data);


/** registers a class. If the class is already registered, then it increases
    its reference count. The class should be unregistered the same times that
    it is registered.
    @param pclass pointer to the class to register.
 */
void awe_register_class(AWE_CLASS *pclass);


/** unregisters a class. If the class is registered for more than one time,
    its reference count is decreased. If the reference count drops to 0, then
    the class is removed from the system
    @param pclass pointer to the class to unregister
 */
void awe_unregister_class(AWE_CLASS *pclass);


/** hooks the class registry. If the given procedure is already registered, it
    must be unregistered the same amount of times. The given procedure is 
    called when a class registry event takes place
    @param proc procedure to register
 */
void awe_add_class_registry_proc(AWE_CLASS_REGISTRY_PROC proc);


/** unhooks the class registry. If the given procedure is registered more than
    once, then it must be unregistered the same amount of times that it is
    registered
    @param proc procedure to unregister
 */
void awe_remove_class_registry_proc(AWE_CLASS_REGISTRY_PROC proc);


/** creates an object instance from a variable argument list. The variable
    argument list must contain (name, value) pairs: the name must be a NULL
    terminated C string of the object's property, as declared by the object;
    the value must be the property's value. The list must be terminated by NULL.
    This function is provided so as that it can be wrapped inside functions
    that create specific types of objects.
    Object constructors are called from base to derived class object.
    @param pclass class to create an instance of
    @param params the class' properties to set
    @return pointer to the created object instance or NULL if the object could
            not be created
 */
AWE_OBJECT *awe_create_object_va(AWE_CLASS *pclass, va_list params);


/** creates an object instance from a class. Same as 'awe_create_object_va'.
    @param pclass class to create an instance of
    @param ... the class' properties to set
    @return pointer to the created object instance or NULL if the object could
            not be created
 */
AWE_OBJECT *awe_create_object(AWE_CLASS *pclass, ...);


/** creates an object instance by class name and namespace. It searches the
    class registry for a class of given name and namespace, then creates 
    the instance. Same as 'awe_create_object_va'.
    @param name name of the class to create
    @param pnamespace namespace that the class belongs
    @param ... the class' properties to set
    @return pointer to the created object instance or NULL if the object could
            not be created
 */
AWE_OBJECT *awe_create_object_by_name(const char *name, const char *pnamespace, ...);


/** creates an object instance by class name and namespace. It searches the
    class registry for a class of given name and namespace, then creates 
    the instance. Same as 'awe_create_object_va'.
    @param name name of the class to create
    @param pnamespace namespace that the class belongs
    @param params the class' properties to set
    @return pointer to the created object instance or NULL if the object could
            not be created
 */
AWE_OBJECT *awe_create_object_by_name_va(const char *name, const char *pnamespace, va_list params);


/** clones an object by using the 'clone' interface.
    @param obj object to clone
    @param pointer to object that is the clone of given object, or NULL if
           the object can not be cloned
 */
AWE_OBJECT *awe_clone_object(AWE_OBJECT *obj);


/** destroys an object. The object's destructors are called from derived to
    base class order.
    @param obj object to destroy.
 */
void awe_destroy_object(AWE_OBJECT *obj);


/** retrieves an object's properties. The given variable argument list must be
    terminated by NULL and it contains a list of (name, buffer) pairs that
    are used for property retrieval. The name must be a NULL-terminated C
    string (same as the class property); the buffer must hold enough bytes for
    storing the object's property (max size is 256 bytes). Each buffer variable
    must be a pointer.
    @param obj object to retrieve the properties of
    @param params received properties NULL-terminated list
    @return the number of properties that were successfully retrieved
 */
int awe_get_object_properties_va(AWE_OBJECT *obj, va_list params);


/** retrieves an object's properties. The given variable argument list must be
    terminated by NULL and it contains a list of (name, buffer) pairs that
    are used for property retrieval. The name must be a NULL-terminated C
    string (same as the class property); the buffer must hold enough bytes for
    storing the object's property (max size is 256 bytes). Each buffer variable
    must be a pointer.
    @param obj object to retrieve the properties of
    @param ... received properties NULL-terminated list
    @return the number of properties that were successfully retrieved
 */
int awe_get_object_properties(AWE_OBJECT *obj, ...);


/** sets an object's properties. The variable argument list must contain
    (name, value) pairs: the name must be a NULL terminated C string of the
    object's property, as declared by the object; the value must be the
    property's value. The list must be terminated by NULL.
    @param obj object to set the properties of
    @param params NULL-terminated list of properties
 */
int awe_set_object_properties_va(AWE_OBJECT *obj, va_list params);


/** sets an object's properties. The variable argument list must contain
    (name, value) pairs: the name must be a NULL terminated C string of the
    object's property, as declared by the object; the value must be the
    property's value. The list must be terminated by NULL.
    @param obj object to set the properties of
    @param ... NULL-terminated list of properties
 */
int awe_set_object_properties(AWE_OBJECT *obj, ...);


/** registers an object as a listener to an event of another object. The
    binding between event producer and event listener is automatically removed
    when one of the two objects is destroyed.
    @param src 'source object'; it is the object that will fire the event
    @param name name of the event; same as the one provided by the class
    @param dst 'destination object'; optional pointer to object to call when the event happens;
           if you want to call a function, pass NULL
    @param proc address of procedure to call when the event is fired; the
           first parameter of the object must be AWE_OBJECT*; the rest of
           the parameters must match those of the event
    @return non-zero if the event is added successfully
 */
int awe_add_object_event(AWE_OBJECT *src, const char *name, AWE_OBJECT *dst, void *proc);


/** unregisters an object as listener of an event that is produced by another
    object.
    when one of the two objects is destroyed.
    @param src 'source object'; it is the object that will fire the event
    @param name name of the event; same as the one provided by the class
    @param dst 'destination object'; object to call when the event happens
    @param proc address of procedure to call when the event is fired
    @return non-zero if the event is removed successfully
 */
int awe_remove_object_event(AWE_OBJECT *src, const char *name, AWE_OBJECT *dst, void *proc);


/** macro that calls an object's event with 0 parameters
    @param OBJ pointer to AWE_OBJECT derived object to fire the event from
    @param NAME name of the event, as registered by the object's class
 */
#define awe_do_object_event0(OBJ, NAME) {\
    AWE_DL_NODE *node;\
    _AWE_SIGNAL *signal;\
    _AWE_SLOT *slot;\
    signal = _awe_find_signal((AWE_OBJECT *)OBJ, NAME);\
    if (signal) {\
        for(node = signal->slots.first; node; node = node->next) {\
            slot = (_AWE_SLOT *)((AWE_DL_DATA_NODE *)node)->data;\
            slot->proc(slot->object);\
        }\
    }\
}


/** macro that calls an object's event with 1 parameter
    @param OBJ pointer to AWE_OBJECT derived object to fire the event from
    @param NAME name of the event, as registered by the object's class
    @param P1 1st parameter
 */
#define awe_do_object_event1(OBJ, NAME, P1) {\
    AWE_DL_NODE *node;\
    _AWE_SIGNAL *signal;\
    _AWE_SLOT *slot;\
    signal = _awe_find_signal((AWE_OBJECT *)OBJ, NAME);\
    if (signal) {\
        for(node = signal->slots.first; node; node = node->next) {\
            slot = (_AWE_SLOT *)((AWE_DL_DATA_NODE *)node)->data;\
            slot->proc(slot->object, P1);\
        }\
    }\
}


/** macro that calls an object's event with 2 parameters
    @param OBJ pointer to AWE_OBJECT derived object to fire the event from
    @param NAME name of the event, as registered by the object's class
    @param P1 1st parameter
    @param P2 2nd parameter
 */
#define awe_do_object_event2(OBJ, NAME, P1, P2) {\
    AWE_DL_NODE *node;\
    _AWE_SIGNAL *signal;\
    _AWE_SLOT *slot;\
    signal = _awe_find_signal((AWE_OBJECT *)OBJ, NAME);\
    if (signal) {\
        for(node = signal->slots.first; node; node = node->next) {\
            slot = (_AWE_SLOT *)((AWE_DL_DATA_NODE *)node)->data;\
            slot->proc(slot->object, P1, P2);\
        }\
    }\
}


/** macro that calls an object's event with 3 parameters
    @param OBJ pointer to AWE_OBJECT derived object to fire the event from
    @param NAME name of the event, as registered by the object's class
    @param P1 1st parameter
    @param P2 2nd parameter
    @param P3 3rd parameter
 */
#define awe_do_object_event3(OBJ, NAME, P1, P2, P3) {\
    AWE_DL_NODE *node;\
    _AWE_SIGNAL *signal;\
    _AWE_SLOT *slot;\
    signal = _awe_find_signal((AWE_OBJECT *)OBJ, NAME);\
    if (signal) {\
        for(node = signal->slots.first; node; node = node->next) {\
            slot = (_AWE_SLOT *)((AWE_DL_DATA_NODE *)node)->data;\
            slot->proc(slot->object, P1, P2, P3);\
        }\
    }\
}


/** macro that calls an object's event with 4 parameters
    @param OBJ pointer to AWE_OBJECT derived object to fire the event from
    @param NAME name of the event, as registered by the object's class
    @param P1 1st parameter
    @param P2 2nd parameter
    @param P3 3rd parameter
    @param P4 4th parameter
 */
#define awe_do_object_event4(OBJ, NAME, P1, P2, P3, P4) {\
    AWE_DL_NODE *node;\
    _AWE_SIGNAL *signal;\
    _AWE_SLOT *slot;\
    signal = _awe_find_signal((AWE_OBJECT *)OBJ, NAME);\
    if (signal) {\
        for(node = signal->slots.first; node; node = node->next) {\
            slot = (_AWE_SLOT *)((AWE_DL_DATA_NODE *)node)->data;\
            slot->proc(slot->object, P1, P2, P3, P4);\
        }\
    }\
}


/** macro that calls an object's event with 5 parameters
    @param OBJ pointer to AWE_OBJECT derived object to fire the event from
    @param NAME name of the event, as registered by the object's class
    @param P1 1st parameter
    @param P2 2nd parameter
    @param P3 3rd parameter
    @param P4 4th parameter
    @param P5 5th parameter
 */
#define awe_do_object_event5(OBJ, NAME, P1, P2, P3, P4, P5) {\
    AWE_DL_NODE *node;\
    _AWE_SIGNAL *signal;\
    _AWE_SLOT *slot;\
    signal = _awe_find_signal((AWE_OBJECT *)OBJ, NAME);\
    if (signal) {\
        for(node = signal->slots.first; node; node = node->next) {\
            slot = (_AWE_SLOT *)((AWE_DL_DATA_NODE *)node)->data;\
            slot->proc(slot->object, P1, P2, P3, P4, P5);\
        }\
    }\
}


/** macro that calls an object's event with 6 parameters
    @param OBJ pointer to AWE_OBJECT derived object to fire the event from
    @param NAME name of the event, as registered by the object's class
    @param P1 1st parameter
    @param P2 2nd parameter
    @param P3 3rd parameter
    @param P4 4th parameter
    @param P5 5th parameter
    @param P6 6th parameter
 */
#define awe_do_object_event6(OBJ, NAME, P1, P2, P3, P4, P5, P6) {\
    AWE_DL_NODE *node;\
    _AWE_SIGNAL *signal;\
    _AWE_SLOT *slot;\
    signal = _awe_find_signal((AWE_OBJECT *)OBJ, NAME);\
    if (signal) {\
        for(node = signal->slots.first; node; node = node->next) {\
            slot = (_AWE_SLOT *)((AWE_DL_DATA_NODE *)node)->data;\
            slot->proc(slot->object, P1, P2, P3, P4, P5, P6);\
        }\
    }\
}


/** macro that calls an object's event with 7 parameters
    @param OBJ pointer to AWE_OBJECT derived object to fire the event from
    @param NAME name of the event, as registered by the object's class
    @param P1 1st parameter
    @param P2 2nd parameter
    @param P3 3rd parameter
    @param P4 4th parameter
    @param P5 5th parameter
    @param P6 6th parameter
    @param P7 7th parameter
 */
#define awe_do_object_event7(OBJ, NAME, P1, P2, P3, P4, P5, P6, P7) {\
    AWE_DL_NODE *node;\
    _AWE_SIGNAL *signal;\
    _AWE_SLOT *slot;\
    signal = _awe_find_signal((AWE_OBJECT *)OBJ, NAME);\
    if (signal) {\
        for(node = signal->slots.first; node; node = node->next) {\
            slot = (_AWE_SLOT *)((AWE_DL_DATA_NODE *)node)->data;\
            slot->proc(slot->object, P1, P2, P3, P4, P5, P6, P7);\
        }\
    }\
}


/** macro that calls an object's event with 8 parameters
    @param OBJ pointer to AWE_OBJECT derived object to fire the event from
    @param NAME name of the event, as registered by the object's class
    @param P1 1st parameter
    @param P2 2nd parameter
    @param P3 3rd parameter
    @param P4 4th parameter
    @param P5 5th parameter
    @param P6 6th parameter
    @param P7 7th parameter
    @param P8 8th parameter
 */
#define awe_do_object_event8(OBJ, NAME, P1, P2, P3, P4, P5, P6, P7, P8) {\
    AWE_DL_NODE *node;\
    _AWE_SIGNAL *signal;\
    _AWE_SLOT *slot;\
    signal = _awe_find_signal((AWE_OBJECT *)OBJ, NAME);\
    if (signal) {\
        for(node = signal->slots.first; node; node = node->next) {\
            slot = (_AWE_SLOT *)((AWE_DL_DATA_NODE *)node)->data;\
            slot->proc(slot->object, P1, P2, P3, P4, P5, P6, P7, P8);\
        }\
    }\
}


/** macro that calls an object's event with 9 parameters
    @param OBJ pointer to AWE_OBJECT derived object to fire the event from
    @param NAME name of the event, as registered by the object's class
    @param P1 1st parameter
    @param P2 2nd parameter
    @param P3 3rd parameter
    @param P4 4th parameter
    @param P5 5th parameter
    @param P6 6th parameter
    @param P7 7th parameter
    @param P8 8th parameter
    @param P9 9th parameter
 */
#define awe_do_object_event9(OBJ, NAME, P1, P2, P3, P4, P5, P6, P7, P8, P9) {\
    AWE_DL_NODE *node;\
    _AWE_SIGNAL *signal;\
    _AWE_SLOT *slot;\
    signal = _awe_find_signal((AWE_OBJECT *)OBJ, NAME);\
    if (signal) {\
        for(node = signal->slots.first; node; node = node->next) {\
            slot = (_AWE_SLOT *)((AWE_DL_DATA_NODE *)node)->data;\
            slot->proc(slot->object, P1, P2, P3, P4, P5, P6, P7, P8, P9);\
        }\
    }\
}


/** macro that calls an object's event with 10 parameters
    @param OBJ pointer to AWE_OBJECT derived object to fire the event from
    @param NAME name of the event, as registered by the object's class
    @param P1 1st parameter
    @param P2 2nd parameter
    @param P3 3rd parameter
    @param P4 4th parameter
    @param P5 5th parameter
    @param P6 6th parameter
    @param P7 7th parameter
    @param P8 8th parameter
    @param P9 9th parameter
    @param P10 10th parameter
 */
#define awe_do_object_event10(OBJ, NAME, P1, P2, P3, P4, P5, P6, P7, P8, P9, P10) {\
    AWE_DL_NODE *node;\
    _AWE_SIGNAL *signal;\
    _AWE_SLOT *slot;\
    signal = _awe_find_signal((AWE_OBJECT *)OBJ, NAME);\
    if (signal) {\
        for(node = signal->slots.first; node; node = node->next) {\
            slot = (_AWE_SLOT *)((AWE_DL_DATA_NODE *)node)->data;\
            slot->proc(slot->object, P1, P2, P3, P4, P5, P6, P7, P8, P9, P10);\
        }\
    }\
}


/** checks if a class inherits from or is a specific class
    @param pclass class to check
    @param name NULL-terminated C string of the class' name
    @param pnamespace NULL-terminated C string of the class' namespace
    @return non-zero if the given class derives from or is of given class
 */
int awe_class_is_class(AWE_CLASS *pclass, const char *name, const char *pnamespace);


/** checks if an object is or derives from a specific class
    @param obj object to check
    @param name NULL-terminated C string of the class' name
    @param pnamespace NULL-terminated C string of the class' namespace
    @return non-zero if the given object derives from or is of given class
 */
int awe_object_is_class(AWE_OBJECT *obj, const char *name, const char *pnamespace);


/** retrieves an interface (a vtable pointer) from an object, calling the
    relevant method
    @param obj object to get an interface of
    @param name NULL-terminated C string name of the interface's name
    @param pnamespace NULL-terminated C string name of the interface's namespace
    @return a pointer to the requested interface or NULL if the interface is 
            not supported
 */
void *awe_get_object_interface(AWE_OBJECT *obj, const char *name, const char *pnamespace);


/** returns the class of an object
    @param obj object to get the class of
    @return the object's class
 */
AWE_CLASS *awe_get_object_class(AWE_OBJECT *obj);


/*@}*/


#ifdef __cplusplus
    }
#endif


#endif //AWE_COM_H
