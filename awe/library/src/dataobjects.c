#include "dataobjects.h"
#include <allegro.h>


/*****************************************************************************
    PRIVATE
 *****************************************************************************/


//text constructor
static void _text_constructor(AWE_OBJECT *obj)
{
    AWE_TEXT *text = (AWE_TEXT *)obj;
    text->text = ustrdup(empty_string);
}


//text destructor
static void _text_destructor(AWE_OBJECT *obj)
{
    AWE_TEXT *text = (AWE_TEXT *)obj;
    free(text->text);
}


//gets the text
static void _text_get(AWE_OBJECT *obj, void *data)
{
    AWE_TEXT *text = (AWE_TEXT *)obj;
    *(const char **)data = text->text;
}


//sets the text
static void _text_set(AWE_OBJECT *obj, void *data)
{
    const char *new_text = *(const char **)data;
    AWE_TEXT *text = (AWE_TEXT *)obj;
    free(text->text);
    text->text = ustrdup(new_text ? new_text : empty_string);
}


//text properties
static AWE_CLASS_PROPERTY _text_properties[] = {
    {AWE_ID_TEXT, "const char *", sizeof(const char *), _text_get, _text_set, 0},
    {0}
};


//text vtable
static AWE_OBJECT_VTABLE _text_vtable = {
    awe_object_get_interface,
    0,
    awe_text_clone,
};


/*****************************************************************************
    PUBLIC
 *****************************************************************************/


//a text object class
AWE_CLASS awe_text_class = {
    AWE_ID_TEXT,
    AWE_ID_AWE,
    &awe_object_class,
    sizeof(AWE_TEXT),
    _text_properties,
    0,
    &_text_vtable,
    _text_constructor,
    _text_destructor
};


//clones the text object
AWE_OBJECT *awe_text_clone(AWE_OBJECT *obj)
{
    AWE_TEXT *text = (AWE_TEXT *)obj;
    return awe_create_object(&awe_text_class,
        AWE_ID_TEXT, text->text,
        0);
}
