#ifndef AWE_SKIN_H
#define AWE_SKIN_H


#include <allegro.h>
#include <string.h>
#include "gdi.h"
#include "com.h"
#include "symbintree.h"
#include "mouse.h"
#include "font.h"


#ifdef __cplusplus
   extern "C" {
#endif


/**@name Skin
    <p>The purpose of the Skin module is to provide an easy way to manage and load themes.</p>
 */
/*@{*/


/** skin structure
 */
struct AWE_SKIN {
    /// skin datafile
    DATAFILE *dat;

    /// object list loaded from datafile
    AWE_SBT_NODE *obj_list;
};
typedef struct AWE_SKIN AWE_SKIN;


/** skin vtable
 */
struct AWE_SKIN_VTABLE {
    /// apply skin function
    void (*apply_skin)(AWE_OBJECT *obj, AWE_SKIN *skn);
};
typedef struct AWE_SKIN_VTABLE AWE_SKIN_VTABLE;


/** loads a skin
    @param filename path to skin
    @return returns a valid AWE_SKIN pointer, otherwise NULL
 */
AWE_SKIN *awe_load_skin(const char *filename);


/** unloads a skin
    @param skn valid AWE_SKIN pointer
 */
void awe_unload_skin(AWE_SKIN *skn);


/** retrieves a skin texture
    @param skn valid AWE_SKIN pointer
    @param name name of texture
    @return returns a valid AWE_TEXTURE pointer, otherwise NULL
 */
AWE_TEXTURE *awe_get_skin_texture(AWE_SKIN *skn, const char *name);


/** retrieves a skin RGB structure
    @param skn valid AWE_SKIN pointer
    @param name name of RGB structure
    @return returns a valid RGB pointer, otherwise NULL
 */
RGB *awe_get_skin_color(AWE_SKIN *skn, const char *name);


/** retrieves a skin FONT structure
    @param skn valid AWE_SKIN pointer
    @param name name of FONT structure
    @return returns a valid FONT pointer, otherwise the default font
 */
FONT *awe_get_skin_font(AWE_SKIN *skn, const char *name);


/** retrieves a skin BITMAP structure
    @param skn valid AWE_SKIN pointer
    @param name name of BITMAP structure
    @return returns a valid BITMAP pointer, otherwise NULL
 */
BITMAP *awe_get_skin_bitmap(AWE_SKIN *skn, const char *name);


/** retrieves a skin binary data structure
    @param skn valid binary data pointer
    @param name name of a binary data structure
    @return returns a valid binary data pointer, otherwise NULL
 */
void *awe_get_skin_data(AWE_SKIN *skn, const char *name);


/** retrieves a skin integer
    @param skn valid AWE_SKIN pointer
    @param name name of integer
    @param num default value if integer value not found
    @return returns a valid integer, otherwise num
 */
int awe_get_skin_int(AWE_SKIN *skn, const char *name, int num);


/*@}*/


#ifdef __cplusplus
   }
#endif


#endif // AWE_SKIN_H
