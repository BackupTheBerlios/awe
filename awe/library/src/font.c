#include "font.h"


/*****************************************************************************
    INTERNALS
 *****************************************************************************/


//font install count
static int _font_install_count = 0;


#ifdef TTFONT


//font entry structure
typedef struct _font_entry {
    char *path;
    FONT *font;
    int size;
    int counter;
    struct _font_entry *next;
} _FONT_ENTRY;


//list of loaded fonts
static _FONT_ENTRY *first_entry = NULL;


//enables or disables anti-aliased fonts
static int _font_aa = 0;


//returns the font height
static int _font_height(const FONT *font)
{ 
    return alfont_text_height(font->data); 
}


//returns the character length
static int _char_length(const FONT *f, int ch)
{
    char s[16];
    usetc(s, ch);
    usetat(s, -1, '\0');
    return alfont_text_length(f->data, s);    
}


//returns the length of the text
static int _text_length(const FONT *f, const char *text)
{
    return alfont_text_length(f->data, text);
}


//renders a single character
static int _render_char(const FONT *f, int ch, int fg, int bg, BITMAP *bmp, int x, int y)
{
    char s[16];
    usetc(s, ch);
    usetat(s, -1, '\0');
    alfont_text_mode(bg);
    if(_font_aa)
        alfont_textout_aa(bmp, f->data, s, x, y, fg);
    else
        alfont_textout(bmp, f->data, s, x, y, fg);
    return 1;
}


//renders a line of text
static void _render(const FONT *f, const char *text, int fg, int bg, BITMAP *bmp, int x, int y) {
    alfont_text_mode(bg);
    if(_font_aa)
        alfont_textout_aa(bmp, f->data, text, x, y, fg);
    else
        alfont_textout(bmp, f->data, text, x, y, fg);
}


//destroys font
static void _destroy(FONT *f) {
    alfont_destroy_font(f->data);
}


//alfont vtable entries
static FONT_VTABLE _alfont_vtable = {
    _font_height,
    _char_length,
    _text_length,
    _render_char,
    _render,
    _destroy
};

#endif


/*****************************************************************************
    PUBLIC
 *****************************************************************************/


//sets the font alias
void awe_set_font_alias(int alias)
{
    
    #ifdef TTFONT
        
    _font_aa = alias;
    
    #endif
}


//returns the font alias
int awe_get_font_alias(void)
{
    
    #ifdef TTFONT
    
    return _font_aa;
    
    #else
    
    return 0;
    
    #endif
}


//initializes the font system if font_count is 0, increments font_count.
int awe_install_font(void)
{
    if(_font_install_count == 0){
        
        #ifdef TTFONT

        if(alfont_init() != ALFONT_OK)
            return 0;
        
        #endif
    
        _font_install_count++;
        TRACE("Font: Initialization Successful\n");
        return 1;
    }
    _font_install_count++;
    return 0;
}


//adds a font to the list.
FONT *awe_load_font(const char *path, int size)
{

    #ifdef TTFONT

    ALFONT_FONT *alfont;
    _FONT_ENTRY *entry;
    FONT *new_font;

    /* first, check to see if font is already loaded */
    for(entry = first_entry; entry; entry = entry->next) {
        if (entry->size == size && !ustrcmp(path, entry->path)) {
            TRACE("Font: %s Already Loaded\n", path);
            /* add a reference if font is already loaded */
            entry->counter++;
            return entry->font;
        }    
    }
    
    /* try to load the alfont font */
    alfont = alfont_load_font(path);

    /* if it does not exist, return the default font */
    if (!alfont) {
        TRACE("Font: %s Failed to Load - Returning Default Font\n", path);
        return font;
    }
    /* set size */
    alfont_set_font_size(alfont, size);
    
    /* setup the allegro font */
    if((new_font = (FONT *)malloc(sizeof(FONT))) == NULL){
        TRACE("Font: %s Failed to Allocate New Font\n", path);
        return NULL;
    }
    
    new_font->height = alfont_text_height(alfont);
    new_font->vtable = &_alfont_vtable;
    new_font->data = alfont;
    
    /* add a new font entry */
    if((entry = (_FONT_ENTRY *)malloc(sizeof(_FONT_ENTRY))) == NULL){
        free(new_font);
        TRACE("Font: %s Failed to Allocate New Entry\n", path);
        return NULL;
    }
    entry->path = ustrdup(path);
    entry->size = alfont_text_height(alfont);
    entry->font = new_font;
    entry->counter = 1;
    entry->next = first_entry;
    first_entry = entry;
    TRACE("Font: %s Loaded Successfully\n", path);
    return new_font;

    #else

    return font;

    #endif
    
}


//adds a font to the list.
FONT *awe_load_memory_font(const char *data, int data_len, int size)
{

    #ifdef TTFONT

    ALFONT_FONT *alfont;
    FONT *new_font;
    
    /* try to load the alfont font */
    alfont = alfont_load_font_from_mem(data, data_len);

    /* if it does not exist, return the default font */
    if (!alfont) {
        TRACE("Font: Failed to Load - Returning Default Font\n");
        return font;
    }
    /* set size */
    alfont_set_font_size(alfont, size);
    
    /* setup the allegro font */
    if((new_font = (FONT *)malloc(sizeof(FONT))) == NULL){
        TRACE("Font: Failed to Allocate New Font\n");
        return NULL;
    }
    
    new_font->height = alfont_text_height(alfont);
    new_font->vtable = &_alfont_vtable;
    new_font->data = alfont;
    
    TRACE("Font: Loaded Successfully\n");
    return new_font;

    #else

    return font;

    #endif
    
}


//removes a font from the cache, returns 1 if successful, 0 otherwise.
int awe_unload_font(FONT *font)
{
    #ifdef TTFONT

    _FONT_ENTRY *entry, *prev;
    
    /* find the font entry; font can be used as key since it is unique */
    prev = NULL;
    for(entry = first_entry; entry; entry = entry->next) {
        if (entry->font == font)
            break;
        prev = entry;
    }
    
    /* check if font is found */
    if (entry == NULL) {
        TRACE("Font: Font Does Not Exist\n");
        return 0;
    }
    
    /* del ref */
    entry->counter--;
    
    /* remove entry and unload font if not needed */
    if (entry->counter == 0) {
        if (prev) prev->next = entry->next; else first_entry = entry->next;
        free(entry->path);
        free(entry);
        destroy_font(font);
    }
    TRACE("Font: Font Destroyed Successfully\n");

    #endif

    return 1;     
}


//cleans up and shuts down the font system if font_count is 1, decrements font_count.
void awe_font_exit(void)
{
    if(_font_install_count == 1){

        #ifdef TTFONT
        
        _FONT_ENTRY *entry, *next;
        for(entry = first_entry; entry; ) {
            next = entry->next;
            free(entry->path);
            destroy_font(entry->font);
            free(entry);
            entry = next;
            TRACE("Font: Font Destroyed Successfully\n");
        }
        alfont_exit();
        TRACE("Font: Shutdown Successful\n");

        #endif

    }
    _font_install_count--;
}

