#include "skin.h"

/*****************************************************************************
    INTERNALS
 *****************************************************************************/


#define BIG_ENDIAN      0
#define LITTLE_ENDIAN   1


/** skin tile methods
 */
enum _SKIN_TILE_METHOD {
    SKIN_TILE_HORIZONTAL,
    SKIN_TILE_VERTICAL
};
typedef enum _SKIN_TILE_METHOD _SKIN_TILE_METHOD;


/** skin object types
 */
enum _SKIN_OBJECT_TYPE {
    SKIN_BITMAP = DAT_ID('B', 'M', 'P', ' '),
    SKIN_TEXTURE = DAT_ID('T', 'X', 'T', 'R'),
    SKIN_FONT = DAT_ID('F', 'O', 'N', 'T'),
    SKIN_RGB = DAT_ID('R', 'G', 'B', ' '),
    SKIN_INT = DAT_ID('I', 'N', 'T', ' '),
    SKIN_ANIM = DAT_ID('A', 'N', 'I', 'M')
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


static AWE_SKIN_ANIM _default_anim = { 0, 1, 0 };


static RGB _default_color = { 0, 0, 0 };


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


static void _skin_destroy_data(void *data){
    free(data);
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


static int _generate_skin_texture(AWE_SKIN *skn, BITMAP *base, _BOARDER *boarder, const char *name, int idx, int startframe, int numframes, int animframes, _SKIN_TILE_METHOD basetile, _SKIN_TILE_METHOD animtile){
    int offsetx = animtile ? 0 : base->w / numframes;
    int offsety = animtile ? base->h / numframes : 0;
    int startx = basetile ? 0 : startframe * (base->w / numframes);
    int starty = basetile ? startframe * (base->h / numframes) : 0;
    int texturew = basetile ? (animtile ? base->w : base->w / animframes) : base->w / numframes;
    int textureh = basetile ? (animtile ? base->h / numframes : base->h / numframes) : (animtile ? base->h / animframes : base->h);
    char curframe[8];
    _SKIN_OBJECT *obj;
    int i;
    for(i = 0; i < animframes; i++){
        if((obj = (_SKIN_OBJECT*)malloc(sizeof(_SKIN_OBJECT))) == NULL)
            goto _skin_error;
        if(animframes == 1){
            if((obj->name = (char*)malloc(ustrlen(name) + uwidth_max(U_CURRENT))) == NULL)
                goto _skin_error;
            ustrcpy(obj->name, name);
        }
        else{
            usprintf(curframe, "Frame%d", i);
            if((obj->name = (char*)malloc(ustrlen(name) + ustrlen(curframe) + uwidth(".") + uwidth_max(U_CURRENT))) == NULL)
                goto _skin_error;
            usprintf(obj->name, "%s.%s", name, curframe);
        }
        obj->type = SKIN_TEXTURE;
        obj->skin_idx = idx;
        obj->destroy_object = _skin_destroy_texture;
        TRACE("Skin: Loading %s as texture\n", obj->name);
        if(((AWE_TEXTURE*)obj->data = awe_create_texture(base, startx, starty, texturew, textureh, boarder->left, boarder->top, boarder->right, boarder->bottom)) == NULL)
            goto _skin_error;
        awe_sbt_insert(&skn->obj_list, obj, _skin_compare_func);
        startx += offsetx;
        starty += offsety;
    }
    return 1;
    _skin_error:
    if(obj)
        free(obj->name);
    free(obj);
    return 0;
}


static int _create_animation(AWE_SKIN *skn, const char *name, AWE_SKIN_ANIM_TYPE type, int numframes, int speed){
    _SKIN_OBJECT *obj;
    char *anim = "Anim";
    if((obj = (_SKIN_OBJECT*)malloc(sizeof(_SKIN_OBJECT))) == NULL)
        goto _skin_error;
    if((obj->name = (char*)malloc(ustrlen(name) + ustrlen(anim) + uwidth(".") + uwidth_max(U_CURRENT))) == NULL)
        goto _skin_error;
    if((obj->data = (AWE_SKIN_ANIM*)malloc(sizeof(AWE_SKIN_ANIM))) == NULL)
        goto _skin_error;
    usprintf(obj->name, "%s.%s", name, anim);
    TRACE("Skin: Loading %s as animation\n", obj->name);
    ((AWE_SKIN_ANIM*)(obj->data))->type = type;
    ((AWE_SKIN_ANIM*)(obj->data))->numframes = numframes;
    ((AWE_SKIN_ANIM*)(obj->data))->speed = speed;
    obj->skin_idx = -1;
    obj->type = SKIN_ANIM;
    obj->destroy_object = _skin_destroy_data;
    awe_sbt_insert(&skn->obj_list, obj, _skin_compare_func);
    return 1;
    _skin_error:
    if(obj){
        free(obj->name);
        free(obj->data);
    }
    free(obj);
    return 0;
}


static int _load_texture_references(AWE_SKIN *skn){
    _SKIN_OBJECT *tmp;
    _BOARDER boarder;
    const char *name;
    int num_frames, start_frame, anim_frames, base_tile, anim_tile, anim_type, anim_speed, i;
    for(i = 0; skn->dat[i].type != DAT_END; i++){
        /* Load references of type texture */
        if(skn->dat[i].type == DAT_ID('R', 'E', 'F', ' ')){
            name = get_datafile_property(skn->dat + i, DAT_ID('T', 'Y', 'P', 'E'));
            if(ustricmp(name, "texture") == 0){
                name = get_datafile_property(skn->dat + i, DAT_ID('S', 'K', 'I', 'D'));
                if(*name == '\0'){
                    /* Invalid skin id, continue on */
                    TRACE("Skin: Warning - No Reference Specified\n");
                    continue;
                }
                if((tmp = _get_skin_object(skn, name)) == NULL){
                    TRACE("Skin: Warning - Reference %s Not Found\n", name);
                    continue;
                }
                if(tmp->type != SKIN_BITMAP){
                    TRACE("Skin: Warning - Reference Not Of Type Bitmap\n");
                    continue;
                }
                _get_dat_boarder(skn->dat, &boarder, tmp->skin_idx);
                num_frames = _get_dat_int(skn->dat, tmp->skin_idx, DAT_ID('N', 'F', 'R', 'M'), 1);
                anim_frames = _get_dat_int(skn->dat, i, DAT_ID('N', 'F', 'R', 'M'), 1);
                anim_type = _get_dat_int(skn->dat, i, DAT_ID('A', 'N', 'M', 'T'), 1);
                anim_speed = _get_dat_int(skn->dat, i, DAT_ID('A', 'N', 'M', 'S'), 0);
                start_frame = _get_dat_int(skn->dat, i, DAT_ID('F', 'R', 'M', 'E'), 0);
                name = get_datafile_property(skn->dat + i, DAT_ID('T', 'I', 'L', 'E'));
                if(*name == '\0' || ustricmp(name, "Horizontal") == 0)
                    anim_tile = SKIN_TILE_HORIZONTAL;
                else
                    anim_tile = SKIN_TILE_VERTICAL;
                name = get_datafile_property(skn->dat + tmp->skin_idx, DAT_ID('T', 'I', 'L', 'E'));
                if(*name == '\0' || ustricmp(name, "Horizontal") == 0)
                    base_tile = SKIN_TILE_HORIZONTAL;
                else
                    base_tile = SKIN_TILE_VERTICAL;
                name = get_datafile_property(skn->dat + i, DAT_ID('A', 'W', 'E', ' '));
                if(*name == '\0')
                    continue;
                else{ 
                    if(!_generate_skin_texture(skn, tmp->data, &boarder, name, i, start_frame, num_frames, anim_frames, base_tile, anim_tile))
                        return 0;
                    if(anim_frames > 1){
                        if(!_create_animation(skn, name, anim_type, anim_frames, anim_speed))
                            return 0;
                    }
                }
            }
        }
    }
    return 1;
}


static int _load_objects(AWE_SKIN *skn){
    _SKIN_OBJECT *obj;
    _BOARDER boarder;
    const char *name;
    int i;
    for(i = 0; skn->dat[i].type != DAT_END; i++){
        name = get_datafile_property(skn->dat + i, DAT_ID('A', 'W', 'E', ' '));
        if(*name == '\0')
            continue;
        /* Skip references */
        if(skn->dat[i].type == DAT_ID('R', 'E', 'F', ' '))
            continue;
        if((obj = (_SKIN_OBJECT*)malloc(sizeof(_SKIN_OBJECT))) == NULL)
            goto _skin_error;
        obj->skin_idx = i;
        if((obj->name = (char*)malloc(ustrlen(name) + uwidth_max(U_CURRENT))) == NULL)
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
                    TRACE("Skin: Loading %s as texture\n", obj->name);
                    /* Create texture */
                    _get_dat_boarder(skn->dat, &boarder, i);
                    obj->type = SKIN_TEXTURE;
                    if(((AWE_TEXTURE*)obj->data = awe_create_texture((BITMAP*)skn->dat[i].dat, 0, 0, ((BITMAP*)(skn->dat[i].dat))->w, ((BITMAP*)(skn->dat[i].dat))->h, boarder.left, boarder.top, boarder.right, boarder.bottom)) == NULL)
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
    return 1;
    _skin_error:
    if(obj)
        free(obj->name);
    free(obj);
    return 0;
}


static int _load_references(AWE_SKIN *skn){
    _SKIN_OBJECT *obj;
    _SKIN_OBJECT *tmp;
    const char *name;
    int i;
    for(i = 0; skn->dat[i].type != DAT_END; i++){
        /* Load references */
        if(skn->dat[i].type == DAT_ID('R', 'E', 'F', ' ')){
            name = get_datafile_property(skn->dat + i, DAT_ID('T', 'Y', 'P', 'E'));
            if(ustricmp(name, "texture") == 0)
                continue;
            name = get_datafile_property(skn->dat + i, DAT_ID('S', 'K', 'I', 'D'));
            if(*name == '\0'){
                /* Invalid skin id, continue on */
                TRACE("Skin: Warning - No Reference Specified\n");
                continue;
            }
            if((tmp = _get_skin_object(skn, name)) == NULL){
                TRACE("Skin: Warning - Reference %s Not Found\n", name);
                continue;
            }
            name = get_datafile_property(skn->dat + i, DAT_ID('A', 'W', 'E', ' '));
            if(*name == '\0')
                continue;
            TRACE("Skin: Loading %s as binary data link\n", name);
            if((obj = (_SKIN_OBJECT*)malloc(sizeof(_SKIN_OBJECT))) == NULL)
                goto _skin_error;
            if((obj->name = (char*)malloc(ustrlen(name) + uwidth_max(U_CURRENT))) == NULL)
                goto _skin_error;
            ustrcpy(obj->name, name);
            obj->skin_idx = i;
            obj->type = tmp->type;
            obj->data = tmp->data;
            /* Don't destroy linked data */
            obj->destroy_object = NULL;
            awe_sbt_insert(&skn->obj_list, obj, _skin_compare_func);
        }
    }
    return 1;
    _skin_error:
    if(obj)
        free(obj->name);
    free(obj);
    return 0;
}


/*****************************************************************************
    PUBLIC
 *****************************************************************************/


AWE_SKIN *awe_load_skin(const char *filename){
    AWE_SKIN *skn;
    int i, alpha = 0;
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
    if(!_load_objects(skn))
        goto _skin_error;
    if(!_load_texture_references(skn))
        goto _skin_error;
    if(!_load_references(skn))
        goto _skin_error;
    if(alpha)
        set_color_conversion(COLORCONV_TOTAL);
    TRACE("Skin: Loaded Successfully\n");
    return skn;
    _skin_error:
    TRACE("Skin: Unable to load skin\n");
    /* Cleanup */
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
    return &_default_color;
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


AWE_SKIN_ANIM *awe_get_skin_anim(AWE_SKIN *skn, const char *name){
    _SKIN_OBJECT *obj;
    obj = _get_skin_object(skn, name);
    if(obj && obj->type == SKIN_ANIM)
        return (AWE_SKIN_ANIM*)obj->data;
    return &_default_anim;
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
