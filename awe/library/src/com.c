#include "com.h"
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>


/*****************************************************************************
    PRIVATE
 *****************************************************************************/


//namespace node
typedef struct _NAMESPACE_NODE {
    AWE_DL_NODE node;
    char *name;
    AWE_DL_LIST classes;
} _NAMESPACE_NODE;


//class node
typedef struct _CLASS_NODE {
    AWE_DL_NODE node;
    AWE_CLASS *pclass;
    int ref;
} _CLASS_NODE;


//registry proc node
typedef struct _PROC_NODE {
    AWE_DL_NODE node;
    AWE_CLASS_REGISTRY_PROC proc;
    int ref;
} _PROC_NODE;


//registered namespaces
static AWE_DL_LIST _namespaces = {0, 0};


//registered procs
static AWE_DL_LIST _procs = {0, 0};


//finds a slot from a signal
static _AWE_SLOT *_find_slot(_AWE_SIGNAL *signal, AWE_OBJECT *dst, void *proc)
{
    AWE_DL_NODE *node;
    _AWE_SLOT *slot;

    for(node = signal->slots.first; node; node = node->next) {
        slot = (_AWE_SLOT *)((AWE_DL_DATA_NODE *)node)->data;
        if (slot->object == dst && slot->proc == proc) return slot;
    }
    return 0;
}


//destroys a slot
static void _destroy_slot(_AWE_SLOT *slot)
{
    if (slot->slot_list) awe_list_remove(slot->slot_list, &slot->slot_node.node);
    awe_list_remove(slot->signal_list, &slot->signal_node.node);
    free(slot);
}


//finds a signal
_AWE_SIGNAL *_awe_find_signal(AWE_OBJECT *obj, const char *name)
{
    AWE_DL_NODE *node;
    _AWE_SIGNAL *signal;

    for(node = obj->signals.first; node; node = node->next) {
        signal = (_AWE_SIGNAL *)node;
        if (strcmp(signal->name, name) == 0) return signal;
    }
    return 0;
}


//destroys a signal
static void _destroy_signal(_AWE_SIGNAL *signal)
{
    AWE_DL_NODE *node, *next;
    _AWE_SLOT *slot;

    node = signal->slots.first;
    while (node) {
        next = node->next;
        slot = (_AWE_SLOT *)((AWE_DL_DATA_NODE *)node)->data;
        _destroy_slot(slot);
        node = next;
    }
    free(signal);
}


//finds an event from a class
static AWE_CLASS_EVENT *_find_event(const char *name, AWE_CLASS *pclass)
{
    AWE_CLASS_EVENT *event;

    if (pclass->events) {
        for(event = pclass->events; event->name; event++) {
            if (strcmp(event->name, name) == 0) return event;
        }
    }
    return pclass->super ? _find_event(name, pclass->super) : 0;
}


//finds a namespace
static _NAMESPACE_NODE *_find_namespace(const char *pnamespace)
{
    _NAMESPACE_NODE *namespace_node;
    AWE_DL_NODE *node;

    for(node = _namespaces.first; node; node = node->next) {
        namespace_node = (_NAMESPACE_NODE *)node;
        if (strcmp(namespace_node->name, pnamespace) == 0) return namespace_node;
    }
    return 0;
}


//finds a class node
static _CLASS_NODE *_find_class(_NAMESPACE_NODE *namespace_node, const char *name)
{
    _CLASS_NODE *class_node;
    AWE_DL_NODE *node;

    for(node = namespace_node->classes.first; node; node = node->next) {
        class_node = (_CLASS_NODE *)node;
        if (strcmp(class_node->pclass->name, name) == 0) return class_node;
    }
    return 0;
}


//calls all registry procs with given event
static void _call_registry_procs(AWE_CLASS *pclass, AWE_CLASS_REGISTRY_EVENT_TYPE event)
{
    _PROC_NODE *proc_node;
    AWE_DL_NODE *node;

    for(node = _procs.first; node; node = node->next) {
        proc_node = (_PROC_NODE *)node;
        proc_node->proc(pclass, event);
    }    
}


//finds a registry proc
static _PROC_NODE *_find_registry_proc(AWE_CLASS_REGISTRY_PROC proc)
{
    _PROC_NODE *proc_node;
    AWE_DL_NODE *node;

    for(node = _procs.first; node; node = node->next) {
        proc_node = (_PROC_NODE *)node;
        if (proc_node->proc == proc) return proc_node;
    }    
    return 0;
}


static void *_awe_object_get_interface(AWE_OBJECT *obj, const char *name, const char *pnamespace);
static AWE_OBJECT *_awe_object_clone(AWE_OBJECT *obj);


//object vtable
static AWE_OBJECT_VTABLE _awe_object_vtable = {
    _awe_object_get_interface,
    NULL,
    _awe_object_clone
};


//returns interface
static void *_awe_object_get_interface(AWE_OBJECT *obj, const char *name, const char *pnamespace)
{
    if (!strcmp(pnamespace, "AWE")) {
        if (!strcmp(name, "Object")) return &_awe_object_vtable;
    }
    return 0;
}


//clones an object
static AWE_OBJECT *_awe_object_clone(AWE_OBJECT *obj)
{
    return awe_create_object(&awe_object_class, NULL);
}


//calls constructors
static void _construct_object(AWE_OBJECT *obj, AWE_CLASS *pclass)
{
    if (pclass->super) _construct_object(obj, pclass->super);
    if (!pclass->constructor) return;
    obj->pclass = pclass;
    pclass->constructor(obj);
}


//finds a property
static AWE_CLASS_PROPERTY *_find_property(const char *name, AWE_CLASS *pclass)
{
    AWE_CLASS_PROPERTY *prop;

    if (pclass->properties) {
        for(prop = pclass->properties; prop->name; prop++) {
            if (strcmp(prop->name, name) == 0) return prop;
        }
    }
    return pclass->super ? _find_property(name, pclass->super) : 0;
}


/*****************************************************************************
    PUBLIC
 *****************************************************************************/


//base class for all objects
AWE_CLASS awe_object_class = {
    "Object",                //name
    "AWE",                   //namespace
    NULL,                    //superclass
    sizeof(AWE_OBJECT),      //struct size
    NULL,                    //properties
    NULL,                    //events
    &_awe_object_vtable,     //vtable
    NULL,                    //constructor
    NULL                     //destructor
};


//finds a class from name and namespace
AWE_CLASS *awe_find_class(const char *name, const char *pnamespace)
{
    _NAMESPACE_NODE *namespace_node;
    _CLASS_NODE *class_node;

    namespace_node = _find_namespace(pnamespace);
    if (!namespace_node) return 0;
    class_node = _find_class(namespace_node, name);
    return class_node ? class_node->pclass : 0;
}


//enumerates classes
void awe_enum_classes(AWE_CLASS_REGISTRY_ENUM_PROC proc, void *data)
{
    _NAMESPACE_NODE *namespace_node;
    _CLASS_NODE *class_node;
    AWE_DL_NODE *outter_node, *inner_node;

    for(outter_node = _namespaces.first; outter_node; outter_node = outter_node->next) {
        namespace_node = (_NAMESPACE_NODE *)outter_node;
        for(inner_node = namespace_node->classes.first; inner_node; inner_node = inner_node->next) {
            class_node = (_CLASS_NODE *)inner_node;
            if (!proc(class_node->pclass, data)) return;
        }
    }
}


//registers a class
void awe_register_class(AWE_CLASS *pclass)
{
    _NAMESPACE_NODE *namespace_node;
    _CLASS_NODE *class_node = 0;

    //find class node
    namespace_node = _find_namespace(pclass->pnamespace);

    //find class if namespace exists
    if (namespace_node) {
        class_node = _find_class(namespace_node, pclass->name);

        //if the class is found, just increase its ref count and notify procs
        if (class_node) {
            class_node->ref++;
            return;
        }
    }

    //else install a new namespace if given namespace does not exist
    else {
        namespace_node = (_NAMESPACE_NODE *)calloc(1, sizeof(_NAMESPACE_NODE));
        namespace_node->name = strdup(pclass->pnamespace);
        awe_list_insert(&_namespaces, &namespace_node->node, 0);
    }

    //install a new class
    class_node = (_CLASS_NODE *)calloc(1, sizeof(_CLASS_NODE));
    class_node->pclass = pclass;
    class_node->ref = 1;
    awe_list_insert(&namespace_node->classes, &class_node->node, 0);

    //notify procs
    _call_registry_procs(class_node->pclass, AWE_CLASS_ADDED);
}


//unregisters a class
void awe_unregister_class(AWE_CLASS *pclass)
{
    _NAMESPACE_NODE *namespace_node;
    _CLASS_NODE *class_node;

    //find class node
    namespace_node = _find_namespace(pclass->pnamespace);
    if (!namespace_node) return;
    class_node = _find_class(namespace_node, pclass->name);
    if (!class_node) return;

    //del ref
    class_node->ref--;

    //don't remove class if ref is not 0
    if (class_node->ref > 0) return;

    //remove class
    awe_list_remove(&namespace_node->classes, &class_node->node);
    free(class_node);

    //if the namespace has no classes, remove it
    if (namespace_node->classes.first == namespace_node->classes.last == 0) {
        awe_list_remove(&_namespaces, &namespace_node->node);
        free(namespace_node->name);
        free(namespace_node);
    }

    //notify the environment
    _call_registry_procs(pclass, AWE_CLASS_REMOVED);
}


//hooks the class registry
void awe_add_class_registry_proc(AWE_CLASS_REGISTRY_PROC proc)
{
    _PROC_NODE *proc_node;

    //check if the proc is already added
    proc_node = _find_registry_proc(proc);

    //increase ref if exists
    if (proc_node) {
        proc_node->ref++;
        return;
    }

    //else add proc
    proc_node = (_PROC_NODE *)calloc(1, sizeof(_PROC_NODE));
    proc_node->proc = proc;
    proc_node->ref = 1;
    awe_list_insert(&_procs, &proc_node->node, 0);
}


//unhooks the class registry
void awe_remove_class_registry_proc(AWE_CLASS_REGISTRY_PROC proc)
{
    _PROC_NODE *proc_node;

    //find proc
    proc_node = _find_registry_proc(proc);
    if (!proc_node) return;

    //del ref
    proc_node->ref--;

    //remove node if ref reached 0
    if (proc_node->ref == 0) {
        awe_list_remove(&_procs, &proc_node->node);
        free(proc_node);
    }
}


//creates an object instance from a va list
AWE_OBJECT *awe_create_object_va(AWE_CLASS *pclass, va_list params)
{
    AWE_OBJECT *obj;

    obj = (AWE_OBJECT *)calloc(1, pclass->size);
    if (!obj) return 0;
    _construct_object(obj, pclass);
    awe_set_object_properties_va(obj, params);
    return obj;
}


//creates an object instance
AWE_OBJECT *awe_create_object(AWE_CLASS *pclass, ...)
{
    AWE_OBJECT *result;
    va_list params;

    va_start(params, pclass);
    result = awe_create_object_va(pclass, params);
    va_end(params);
    return result;
}


//creates an object instance by class name and namespace
AWE_OBJECT *awe_create_object_by_name(const char *name, const char *pnamespace, ...)
{
    AWE_OBJECT *result;
    AWE_CLASS *pclass;
    va_list params;

    pclass = awe_find_class(name, pnamespace);
    if (!pclass) return 0;
    va_start(params, pnamespace);
    result = awe_create_object_va(pclass, params);
    va_end(params);
    return result;
}


//creates an object instance by class name and namespace
AWE_OBJECT *awe_create_object_by_name_va(const char *name, const char *pnamespace, va_list params)
{
    AWE_OBJECT *result;
    AWE_CLASS *pclass;

    pclass = awe_find_class(name, pnamespace);
    if (!pclass) return 0;
    result = awe_create_object_va(pclass, params);
    return result;
}


//clones an object
AWE_OBJECT *awe_clone_object(AWE_OBJECT *obj)
{
    return obj->pclass->vtable->clone ? obj->pclass->vtable->clone(obj) : 0;
}


//destroys an object
void awe_destroy_object(AWE_OBJECT *obj)
{
    AWE_DL_NODE *node, *next;
    _AWE_SIGNAL *signal;
    _AWE_SLOT *slot;

    //call destructors
    for(; obj->pclass; obj->pclass = obj->pclass->super) {
        if (obj->pclass->destructor) obj->pclass->destructor(obj);
    }

    //free signals
    node = obj->signals.first;
    while (node) {
        next = node->next;
        signal = (_AWE_SIGNAL *)node;
        _destroy_signal(signal);
        node = next;
    }

    //free slots
    node = obj->slots.first;
    while (node) {
        next = node->next;
        slot = (_AWE_SLOT *)((AWE_DL_DATA_NODE *)node)->data;
        _destroy_slot(slot);
        node = next;
    }    

    //free object
    free(obj);
}


//gets an object's properties
int awe_get_object_properties_va(AWE_OBJECT *obj, va_list params)
{
    AWE_CLASS_PROPERTY *prop;
    const char *prop_name;
    int count = 0;
    void *buffer;

    while (1) {
        prop_name = va_arg(params, const char *);
        if (!prop_name) break;
        buffer = va_arg(params, void *);
        prop = _find_property(prop_name, obj->pclass);
        if (!prop || !prop->get) continue;
        prop->get(obj, buffer);
        count++;
    }

    return count;
}


//gets an object's properties
int awe_get_object_properties(AWE_OBJECT *obj, ...)
{
    va_list params;
    int result;

    va_start(params, obj);
    result = awe_get_object_properties(obj, params);
    va_end(params);
    return result;
}


//sets an object's properties
int awe_set_object_properties_va(AWE_OBJECT *obj, va_list params)
{
    AWE_CLASS_PROPERTY *prop;
    const char *prop_name;
    int count = 0;
    char value[256];

    while (1) {
        //get property name
        prop_name = va_arg(params, const char *);
        if (!prop_name) break;

        //find property; if not found, stop setting properties
        prop = _find_property(prop_name, obj->pclass);
        if (!prop) break;

        //get value from the stack
        memcpy(value, params, prop->stack_size);
        params = ((char *)params) + prop->stack_size;

        //set the property
        if (!prop->set) continue;
        prop->set(obj, value);
        count++;
    }

    //notify the object if properties have changed
    if (count && obj->pclass->vtable->properties_changed)
        obj->pclass->vtable->properties_changed(obj);

    //return number of set properties
    return count;
}


//sets an object's properties
int awe_set_object_properties(AWE_OBJECT *obj, ...)
{
    va_list params;
    int result;

    va_start(params, obj);
    result = awe_set_object_properties_va(obj, params);
    va_end(params);
    return result;
}


//registers an event with an object
int awe_add_object_event(AWE_OBJECT *src, const char *name, AWE_OBJECT *dst, void *proc)
{
    AWE_CLASS_EVENT *event;
    _AWE_SIGNAL *signal;
    _AWE_SLOT *slot;

    //find signal
    signal = _awe_find_signal(src, name);

    //if found, check if the slot already exists; if it exists, do nothing
    if (signal) {
        slot = _find_slot(signal, dst, proc);
        if (slot) return 1;
    }

    //if not found, install a new signal in the object (if the event exists)
    if (!signal) {
        event = _find_event(name, src->pclass);
        if (!event) return 0;
        signal = (_AWE_SIGNAL *)calloc(1, sizeof(_AWE_SIGNAL));
        signal->name = event->name;
        awe_list_insert(&src->signals, &signal->node, 0);
    }

    //install a new slot in the src signal and in the dst object
    slot = (_AWE_SLOT *)calloc(1, sizeof(_AWE_SLOT));
    slot->signal_node.data = slot;
    slot->slot_node.data = slot;
    slot->signal_list = &signal->slots;
    if (dst) slot->slot_list = &dst->slots;
    slot->object = dst;
    slot->proc = proc;
    awe_list_insert(&signal->slots, &slot->signal_node.node, 0);
    if (dst) awe_list_insert(&dst->slots, &slot->slot_node.node, 0);

    return 1;
}


//unregisters an event with an object
int awe_remove_object_event(AWE_OBJECT *src, const char *name, AWE_OBJECT *dst, void *proc)
{
    _AWE_SIGNAL *signal;
    _AWE_SLOT *slot;

    //find signal
    signal = _awe_find_signal(src, name);
    if (!signal) return 0;

    //find slot
    slot = _find_slot(signal, dst, proc);
    if (!slot) return 0;

    //destroy the slot
    _destroy_slot(slot);

    //if the signal is empty, destroy it
    if (signal->slots.first == signal->slots.last == 0) {
        _destroy_signal(signal);
    }

    return 1;
}


//checks if a class is a or inherits from a specific class
int awe_class_is_class(AWE_CLASS *pclass, const char *name, const char *pnamespace)
{
    for(; pclass; pclass = pclass->super) {
        if (strcmp(pclass->name, name) == 0 &&
            strcmp(pclass->pnamespace, pnamespace) == 0)
            return 1;
    }
    return 0;
}


//checks if an object is a or inherits from a specific class
int awe_object_is_class(AWE_OBJECT *obj, const char *name, const char *pnamespace)
{
    return awe_class_is_class(obj->pclass, name, pnamespace);
}


//retrieves an interface from an object
void *awe_get_object_interface(AWE_OBJECT *obj, const char *name, const char *pnamespace)
{
    return obj->pclass->vtable->get_interface ?
           obj->pclass->vtable->get_interface(obj, name, pnamespace) : 
           0;
}


/** returns the class of an object
    @param obj object to get the class of
    @return the object's class
 */
AWE_CLASS *awe_get_object_class(AWE_OBJECT *obj)
{
    return obj->pclass;
}
