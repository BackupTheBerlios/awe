#ifndef AWE_DATAOBJECTS_H
#define AWE_DATAOBJECTS_H


#include "com.h"


#ifdef __cplusplus
    extern "C" {
#endif


struct AWE_CLASS;


/**@name Data Objects
    <p> This module contains COM classes that are data objects and are used
        in drag-and-drop, clipboard and other application-specific places.
    </p>
 */
/*@{*/


///text class/property name
#define AWE_ID_TEXT             "Text"


///a Allegro text data object; inherits from 'Object'; it has a property 'Text'
struct AWE_TEXT {
    AWE_OBJECT object;
    char *text;
};
typedef struct AWE_TEXT AWE_TEXT;


//a text object class
extern AWE_CLASS awe_text_class;


/** the default implementation of the text clone method
    @param obj object to clone
    @return the cloned object
 */
AWE_OBJECT *awe_text_clone(AWE_OBJECT *obj);


/*@}*/


#ifdef __cplusplus
    }
#endif


#endif //AWE_DATAOBJECTS_H
