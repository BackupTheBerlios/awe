#include "skin.h"

/*****************************************************************************
    INTERNALS
 *****************************************************************************/


#define BIG_ENDIAN      0
#define LITTLE_ENDIAN   1


/** skin object types
 */
enum _SKIN_OBJECT_TYPE {
    SKIN_BITMAP = DAT_ID('B', 'M', 'P', ' '),
    SKIN_TEXTURE = DAT_ID('T', 'X', 'T', 'R'),
    SKIN_FONT = DAT_ID('F', 'O', 'N', 'T'),
    SKIN_RGB = DAT_ID('R', 'G', 'B', ' '),
    SKIN_INT = DAT_ID('I', 'N', 'T', ' ')
};
typedef enum _SKIN_OBJECT_TYPE _SKIN_OBJECT_TYPE;


/** skin object
 */
struct _SKIN_OBJECT {
    /* Object Type */
    _SKIN_OBJECT_TYPE type;
    /* Origional Datafile Index */
    int skin_idx;
    /* Object Name */
    char *name;
    /* Object Data */
    void *data;
    /* Data Destroy Callback */
    void (*destroy_object)(void *data);
};
typedef struct _SKIN_OBJECT _SKIN_OBJECT;


struct _BOARDER {
    int left;
    int top;
    int right;
    int bottom;
};
typedef struct _BOARDER _BOARDER;


static void _skin_delete_func(const void *data){
    _SKIN_OBJECT *obj = (_SKIN_OBJECT*)data;
    TRACE("Skin: Destroying %s\n", obj->name);
    if(obj->destroy_object)
        obj->destroy_object(obj->data);
    if(obj->name)
        free(obj->name);
	free(obj);
}


static int _skin_compare_func(const void *a, const void *b){
    _SKIN_OBJECT *obj1 = (_SKIN_OBJECT*)a;
    _SKIN_OBJECT *obj2 = (_SKIN_OBJECT*)b;
    return ustrcmp(obj1->name, obj2->name);
}


static void _skin_destroy_texture(void *data){
    awe_destroy_texture((AWE_TEXTURE*)data);
}


static void _skin_destroy_font(void *data){
    destroy_font((FONT*)data);
}


static int _get_dat_idx(DATAFILE *dat, const char *name){
    int i;
    for(i = 0; dat[i].type != DAT_END; i++){
        if(ustricmp(get_datafile_property(dat + i, DAT_ID('N', 'A', 'M', 'E')), name) == 0)
            return i;
    }
    return -1;
}


static int _get_dat_int(DATAFILE *dat, int idx, int prop, int val){
    if(idx > 0){
        const char *tmp;
        tmp = get_datafile_property(dat + idx, prop);
        if(*tmp == '\0')
	    return val;
        return atoi(tmp);
    }
    return val;
}


static void _get_dat_boarder(DATAFILE *dat, _BOARDER *boarder, int idx){
    const char *tmp;
    tmp = get_datafile_property(dat + idx, DAT_ID('B', 'R', 'D', 'L'));
    if(*tmp == '\0')
        boarder->left = 0;
    else
        boarder->left = atoi(tmp);
    tmp = get_datafile_property(dat + idx, DAT_ID('B', 'R', 'D', 'T'));
    if(*tmp == '\0')
        boarder->top = 0;
    else
        boarder->top = atoi(tmp);
    tmp = get_datafile_property(dat + idx, DAT_ID('B', 'R', 'D', 'R'));
    if(*tmp == '\0')
        boarder->right = 0;
    else
        boarder->right = atoi(tmp);
    tmp = get_datafile_property(dat + idx, DAT_ID('B', 'R', 'D', 'B'));
    if(*tmp == '\0')
        boarder->bottom = 0;
    else
        boarder->bottom = atoi(tmp);
}


static _SKIN_OBJECT *_get_skin_object(AWE_SKIN *skn, const char *name){
    _SKIN_OBJECT tmp;
    tmp.name = (char*)name;
    tmp.data = NULL;
    return awe_sbt_find(skn->obj_list, &tmp, _skin_compare_func);
}


static int _test_byte_order(){
   short int word = 0x0001;
   char *byte = (char *) &word;
   return(byte[0] ? LITTLE_ENDIAN : BIG_ENDIAN);
}


static unsigned long _byte_swap(unsigned long nLongNumber){
    return (((nLongNumber & 0x000000FF) << 24) + ((nLongNumber & 0x0000FF00) << 8) +
    ((nLongNumber & 0x00FF0000) >> 8) + ((nLongNumber & 0xFF000000) >> 24));
}


/*****************************************************************************
    PUBLIC
 *****************************************************************************/


AWE_SKIN *awe_load_skin(const char *filename){
    AWE_SKIN *skn;
    _SKIN_OBJECT *obj = NULL;
    int i, alpha = 0;
    const char *name;
    skn = (AWE_SKIN*)malloc(sizeof(AWE_SKIN));
    if(!skn)
        goto _skin_error;
    if(filename){
        skn->dat = load_datafile(filename);
        if(skn->dat){
            i = _get_dat_idx(skn->dat, "ALPHA_CFG");
            alpha = _get_dat_int(skn->dat, i, DAT_ID('C', 'F', 'G', 'A'), 0);                
        }
    }
    /* Set color conversion if alpha channel is present */
    if(alpha){
        unload_datafile(skn->dat);
        set_color_conversion(COLORCONV_EXPAND_HI_TO_TRUE);
        skn->dat = load_datafile(filename);
    }
    if(!skn->dat)
        goto _skin_error;
    /* Load cursor */
    awe_load_datafile_mouse(skn->dat);
    skn->obj_list = awe_sbt_init();
    for(i = 0; skn->dat[i].type != DAT_END; i++){
        name = get_datafile_property(skn->dat + i, DAT_ID('A', 'W', 'E', ' '));
        if(*name == '\0')
            continue;
        /* Skip references */
        if(skn->dat[i].type == DAT_ID('R', 'E', 'F', ' '))
            continue;
        obj = (_SKIN_OBJECT*)malloc(sizeof(_SKIN_OBJECT));
        if(!obj)
            goto _skin_error;
        obj->skin_idx = i;
        obj->name = (char*)malloc(ustrlen(name) + uwidth_max(U_CURRENT));
        if(!obj->name)
            goto _skin_error;
        ustrcpy(obj->name, name);
        switch(skn->dat[i].type){
            case DAT_ID('B', 'M', 'P', ' '):
            case DAT_ID('T', 'G', 'A', ' '):
            case DAT_ID('P', 'C', 'X', ' '):
            #ifdef LOADPNG
            case DAT_ID('P', 'N', 'G', ' '):
            #endif
                name = get_datafile_property(skn->dat + i, DAT_ID('T', 'Y', 'P', 'E'));
                if(ustricmp(name, "texture") == 0){
                    _BOARDER boarder;
                    TRACE("Skin: Loading %s as texture\n", obj->name);
                    /* Create texture */
                    _get_dat_boarder(skn->dat, &boarder, i);
                    obj->type = SKIN_TEXTURE;
                    (AWE_TEXTURE*)obj->data = awe_create_texture((BITMAP*)skn->dat[i].dat, 0, 0, ((BITMAP*)(skn->dat[i].dat))->w, ((BITMAP*)(skn->dat[i].dat))->h, boarder.left, boarder.top, boarder.right, boarder.bottom);
                    if(!obj->data)
                        goto _skin_error;
                    obj->destroy_object = _skin_destroy_texture;
                }
                else{
                    TRACE("Skin: Loading %s as bitmap\n", obj->name);
                    obj->type = SKIN_BITMAP;
                    obj->data = skn->dat[i].dat;
                    obj->destroy_object = NULL;
                }
            break;
            case DAT_ID('T', 'T', 'F', ' '):
                TRACE("Skin: Loading %s as true type font\n", obj->name);
                name = get_datafile_property(skn->dat + i, DAT_ID('S', 'I', 'Z', 'E'));
                obj->type = SKIN_FONT;
                obj->data = awe_load_memory_font((const char *)skn->dat[i].dat, skn->dat[i].size, *name == '\0' ? 12 : atoi(name));
                if(obj->data == font)
	            obj->destroy_object = NULL;
		else
		    obj->destroy_object = _skin_destroy_font;
            break;
            default:
                TRACE("Skin: Loading %s as binary\n", obj->name);
                obj->type = skn->dat[i].type;
                obj->data = skn->dat[i].dat;
                obj->destroy_object = NULL;
            break;
        }
        awe_sbt_insert(&skn->obj_list, obj, _skin_compare_func);
    }
    for(i = 0; skn->dat[i].type != DAT_END; i++){
        name = get_datafile_property(skn->dat + i, DAT_ID('A', 'W', 'E', ' '));
        if(*name == '\0')
            continue;
        /* Load references */
        if(skn->dat[i].type == DAT_ID('R', 'E', 'F', ' ')){
            _SKIN_OBJECT *tmp;
            obj = (_SKIN_OBJECT*)malloc(sizeof(_SKIN_OBJECT));
            if(!obj)
                goto _skin_error;
            obj->skin_idx = i;
            obj->name = (char*)malloc(ustrlen(name) + uwidth_max(U_CURRENT));
            if(!obj->name)
                goto _skin_error;
            ustrcpy(obj->name, name);
            name = get_datafile_property(skn->dat + i, DAT_ID('T', 'Y', 'P', 'E'));
            if(ustricmp(name, "texture") == 0){
                _BOARDER boarder;
                int num_frames;
                int cur_frame;
                TRACE("Skin: Loading %s as texture\n", obj->name);
                /* Create texture */
                _get_dat_boarder(skn->dat, &boarder, i);
                obj->type = SKIN_TEXTURE;
                name = get_datafile_property(skn->dat + i, DAT_ID('S', 'K', 'I', 'D'));
                if(*name == '\0'){
                    /* Invalid skin id, continue on */
                    TRACE("Skin: Warning - No Reference Specified\n");
                    free(obj->name);
                    free(obj);
                    continue;
                }
                tmp = _get_skin_object(skn, name);
                if(!tmp){
                    TRACE("Skin: Warning - Reference %s Not Found\n", name);
                    free(obj->name);
                    free(obj);
                    continue;
                }
                if(tmp->type != SKIN_BITMAP){
                    TRACE("Skin: Warning - Reference Not Of Type Bitmap\n");
                    free(obj->name);
                    free(obj);
                    continue;
                }
                name = get_datafile_property(skn->dat + tmp->skin_idx, DAT_ID('N', 'F', 'R', 'M'));
                if(*name == '\0')
                    num_frames = 1;
                else
                    num_frames = atoi(name);
                name = get_datafile_property(skn->dat + i, DAT_ID('F', 'R', 'M', 'E'));
                if(*name == '\0')
                    cur_frame = 0;
                else
                    cur_frame = atoi(name);
                name = get_datafile_property(skn->dat + i, DAT_ID('T', 'I', 'L', 'E'));
                if(*name == '\0' || ustricmp(name, "Horizontal") == 0){
                    (AWE_TEXTURE*)obj->data = awe_create_texture((BITMAP*)tmp->data, cur_frame * (((BITMAP*)(tmp->data))->w / num_frames), 0, ((BITMAP*)(tmp->data))->w / num_frames, ((BITMAP*)(tmp->data))->h, boarder.left, boarder.top, boarder.right, boarder.bottom);
                    if(!obj->data)
                        goto _skin_error;
                }
                else{
                    (AWE_TEXTURE*)obj->data = awe_create_texture((BITMAP*)tmp->data, 0, cur_frame * (((BITMAP*)(tmp->data))->h / num_frames), ((BITMAP*)(tmp->data))->w, ((BITMAP*)(tmp->data))->h / num_frames, boarder.left, boarder.top, boarder.right, boarder.bottom);
                    if(!obj->data)
                        goto _skin_error;
                }
                obj->destroy_object = _skin_destroy_texture;
                awe_sbt_insert(&skn->obj_list, obj, _skin_compare_func);
            }
        }
    }
    for(i = 0; skn->dat[i].type != DAT_END; i++){
        name = get_datafile_property(skn->dat + i, DAT_ID('A', 'W', 'E', ' '));
        if(*name == '\0')
            continue;
        /* Load references */
        if(skn->dat[i].type == DAT_ID('R', 'E', 'F', ' ')){
            _SKIN_OBJECT *tmp;
            obj = (_SKIN_OBJECT*)malloc(sizeof(_SKIN_OBJECT));
            if(!obj)
                goto _skin_error;
            obj->skin_idx = i;
            obj->name = (char*)malloc(ustrlen(name) + uwidth_max(U_CURRENT));
            if(!obj->name)
                goto _skin_error;
            ustrcpy(obj->name, name);
            name = get_datafile_property(skn->dat + i, DAT_ID('T', 'Y', 'P', 'E'));
            if(ustricmp(name, "texture") == 0){
                free(obj->name);
                free(obj);
                continue;
            }
            TRACE("Skin: Loading %s as binary data link\n", obj->name);
            name = get_datafile_property(skn->dat + i, DAT_ID('S', 'K', 'I', 'D'));
            if(*name == '\0'){
                /* Invalid skin id, continue on */
                TRACE("Skin: Warning - No Reference Specified\n");
                free(obj->name);
                free(obj);
                continue;
            }
            tmp = _get_skin_object(skn, name);
            if(!tmp){
                TRACE("Skin: Warning - Reference %s Not Found\n", name);
                free(obj->name);
                free(obj);
                continue;
            }
            obj->type = tmp->type;
            obj->data = tmp->data;
            /* Don't destroy linked data */
            obj->destroy_object = NULL;
            awe_sbt_insert(&skn->obj_list, obj, _skin_compare_func);
        }
    }
    if(alpha)
        set_color_conversion(COLORCONV_TOTAL);
    TRACE("Skin: Loaded Successfully\n");
    return skn;
    _skin_error:
    TRACE("Skin: Unable to load skin\n");
    /* Cleanup */
    if(obj)
        free(obj->name);
    free(obj);
    awe_unload_skin(skn);
    return NULL;
}


void awe_unload_skin(AWE_SKIN *skn){
    if(skn->dat)
        unload_datafile(skn->dat);
    awe_sbt_destroy(&skn->obj_list, _skin_delete_func);
    free(skn);
    TRACE("Skin: Unloaded Successfully\n");
}


AWE_TEXTURE *awe_get_skin_texture(AWE_SKIN *skn, const char *name){
    _SKIN_OBJECT *obj;
    obj = _get_skin_object(skn, name);
    if(obj && obj->type == SKIN_TEXTURE)
        return (AWE_TEXTURE*)obj->data;
    return NULL;
}


RGB *awe_get_skin_color(AWE_SKIN *skn, const char *name){
    _SKIN_OBJECT *obj;
    obj = _get_skin_object(skn, name);
    if(obj && obj->type == SKIN_RGB)
        return (RGB*)obj->data;
    return NULL;
}


FONT *awe_get_skin_font(AWE_SKIN *skn, const char *name){
    _SKIN_OBJECT *obj;
    obj = _get_skin_object(skn, name);
    if(obj && obj->type == SKIN_FONT)
        return (FONT*)obj->data;
    return font;
}


BITMAP *awe_get_skin_bitmap(AWE_SKIN *skn, const char *name){
    _SKIN_OBJECT *obj;
    obj = _get_skin_object(skn, name);
    if(obj && obj->type == SKIN_BITMAP)
        return (BITMAP*)obj->data;
    return NULL;
}


void *awe_get_skin_data(AWE_SKIN *skn, const char *name){
    _SKIN_OBJECT *obj;
    obj = _get_skin_object(skn, name);
    if(obj)
        return (void*)obj->data;
    return NULL;
}


int awe_get_skin_int(AWE_SKIN *skn, const char *name, int num){
    _SKIN_OBJECT *obj;
    static int tmp2;
    tmp2 = num;
    obj = _get_skin_object(skn, name);
    if(obj && obj->type == SKIN_INT){
        memcpy(&tmp2, obj->data, sizeof(int));
        if(_test_byte_order() == BIG_ENDIAN)
            tmp2 = _byte_swap(tmp2);
    }
    return tmp2;
}
