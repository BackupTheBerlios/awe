#ifndef AWE_FONT_H
#define AWE_FONT_H


#include <allegro.h>
#include <allegro/internal/aintern.h>


#ifdef TTFONT
    #include <alfont.h>
#endif


#ifdef __cplusplus
   extern "C" {
#endif


/**@name Font
    <p>The purpose of the Font module is to provide an easy way to manage and load multiple fonts. 
       It offers the following additional functionality to Allegro:</p>
    <LL>
    <LI>Loading of True Type Fonts; the system allows you to load true type fonts using the standared Allegro
        font structure.  The system also manages all the fonts for you.
    </LI>
    </LL>
    <p>The Font module is an independent library.</p>
 */
/*@{*/


/** sets the font alias
    @param alias if true then the fonts will be antialiased
 */
void awe_set_font_alias(int alias);


/** returns the font alias
    @return returns the current font alias
 */
int awe_get_font_alias(void);


/** initializes the font system if font_count is 0, increments font_count
    @return returns 1 on success, 0 otherwise
 */
int awe_install_font(void);


/** loads a font, if the font isn't in the list it will be loaded and returned
    @return returns the loaded font or default font upon error
 */
FONT *awe_load_font(const char *path, int size);


/** loads a font from memory
    @return returns the loaded font or default font upon error
 */
FONT *awe_load_memory_font(const char *data, int data_len, int size);


/** removes a font from the cache
    @return returns 1 if successful, 0 otherwise
 */
int awe_unload_font(FONT *font);


/** cleans up and shuts down the font system if font_count is 1, decrements font_count
 */
void awe_font_exit(void);


/*@}*/


#ifdef __cplusplus
   }
#endif


#endif // AWE_FONT_H
