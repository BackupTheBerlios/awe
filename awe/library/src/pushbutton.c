#include "pushbutton.h"


/*****************************************************************************
    INTERNALS
 *****************************************************************************/


static RGB _face_color_normal       = { 212, 208, 200, 0 };
static RGB _face_color_highlighted  = { 228, 224, 216, 0 };
static RGB _font_color_normal       = { 0  , 0  , 0  , 0 };
static RGB _font_color_disabled     = { 128, 128, 128, 0 };
static RGB _shadow_color_normal     = { 255, 0  , 255, 0 };		//Magenta will not show
static RGB _shadow_color_disabled   = { 255, 255, 255, 0 };
static RGB _edge_color_top_left     = { 241, 239, 226, 0 };
static RGB _edge_color_bottom_right = { 128, 128, 128, 0 };
static int _margin[4]               = { 8  , 8  , 8  , 8 };


//used in loading skins /* Not implemented */
/*static char *_push_button_texture[AWE_PUSH_BUTTON_NUM_TEXTURES] = {
    "Enabled",
    "Pressed",
    "Disabled",
    "Highlighted",
    "Focused"
};*/


//push button events
static AWE_CLASS_EVENT _push_button_events[] = {
    { AWE_ID_PUSH_BUTTON_ACTIVATED, 0 },
    { AWE_ID_PUSH_BUTTON_RELEASED,  0 },
    { AWE_ID_PUSH_BUTTON_HELD_DOWN, 0 },
    { AWE_ID_PUSH_BUTTON_PRESSED,   0 },
    { 0 }
};


//constructor
static void _push_button_constructor(AWE_OBJECT *obj)
{
    int i, j;
    AWE_PUSH_BUTTON *tmp = (AWE_PUSH_BUTTON *)obj;
    tmp->text = ustrdup(empty_string);
    tmp->font = font;
    memcpy(&tmp->margin, &_margin, sizeof(_margin));
    for(i = 0; i < AWE_PUSH_BUTTON_NUM_TEXTURES; i++){
        for(j = 0; j < AWE_PUSH_BUTTON_NUM_FACES; j++){
            if(i == AWE_PUSH_BUTTON_TEXTURE_HIGHLIGHTED)
                memcpy(&tmp->texture[i].face_col[j], &_face_color_highlighted, sizeof(RGB));
            else
                memcpy(&tmp->texture[i].face_col[j], &_face_color_normal, sizeof(RGB));
        }
        for(j = 0; j < AWE_PUSH_BUTTON_NUM_EDGES; j++){
            if(i == AWE_PUSH_BUTTON_TEXTURE_PRESSED){
                if(j == AWE_PUSH_BUTTON_EDGE_TOP_LEFT)
                    memcpy(&tmp->texture[i].edge_col[j], &_edge_color_bottom_right, sizeof(RGB));
                else
                    memcpy(&tmp->texture[i].edge_col[j], &_edge_color_top_left, sizeof(RGB));
            }
	    else{
                if(j == AWE_PUSH_BUTTON_EDGE_TOP_LEFT)
                    memcpy(&tmp->texture[i].edge_col[j], &_edge_color_top_left, sizeof(RGB));
                else
                    memcpy(&tmp->texture[i].edge_col[j], &_edge_color_bottom_right, sizeof(RGB));
            }
        }
        if(i == AWE_PUSH_BUTTON_TEXTURE_DISABLED){
            memcpy(&tmp->texture[i].font_col, &_font_color_disabled, sizeof(RGB));
            memcpy(&tmp->texture[i].font_sdw, &_shadow_color_disabled, sizeof(RGB));
        }
        else{
            memcpy(&tmp->texture[i].font_col, &_font_color_normal, sizeof(RGB));
            memcpy(&tmp->texture[i].font_sdw, &_shadow_color_normal, sizeof(RGB));
        }      
    }
}


//destructor
static void _push_button_destructor(AWE_OBJECT *obj)
{
    AWE_PUSH_BUTTON *tmp = (AWE_PUSH_BUTTON *)obj;
    free(tmp->text);
}


//gets the text
static void _push_button_get_text(AWE_OBJECT *obj, void *data)
{
    AWE_PUSH_BUTTON *tmp = (AWE_PUSH_BUTTON *)obj;
    *(const char **)data = tmp->text;
}


//sets the text
static void _push_button_set_text(AWE_OBJECT *obj, void *data)
{
    const char *new_text = *(const char **)data;
    AWE_PUSH_BUTTON *tmp = (AWE_PUSH_BUTTON *)obj;
    free(tmp->text);
    tmp->text = ustrdup(new_text ? new_text : empty_string);
    awe_set_widget_dirty(&tmp->widget);
}


//gets the font
static void _push_button_get_font(AWE_OBJECT *obj, void *data)
{
    AWE_PUSH_BUTTON *tmp = (AWE_PUSH_BUTTON *)obj;
    *(FONT **)data = tmp->font;
}


//sets the font
static void _push_button_set_font(AWE_OBJECT *obj, void *data)
{
    AWE_PUSH_BUTTON *tmp = (AWE_PUSH_BUTTON *)obj;
    tmp->font = *(FONT **)data;
    awe_set_widget_dirty(&tmp->widget);
}


//gets the bitmap
static void _push_button_get_bitmap(AWE_OBJECT *obj, void *data)
{
    AWE_PUSH_BUTTON *tmp = (AWE_PUSH_BUTTON *)obj;
    *(BITMAP **)data = tmp->bitmap;
}


//sets the bitmap
static void _push_button_set_bitmap(AWE_OBJECT *obj, void *data)
{
    AWE_PUSH_BUTTON *tmp = (AWE_PUSH_BUTTON *)obj;
    tmp->bitmap = *(BITMAP **)data;
    awe_set_widget_dirty(&tmp->widget);
}


//gets the bitmap direction
static void _push_button_get_bitmap_dir(AWE_OBJECT *obj, void *data)
{
    AWE_PUSH_BUTTON *tmp = (AWE_PUSH_BUTTON *)obj;
    *(int *)data = tmp->bitmap_dir;
}


//sets the bitmap direction
static void _push_button_set_bitmap_dir(AWE_OBJECT *obj, void *data)
{
    AWE_PUSH_BUTTON *tmp = (AWE_PUSH_BUTTON *)obj;
    tmp->bitmap_dir = *(int *)data;
    awe_set_widget_dirty(&tmp->widget);
}


//gets the top margin
static void _push_button_get_margin_top(AWE_OBJECT *obj, void *data)
{
    AWE_PUSH_BUTTON *tmp = (AWE_PUSH_BUTTON *)obj;
    *(int *)data = tmp->margin.top;
}


//sets the top margin
static void _push_button_set_margin_top(AWE_OBJECT *obj, void *data)
{
    AWE_PUSH_BUTTON *tmp = (AWE_PUSH_BUTTON *)obj;
    tmp->margin.top = *(int *)data;
    awe_set_widget_dirty(&tmp->widget);
}


//gets the left margin
static void _push_button_get_margin_left(AWE_OBJECT *obj, void *data)
{
    AWE_PUSH_BUTTON *tmp = (AWE_PUSH_BUTTON *)obj;
    *(int *)data = tmp->margin.left;
}


//sets the left margin
static void _push_button_set_margin_left(AWE_OBJECT *obj, void *data)
{
    AWE_PUSH_BUTTON *tmp = (AWE_PUSH_BUTTON *)obj;
    tmp->margin.left = *(int *)data;
    awe_set_widget_dirty(&tmp->widget);
}


//gets the right margin
static void _push_button_get_margin_right(AWE_OBJECT *obj, void *data)
{
    AWE_PUSH_BUTTON *tmp = (AWE_PUSH_BUTTON *)obj;
    *(int *)data = tmp->margin.right;
}


//sets the right margin
static void _push_button_set_margin_right(AWE_OBJECT *obj, void *data)
{
    AWE_PUSH_BUTTON *tmp = (AWE_PUSH_BUTTON *)obj;
    tmp->margin.right = *(int *)data;
    awe_set_widget_dirty(&tmp->widget);
}


//gets the bottom margin
static void _push_button_get_margin_bottom(AWE_OBJECT *obj, void *data)
{
    AWE_PUSH_BUTTON *tmp = (AWE_PUSH_BUTTON *)obj;
    *(int *)data = tmp->margin.bottom;
}


//sets the bottom margin
static void _push_button_set_margin_bottom(AWE_OBJECT *obj, void *data)
{
    AWE_PUSH_BUTTON *tmp = (AWE_PUSH_BUTTON *)obj;
    tmp->margin.bottom = *(int *)data;
    awe_set_widget_dirty(&tmp->widget);
}


//gets the top left enabled face color
static void _push_button_get_face_color_top_left_enabled(AWE_OBJECT *obj, void *data)
{
    AWE_PUSH_BUTTON *tmp = (AWE_PUSH_BUTTON *)obj;
    memcpy((RGB *)data, &tmp->texture[AWE_PUSH_BUTTON_TEXTURE_ENABLED].face_col[AWE_PUSH_BUTTON_FACE_TOP_LEFT], sizeof(RGB));
}


//sets the top left enabled face color
static void _push_button_set_face_color_top_left_enabled(AWE_OBJECT *obj, void *data)
{
    AWE_PUSH_BUTTON *tmp = (AWE_PUSH_BUTTON *)obj;
    memcpy(&tmp->texture[AWE_PUSH_BUTTON_TEXTURE_ENABLED].face_col[AWE_PUSH_BUTTON_FACE_TOP_LEFT], (RGB *)data, sizeof(RGB));
    awe_set_widget_dirty(&tmp->widget);
}


//gets the top right enabled face color
static void _push_button_get_face_color_top_right_enabled(AWE_OBJECT *obj, void *data)
{
    AWE_PUSH_BUTTON *tmp = (AWE_PUSH_BUTTON *)obj;
    memcpy((RGB *)data, &tmp->texture[AWE_PUSH_BUTTON_TEXTURE_ENABLED].face_col[AWE_PUSH_BUTTON_FACE_TOP_RIGHT], sizeof(RGB));
}


//sets the top right enabled face color
static void _push_button_set_face_color_top_right_enabled(AWE_OBJECT *obj, void *data)
{
    AWE_PUSH_BUTTON *tmp = (AWE_PUSH_BUTTON *)obj;
    memcpy(&tmp->texture[AWE_PUSH_BUTTON_TEXTURE_ENABLED].face_col[AWE_PUSH_BUTTON_FACE_TOP_RIGHT], (RGB *)data, sizeof(RGB));
    awe_set_widget_dirty(&tmp->widget);
}


//gets the bottom left enabled face color
static void _push_button_get_face_color_bottom_left_enabled(AWE_OBJECT *obj, void *data)
{
    AWE_PUSH_BUTTON *tmp = (AWE_PUSH_BUTTON *)obj;
    memcpy((RGB *)data, &tmp->texture[AWE_PUSH_BUTTON_TEXTURE_ENABLED].face_col[AWE_PUSH_BUTTON_FACE_BOTTOM_LEFT], sizeof(RGB));
}


//sets the bottom left enabled face color
static void _push_button_set_face_color_bottom_left_enabled(AWE_OBJECT *obj, void *data)
{
    AWE_PUSH_BUTTON *tmp = (AWE_PUSH_BUTTON *)obj;
    memcpy(&tmp->texture[AWE_PUSH_BUTTON_TEXTURE_ENABLED].face_col[AWE_PUSH_BUTTON_FACE_BOTTOM_LEFT], (RGB *)data, sizeof(RGB));
    awe_set_widget_dirty(&tmp->widget);
}


//gets the bottom right enabled face color
static void _push_button_get_face_color_bottom_right_enabled(AWE_OBJECT *obj, void *data)
{
    AWE_PUSH_BUTTON *tmp = (AWE_PUSH_BUTTON *)obj;
    memcpy((RGB *)data, &tmp->texture[AWE_PUSH_BUTTON_TEXTURE_ENABLED].face_col[AWE_PUSH_BUTTON_FACE_BOTTOM_RIGHT], sizeof(RGB));
}


//sets the bottom right enabled face color
static void _push_button_set_face_color_bottom_right_enabled(AWE_OBJECT *obj, void *data)
{
    AWE_PUSH_BUTTON *tmp = (AWE_PUSH_BUTTON *)obj;
    memcpy(&tmp->texture[AWE_PUSH_BUTTON_TEXTURE_ENABLED].face_col[AWE_PUSH_BUTTON_FACE_BOTTOM_RIGHT], (RGB *)data, sizeof(RGB));
    awe_set_widget_dirty(&tmp->widget);
}


//gets the top left disabled face color
static void _push_button_get_face_color_top_left_disabled(AWE_OBJECT *obj, void *data)
{
    AWE_PUSH_BUTTON *tmp = (AWE_PUSH_BUTTON *)obj;
    memcpy((RGB *)data, &tmp->texture[AWE_PUSH_BUTTON_TEXTURE_DISABLED].face_col[AWE_PUSH_BUTTON_FACE_TOP_LEFT], sizeof(RGB));
}


//sets the top left disabled face color
static void _push_button_set_face_color_top_left_disabled(AWE_OBJECT *obj, void *data)
{
    AWE_PUSH_BUTTON *tmp = (AWE_PUSH_BUTTON *)obj;
    memcpy(&tmp->texture[AWE_PUSH_BUTTON_TEXTURE_DISABLED].face_col[AWE_PUSH_BUTTON_FACE_TOP_LEFT], (RGB *)data, sizeof(RGB));
    awe_set_widget_dirty(&tmp->widget);
}


//gets the top right disabled face color
static void _push_button_get_face_color_top_right_disabled(AWE_OBJECT *obj, void *data)
{
    AWE_PUSH_BUTTON *tmp = (AWE_PUSH_BUTTON *)obj;
    memcpy((RGB *)data, &tmp->texture[AWE_PUSH_BUTTON_TEXTURE_DISABLED].face_col[AWE_PUSH_BUTTON_FACE_TOP_RIGHT], sizeof(RGB));
}


//sets the top right disabled face color
static void _push_button_set_face_color_top_right_disabled(AWE_OBJECT *obj, void *data)
{
    AWE_PUSH_BUTTON *tmp = (AWE_PUSH_BUTTON *)obj;
    memcpy(&tmp->texture[AWE_PUSH_BUTTON_TEXTURE_DISABLED].face_col[AWE_PUSH_BUTTON_FACE_TOP_RIGHT], (RGB *)data, sizeof(RGB));
    awe_set_widget_dirty(&tmp->widget);
}


//gets the bottom left disabled face color
static void _push_button_get_face_color_bottom_left_disabled(AWE_OBJECT *obj, void *data)
{
    AWE_PUSH_BUTTON *tmp = (AWE_PUSH_BUTTON *)obj;
    memcpy((RGB *)data, &tmp->texture[AWE_PUSH_BUTTON_TEXTURE_DISABLED].face_col[AWE_PUSH_BUTTON_FACE_BOTTOM_LEFT], sizeof(RGB));
}


//sets the bottom left disabled face color
static void _push_button_set_face_color_bottom_left_disabled(AWE_OBJECT *obj, void *data)
{
    AWE_PUSH_BUTTON *tmp = (AWE_PUSH_BUTTON *)obj;
    memcpy(&tmp->texture[AWE_PUSH_BUTTON_TEXTURE_DISABLED].face_col[AWE_PUSH_BUTTON_FACE_BOTTOM_LEFT], (RGB *)data, sizeof(RGB));
    awe_set_widget_dirty(&tmp->widget);
}


//gets the bottom right disabled face color
static void _push_button_get_face_color_bottom_right_disabled(AWE_OBJECT *obj, void *data)
{
    AWE_PUSH_BUTTON *tmp = (AWE_PUSH_BUTTON *)obj;
    memcpy((RGB *)data, &tmp->texture[AWE_PUSH_BUTTON_TEXTURE_DISABLED].face_col[AWE_PUSH_BUTTON_FACE_BOTTOM_RIGHT], sizeof(RGB));
}


//sets the bottom right disabled face color
static void _push_button_set_face_color_bottom_right_disabled(AWE_OBJECT *obj, void *data)
{
    AWE_PUSH_BUTTON *tmp = (AWE_PUSH_BUTTON *)obj;
    memcpy(&tmp->texture[AWE_PUSH_BUTTON_TEXTURE_DISABLED].face_col[AWE_PUSH_BUTTON_FACE_BOTTOM_RIGHT], (RGB *)data, sizeof(RGB));
    awe_set_widget_dirty(&tmp->widget);
}


//gets the top left pressed face color
static void _push_button_get_face_color_top_left_pressed(AWE_OBJECT *obj, void *data)
{
    AWE_PUSH_BUTTON *tmp = (AWE_PUSH_BUTTON *)obj;
    memcpy((RGB *)data, &tmp->texture[AWE_PUSH_BUTTON_TEXTURE_PRESSED].face_col[AWE_PUSH_BUTTON_FACE_TOP_LEFT], sizeof(RGB));
}


//sets the top left pressed face color
static void _push_button_set_face_color_top_left_pressed(AWE_OBJECT *obj, void *data)
{
    AWE_PUSH_BUTTON *tmp = (AWE_PUSH_BUTTON *)obj;
    memcpy(&tmp->texture[AWE_PUSH_BUTTON_TEXTURE_PRESSED].face_col[AWE_PUSH_BUTTON_FACE_TOP_LEFT], (RGB *)data, sizeof(RGB));
    awe_set_widget_dirty(&tmp->widget);
}


//gets the top right pressed face color
static void _push_button_get_face_color_top_right_pressed(AWE_OBJECT *obj, void *data)
{
    AWE_PUSH_BUTTON *tmp = (AWE_PUSH_BUTTON *)obj;
    memcpy((RGB *)data, &tmp->texture[AWE_PUSH_BUTTON_TEXTURE_PRESSED].face_col[AWE_PUSH_BUTTON_FACE_TOP_RIGHT], sizeof(RGB));
}


//sets the top right pressed face color
static void _push_button_set_face_color_top_right_pressed(AWE_OBJECT *obj, void *data)
{
    AWE_PUSH_BUTTON *tmp = (AWE_PUSH_BUTTON *)obj;
    memcpy(&tmp->texture[AWE_PUSH_BUTTON_TEXTURE_PRESSED].face_col[AWE_PUSH_BUTTON_FACE_TOP_RIGHT], (RGB *)data, sizeof(RGB));
    awe_set_widget_dirty(&tmp->widget);
}


//gets the bottom left pressed face color
static void _push_button_get_face_color_bottom_left_pressed(AWE_OBJECT *obj, void *data)
{
    AWE_PUSH_BUTTON *tmp = (AWE_PUSH_BUTTON *)obj;
    memcpy((RGB *)data, &tmp->texture[AWE_PUSH_BUTTON_TEXTURE_PRESSED].face_col[AWE_PUSH_BUTTON_FACE_BOTTOM_LEFT], sizeof(RGB));
}


//sets the bottom left pressed face color
static void _push_button_set_face_color_bottom_left_pressed(AWE_OBJECT *obj, void *data)
{
    AWE_PUSH_BUTTON *tmp = (AWE_PUSH_BUTTON *)obj;
    memcpy(&tmp->texture[AWE_PUSH_BUTTON_TEXTURE_PRESSED].face_col[AWE_PUSH_BUTTON_FACE_BOTTOM_LEFT], (RGB *)data, sizeof(RGB));
    awe_set_widget_dirty(&tmp->widget);
}


//gets the bottom right pressed face color
static void _push_button_get_face_color_bottom_right_pressed(AWE_OBJECT *obj, void *data)
{
    AWE_PUSH_BUTTON *tmp = (AWE_PUSH_BUTTON *)obj;
    memcpy((RGB *)data, &tmp->texture[AWE_PUSH_BUTTON_TEXTURE_PRESSED].face_col[AWE_PUSH_BUTTON_FACE_BOTTOM_RIGHT], sizeof(RGB));
}


//sets the bottom right pressed face color
static void _push_button_set_face_color_bottom_right_pressed(AWE_OBJECT *obj, void *data)
{
    AWE_PUSH_BUTTON *tmp = (AWE_PUSH_BUTTON *)obj;
    memcpy(&tmp->texture[AWE_PUSH_BUTTON_TEXTURE_PRESSED].face_col[AWE_PUSH_BUTTON_FACE_BOTTOM_RIGHT], (RGB *)data, sizeof(RGB));
    awe_set_widget_dirty(&tmp->widget);
}


//gets the top left highlighted face color
static void _push_button_get_face_color_top_left_highlighted(AWE_OBJECT *obj, void *data)
{
    AWE_PUSH_BUTTON *tmp = (AWE_PUSH_BUTTON *)obj;
    memcpy((RGB *)data, &tmp->texture[AWE_PUSH_BUTTON_TEXTURE_HIGHLIGHTED].face_col[AWE_PUSH_BUTTON_FACE_TOP_LEFT], sizeof(RGB));
}


//sets the top left highlighted face color
static void _push_button_set_face_color_top_left_highlighted(AWE_OBJECT *obj, void *data)
{
    AWE_PUSH_BUTTON *tmp = (AWE_PUSH_BUTTON *)obj;
    memcpy(&tmp->texture[AWE_PUSH_BUTTON_TEXTURE_HIGHLIGHTED].face_col[AWE_PUSH_BUTTON_FACE_TOP_LEFT], (RGB *)data, sizeof(RGB));
    awe_set_widget_dirty(&tmp->widget);
}


//gets the top right highlighted face color
static void _push_button_get_face_color_top_right_highlighted(AWE_OBJECT *obj, void *data)
{
    AWE_PUSH_BUTTON *tmp = (AWE_PUSH_BUTTON *)obj;
    memcpy((RGB *)data, &tmp->texture[AWE_PUSH_BUTTON_TEXTURE_HIGHLIGHTED].face_col[AWE_PUSH_BUTTON_FACE_TOP_RIGHT], sizeof(RGB));
}


//sets the top right highlighted face color
static void _push_button_set_face_color_top_right_highlighted(AWE_OBJECT *obj, void *data)
{
    AWE_PUSH_BUTTON *tmp = (AWE_PUSH_BUTTON *)obj;
    memcpy(&tmp->texture[AWE_PUSH_BUTTON_TEXTURE_HIGHLIGHTED].face_col[AWE_PUSH_BUTTON_FACE_TOP_RIGHT], (RGB *)data, sizeof(RGB));
    awe_set_widget_dirty(&tmp->widget);
}


//gets the bottom left highlighted face color
static void _push_button_get_face_color_bottom_left_highlighted(AWE_OBJECT *obj, void *data)
{
    AWE_PUSH_BUTTON *tmp = (AWE_PUSH_BUTTON *)obj;
    memcpy((RGB *)data, &tmp->texture[AWE_PUSH_BUTTON_TEXTURE_HIGHLIGHTED].face_col[AWE_PUSH_BUTTON_FACE_BOTTOM_LEFT], sizeof(RGB));
}


//sets the bottom left highlighted face color
static void _push_button_set_face_color_bottom_left_highlighted(AWE_OBJECT *obj, void *data)
{
    AWE_PUSH_BUTTON *tmp = (AWE_PUSH_BUTTON *)obj;
    memcpy(&tmp->texture[AWE_PUSH_BUTTON_TEXTURE_HIGHLIGHTED].face_col[AWE_PUSH_BUTTON_FACE_BOTTOM_LEFT], (RGB *)data, sizeof(RGB));
    awe_set_widget_dirty(&tmp->widget);
}


//gets the bottom right highlighted face color
static void _push_button_get_face_color_bottom_right_highlighted(AWE_OBJECT *obj, void *data)
{
    AWE_PUSH_BUTTON *tmp = (AWE_PUSH_BUTTON *)obj;
    memcpy((RGB *)data, &tmp->texture[AWE_PUSH_BUTTON_TEXTURE_HIGHLIGHTED].face_col[AWE_PUSH_BUTTON_FACE_BOTTOM_RIGHT], sizeof(RGB));
}


//sets the bottom right highlighted face color
static void _push_button_set_face_color_bottom_right_highlighted(AWE_OBJECT *obj, void *data)
{
    AWE_PUSH_BUTTON *tmp = (AWE_PUSH_BUTTON *)obj;
    memcpy(&tmp->texture[AWE_PUSH_BUTTON_TEXTURE_HIGHLIGHTED].face_col[AWE_PUSH_BUTTON_FACE_BOTTOM_RIGHT], (RGB *)data, sizeof(RGB));
    awe_set_widget_dirty(&tmp->widget);
}


//gets the top left focused face color
static void _push_button_get_face_color_top_left_focused(AWE_OBJECT *obj, void *data)
{
    AWE_PUSH_BUTTON *tmp = (AWE_PUSH_BUTTON *)obj;
    memcpy((RGB *)data, &tmp->texture[AWE_PUSH_BUTTON_TEXTURE_FOCUSED].face_col[AWE_PUSH_BUTTON_FACE_TOP_LEFT], sizeof(RGB));
}


//sets the top left focused face color
static void _push_button_set_face_color_top_left_focused(AWE_OBJECT *obj, void *data)
{
    AWE_PUSH_BUTTON *tmp = (AWE_PUSH_BUTTON *)obj;
    memcpy(&tmp->texture[AWE_PUSH_BUTTON_TEXTURE_FOCUSED].face_col[AWE_PUSH_BUTTON_FACE_TOP_LEFT], (RGB *)data, sizeof(RGB));
    awe_set_widget_dirty(&tmp->widget);
}


//gets the top right focused face color
static void _push_button_get_face_color_top_right_focused(AWE_OBJECT *obj, void *data)
{
    AWE_PUSH_BUTTON *tmp = (AWE_PUSH_BUTTON *)obj;
    memcpy((RGB *)data, &tmp->texture[AWE_PUSH_BUTTON_TEXTURE_FOCUSED].face_col[AWE_PUSH_BUTTON_FACE_TOP_RIGHT], sizeof(RGB));
}


//sets the top right focused face color
static void _push_button_set_face_color_top_right_focused(AWE_OBJECT *obj, void *data)
{
    AWE_PUSH_BUTTON *tmp = (AWE_PUSH_BUTTON *)obj;
    memcpy(&tmp->texture[AWE_PUSH_BUTTON_TEXTURE_FOCUSED].face_col[AWE_PUSH_BUTTON_FACE_TOP_RIGHT], (RGB *)data, sizeof(RGB));
    awe_set_widget_dirty(&tmp->widget);
}


//gets the bottom left focused face color
static void _push_button_get_face_color_bottom_left_focused(AWE_OBJECT *obj, void *data)
{
    AWE_PUSH_BUTTON *tmp = (AWE_PUSH_BUTTON *)obj;
    memcpy((RGB *)data, &tmp->texture[AWE_PUSH_BUTTON_TEXTURE_FOCUSED].face_col[AWE_PUSH_BUTTON_FACE_BOTTOM_LEFT], sizeof(RGB));
}


//sets the bottom left focused face color
static void _push_button_set_face_color_bottom_left_focused(AWE_OBJECT *obj, void *data)
{
    AWE_PUSH_BUTTON *tmp = (AWE_PUSH_BUTTON *)obj;
    memcpy(&tmp->texture[AWE_PUSH_BUTTON_TEXTURE_FOCUSED].face_col[AWE_PUSH_BUTTON_FACE_BOTTOM_LEFT], (RGB *)data, sizeof(RGB));
    awe_set_widget_dirty(&tmp->widget);
}


//gets the bottom right focused face color
static void _push_button_get_face_color_bottom_right_focused(AWE_OBJECT *obj, void *data)
{
    AWE_PUSH_BUTTON *tmp = (AWE_PUSH_BUTTON *)obj;
    memcpy((RGB *)data, &tmp->texture[AWE_PUSH_BUTTON_TEXTURE_FOCUSED].face_col[AWE_PUSH_BUTTON_FACE_BOTTOM_RIGHT], sizeof(RGB));
}


//sets the bottom right focused face color
static void _push_button_set_face_color_bottom_right_focused(AWE_OBJECT *obj, void *data)
{
    AWE_PUSH_BUTTON *tmp = (AWE_PUSH_BUTTON *)obj;
    memcpy(&tmp->texture[AWE_PUSH_BUTTON_TEXTURE_FOCUSED].face_col[AWE_PUSH_BUTTON_FACE_BOTTOM_RIGHT], (RGB *)data, sizeof(RGB));
    awe_set_widget_dirty(&tmp->widget);
}


//gets the top left edge color
static void _push_button_get_edge_color_top_left_enabled(AWE_OBJECT *obj, void *data)
{
    AWE_PUSH_BUTTON *tmp = (AWE_PUSH_BUTTON *)obj;
    memcpy((RGB *)data, &tmp->texture[AWE_PUSH_BUTTON_TEXTURE_ENABLED].edge_col[AWE_PUSH_BUTTON_EDGE_TOP_LEFT], sizeof(RGB));
}


//sets the top left edge color
static void _push_button_set_edge_color_top_left_enabled(AWE_OBJECT *obj, void *data)
{
    AWE_PUSH_BUTTON *tmp = (AWE_PUSH_BUTTON *)obj;
    memcpy(&tmp->texture[AWE_PUSH_BUTTON_TEXTURE_ENABLED].edge_col[AWE_PUSH_BUTTON_EDGE_TOP_LEFT], (RGB *)data, sizeof(RGB));
    awe_set_widget_dirty(&tmp->widget);
}


//gets the bottom right edge color
static void _push_button_get_edge_color_bottom_right_enabled(AWE_OBJECT *obj, void *data)
{
    AWE_PUSH_BUTTON *tmp = (AWE_PUSH_BUTTON *)obj;
    memcpy((RGB *)data, &tmp->texture[AWE_PUSH_BUTTON_TEXTURE_ENABLED].edge_col[AWE_PUSH_BUTTON_EDGE_BOTTOM_RIGHT], sizeof(RGB));
}


//sets the bottom right edge color
static void _push_button_set_edge_color_bottom_right_enabled(AWE_OBJECT *obj, void *data)
{
    AWE_PUSH_BUTTON *tmp = (AWE_PUSH_BUTTON *)obj;
    memcpy(&tmp->texture[AWE_PUSH_BUTTON_TEXTURE_ENABLED].edge_col[AWE_PUSH_BUTTON_EDGE_BOTTOM_RIGHT], (RGB *)data, sizeof(RGB));
    awe_set_widget_dirty(&tmp->widget);
}


//gets the top left edge color
static void _push_button_get_edge_color_top_left_disabled(AWE_OBJECT *obj, void *data)
{
    AWE_PUSH_BUTTON *tmp = (AWE_PUSH_BUTTON *)obj;
    memcpy((RGB *)data, &tmp->texture[AWE_PUSH_BUTTON_TEXTURE_DISABLED].edge_col[AWE_PUSH_BUTTON_EDGE_TOP_LEFT], sizeof(RGB));
}


//sets the top left edge color
static void _push_button_set_edge_color_top_left_disabled(AWE_OBJECT *obj, void *data)
{
    AWE_PUSH_BUTTON *tmp = (AWE_PUSH_BUTTON *)obj;
    memcpy(&tmp->texture[AWE_PUSH_BUTTON_TEXTURE_DISABLED].edge_col[AWE_PUSH_BUTTON_EDGE_TOP_LEFT], (RGB *)data, sizeof(RGB));
    awe_set_widget_dirty(&tmp->widget);
}


//gets the bottom right edge color
static void _push_button_get_edge_color_bottom_right_disabled(AWE_OBJECT *obj, void *data)
{
    AWE_PUSH_BUTTON *tmp = (AWE_PUSH_BUTTON *)obj;
    memcpy((RGB *)data, &tmp->texture[AWE_PUSH_BUTTON_TEXTURE_DISABLED].edge_col[AWE_PUSH_BUTTON_EDGE_BOTTOM_RIGHT], sizeof(RGB));
}


//sets the bottom right edge color
static void _push_button_set_edge_color_bottom_right_disabled(AWE_OBJECT *obj, void *data)
{
    AWE_PUSH_BUTTON *tmp = (AWE_PUSH_BUTTON *)obj;
    memcpy(&tmp->texture[AWE_PUSH_BUTTON_TEXTURE_DISABLED].edge_col[AWE_PUSH_BUTTON_EDGE_BOTTOM_RIGHT], (RGB *)data, sizeof(RGB));
    awe_set_widget_dirty(&tmp->widget);
}


//gets the top left edge color
static void _push_button_get_edge_color_top_left_pressed(AWE_OBJECT *obj, void *data)
{
    AWE_PUSH_BUTTON *tmp = (AWE_PUSH_BUTTON *)obj;
    memcpy((RGB *)data, &tmp->texture[AWE_PUSH_BUTTON_TEXTURE_PRESSED].edge_col[AWE_PUSH_BUTTON_EDGE_TOP_LEFT], sizeof(RGB));
}


//sets the top left edge color
static void _push_button_set_edge_color_top_left_pressed(AWE_OBJECT *obj, void *data)
{
    AWE_PUSH_BUTTON *tmp = (AWE_PUSH_BUTTON *)obj;
    memcpy(&tmp->texture[AWE_PUSH_BUTTON_TEXTURE_PRESSED].edge_col[AWE_PUSH_BUTTON_EDGE_TOP_LEFT], (RGB *)data, sizeof(RGB));
    awe_set_widget_dirty(&tmp->widget);
}


//gets the bottom right edge color
static void _push_button_get_edge_color_bottom_right_pressed(AWE_OBJECT *obj, void *data)
{
    AWE_PUSH_BUTTON *tmp = (AWE_PUSH_BUTTON *)obj;
    memcpy((RGB *)data, &tmp->texture[AWE_PUSH_BUTTON_TEXTURE_PRESSED].edge_col[AWE_PUSH_BUTTON_EDGE_BOTTOM_RIGHT], sizeof(RGB));
}


//sets the bottom right edge color
static void _push_button_set_edge_color_bottom_right_pressed(AWE_OBJECT *obj, void *data)
{
    AWE_PUSH_BUTTON *tmp = (AWE_PUSH_BUTTON *)obj;
    memcpy(&tmp->texture[AWE_PUSH_BUTTON_TEXTURE_PRESSED].edge_col[AWE_PUSH_BUTTON_EDGE_BOTTOM_RIGHT], (RGB *)data, sizeof(RGB));
    awe_set_widget_dirty(&tmp->widget);
}


//gets the top left edge color
static void _push_button_get_edge_color_top_left_highlighted(AWE_OBJECT *obj, void *data)
{
    AWE_PUSH_BUTTON *tmp = (AWE_PUSH_BUTTON *)obj;
    memcpy((RGB *)data, &tmp->texture[AWE_PUSH_BUTTON_TEXTURE_HIGHLIGHTED].edge_col[AWE_PUSH_BUTTON_EDGE_TOP_LEFT], sizeof(RGB));
}


//sets the top left edge color
static void _push_button_set_edge_color_top_left_highlighted(AWE_OBJECT *obj, void *data)
{
    AWE_PUSH_BUTTON *tmp = (AWE_PUSH_BUTTON *)obj;
    memcpy(&tmp->texture[AWE_PUSH_BUTTON_TEXTURE_HIGHLIGHTED].edge_col[AWE_PUSH_BUTTON_EDGE_TOP_LEFT], (RGB *)data, sizeof(RGB));
    awe_set_widget_dirty(&tmp->widget);
}


//gets the bottom right edge color
static void _push_button_get_edge_color_bottom_right_highlighted(AWE_OBJECT *obj, void *data)
{
    AWE_PUSH_BUTTON *tmp = (AWE_PUSH_BUTTON *)obj;
    memcpy((RGB *)data, &tmp->texture[AWE_PUSH_BUTTON_TEXTURE_HIGHLIGHTED].edge_col[AWE_PUSH_BUTTON_EDGE_BOTTOM_RIGHT], sizeof(RGB));
}


//sets the bottom right edge color
static void _push_button_set_edge_color_bottom_right_highlighted(AWE_OBJECT *obj, void *data)
{
    AWE_PUSH_BUTTON *tmp = (AWE_PUSH_BUTTON *)obj;
    memcpy(&tmp->texture[AWE_PUSH_BUTTON_TEXTURE_HIGHLIGHTED].edge_col[AWE_PUSH_BUTTON_EDGE_BOTTOM_RIGHT], (RGB *)data, sizeof(RGB));
    awe_set_widget_dirty(&tmp->widget);
}


//gets the top left edge color
static void _push_button_get_edge_color_top_left_focused(AWE_OBJECT *obj, void *data)
{
    AWE_PUSH_BUTTON *tmp = (AWE_PUSH_BUTTON *)obj;
    memcpy((RGB *)data, &tmp->texture[AWE_PUSH_BUTTON_TEXTURE_FOCUSED].edge_col[AWE_PUSH_BUTTON_EDGE_TOP_LEFT], sizeof(RGB));
}


//sets the top left edge color
static void _push_button_set_edge_color_top_left_focused(AWE_OBJECT *obj, void *data)
{
    AWE_PUSH_BUTTON *tmp = (AWE_PUSH_BUTTON *)obj;
    memcpy(&tmp->texture[AWE_PUSH_BUTTON_TEXTURE_FOCUSED].edge_col[AWE_PUSH_BUTTON_EDGE_TOP_LEFT], (RGB *)data, sizeof(RGB));
    awe_set_widget_dirty(&tmp->widget);
}


//gets the bottom right edge color
static void _push_button_get_edge_color_bottom_right_focused(AWE_OBJECT *obj, void *data)
{
    AWE_PUSH_BUTTON *tmp = (AWE_PUSH_BUTTON *)obj;
    memcpy((RGB *)data, &tmp->texture[AWE_PUSH_BUTTON_TEXTURE_FOCUSED].edge_col[AWE_PUSH_BUTTON_EDGE_BOTTOM_RIGHT], sizeof(RGB));
}


//sets the bottom right edge color
static void _push_button_set_edge_color_bottom_right_focused(AWE_OBJECT *obj, void *data)
{
    AWE_PUSH_BUTTON *tmp = (AWE_PUSH_BUTTON *)obj;
    memcpy(&tmp->texture[AWE_PUSH_BUTTON_TEXTURE_FOCUSED].edge_col[AWE_PUSH_BUTTON_EDGE_BOTTOM_RIGHT], (RGB *)data, sizeof(RGB));
    awe_set_widget_dirty(&tmp->widget);
}


//gets the enabled font color
static void _push_button_get_font_color_enabled(AWE_OBJECT *obj, void *data)
{
    AWE_PUSH_BUTTON *tmp = (AWE_PUSH_BUTTON *)obj;
    memcpy((RGB *)data, &tmp->texture[AWE_PUSH_BUTTON_TEXTURE_ENABLED].font_col, sizeof(RGB));
}


//sets the enabled font color
static void _push_button_set_font_color_enabled(AWE_OBJECT *obj, void *data)
{
    AWE_PUSH_BUTTON *tmp = (AWE_PUSH_BUTTON *)obj;
    memcpy(&tmp->texture[AWE_PUSH_BUTTON_TEXTURE_ENABLED].font_col, (RGB *)data, sizeof(RGB));
    awe_set_widget_dirty(&tmp->widget);
}


//gets the disabled font color
static void _push_button_get_font_color_disabled(AWE_OBJECT *obj, void *data)
{
    AWE_PUSH_BUTTON *tmp = (AWE_PUSH_BUTTON *)obj;
    memcpy((RGB *)data, &tmp->texture[AWE_PUSH_BUTTON_TEXTURE_DISABLED].font_col, sizeof(RGB));
}


//sets the disabled font color
static void _push_button_set_font_color_disabled(AWE_OBJECT *obj, void *data)
{
    AWE_PUSH_BUTTON *tmp = (AWE_PUSH_BUTTON *)obj;
    memcpy(&tmp->texture[AWE_PUSH_BUTTON_TEXTURE_DISABLED].font_col, (RGB *)data, sizeof(RGB));
    awe_set_widget_dirty(&tmp->widget);
}


//gets the pressed font color
static void _push_button_get_font_color_pressed(AWE_OBJECT *obj, void *data)
{
    AWE_PUSH_BUTTON *tmp = (AWE_PUSH_BUTTON *)obj;
    memcpy((RGB *)data, &tmp->texture[AWE_PUSH_BUTTON_TEXTURE_PRESSED].font_col, sizeof(RGB));
}


//sets the pressed font color
static void _push_button_set_font_color_pressed(AWE_OBJECT *obj, void *data)
{
    AWE_PUSH_BUTTON *tmp = (AWE_PUSH_BUTTON *)obj;
    memcpy(&tmp->texture[AWE_PUSH_BUTTON_TEXTURE_PRESSED].font_col, (RGB *)data, sizeof(RGB));
    awe_set_widget_dirty(&tmp->widget);
}


//gets the highlighted font color
static void _push_button_get_font_color_highlighted(AWE_OBJECT *obj, void *data)
{
    AWE_PUSH_BUTTON *tmp = (AWE_PUSH_BUTTON *)obj;
    memcpy((RGB *)data, &tmp->texture[AWE_PUSH_BUTTON_TEXTURE_HIGHLIGHTED].font_col, sizeof(RGB));
}


//sets the highlighted font color
static void _push_button_set_font_color_highlighted(AWE_OBJECT *obj, void *data)
{
    AWE_PUSH_BUTTON *tmp = (AWE_PUSH_BUTTON *)obj;
    memcpy(&tmp->texture[AWE_PUSH_BUTTON_TEXTURE_HIGHLIGHTED].font_col, (RGB *)data, sizeof(RGB));
    awe_set_widget_dirty(&tmp->widget);
}


//gets the focused font color
static void _push_button_get_font_color_focused(AWE_OBJECT *obj, void *data)
{
    AWE_PUSH_BUTTON *tmp = (AWE_PUSH_BUTTON *)obj;
    memcpy((RGB *)data, &tmp->texture[AWE_PUSH_BUTTON_TEXTURE_FOCUSED].font_col, sizeof(RGB));
}


//sets the focused font color
static void _push_button_set_font_color_focused(AWE_OBJECT *obj, void *data)
{
    AWE_PUSH_BUTTON *tmp = (AWE_PUSH_BUTTON *)obj;
    memcpy(&tmp->texture[AWE_PUSH_BUTTON_TEXTURE_FOCUSED].font_col, (RGB *)data, sizeof(RGB));
    awe_set_widget_dirty(&tmp->widget);
}


//gets the enabled font color
static void _push_button_get_font_shadow_enabled(AWE_OBJECT *obj, void *data)
{
    AWE_PUSH_BUTTON *tmp = (AWE_PUSH_BUTTON *)obj;
    memcpy((RGB *)data, &tmp->texture[AWE_PUSH_BUTTON_TEXTURE_ENABLED].font_sdw, sizeof(RGB));
}


//sets the enabled font color
static void _push_button_set_font_shadow_enabled(AWE_OBJECT *obj, void *data)
{
    AWE_PUSH_BUTTON *tmp = (AWE_PUSH_BUTTON *)obj;
    memcpy(&tmp->texture[AWE_PUSH_BUTTON_TEXTURE_ENABLED].font_sdw, (RGB *)data, sizeof(RGB));
    awe_set_widget_dirty(&tmp->widget);
}


//gets the disabled font color
static void _push_button_get_font_shadow_disabled(AWE_OBJECT *obj, void *data)
{
    AWE_PUSH_BUTTON *tmp = (AWE_PUSH_BUTTON *)obj;
    memcpy((RGB *)data, &tmp->texture[AWE_PUSH_BUTTON_TEXTURE_DISABLED].font_sdw, sizeof(RGB));
}


//sets the disabled font color
static void _push_button_set_font_shadow_disabled(AWE_OBJECT *obj, void *data)
{
    AWE_PUSH_BUTTON *tmp = (AWE_PUSH_BUTTON *)obj;
    memcpy(&tmp->texture[AWE_PUSH_BUTTON_TEXTURE_DISABLED].font_sdw, (RGB *)data, sizeof(RGB));
    awe_set_widget_dirty(&tmp->widget);
}


//gets the pressed font color
static void _push_button_get_font_shadow_pressed(AWE_OBJECT *obj, void *data)
{
    AWE_PUSH_BUTTON *tmp = (AWE_PUSH_BUTTON *)obj;
    memcpy((RGB *)data, &tmp->texture[AWE_PUSH_BUTTON_TEXTURE_PRESSED].font_sdw, sizeof(RGB));
}


//sets the pressed font color
static void _push_button_set_font_shadow_pressed(AWE_OBJECT *obj, void *data)
{
    AWE_PUSH_BUTTON *tmp = (AWE_PUSH_BUTTON *)obj;
    memcpy(&tmp->texture[AWE_PUSH_BUTTON_TEXTURE_PRESSED].font_sdw, (RGB *)data, sizeof(RGB));
    awe_set_widget_dirty(&tmp->widget);
}


//gets the highlighted font color
static void _push_button_get_font_shadow_highlighted(AWE_OBJECT *obj, void *data)
{
    AWE_PUSH_BUTTON *tmp = (AWE_PUSH_BUTTON *)obj;
    memcpy((RGB *)data, &tmp->texture[AWE_PUSH_BUTTON_TEXTURE_HIGHLIGHTED].font_sdw, sizeof(RGB));
}


//sets the highlighted font color
static void _push_button_set_font_shadow_highlighted(AWE_OBJECT *obj, void *data)
{
    AWE_PUSH_BUTTON *tmp = (AWE_PUSH_BUTTON *)obj;
    memcpy(&tmp->texture[AWE_PUSH_BUTTON_TEXTURE_HIGHLIGHTED].font_sdw, (RGB *)data, sizeof(RGB));
    awe_set_widget_dirty(&tmp->widget);
}


//gets the focused font color
static void _push_button_get_font_shadow_focused(AWE_OBJECT *obj, void *data)
{
    AWE_PUSH_BUTTON *tmp = (AWE_PUSH_BUTTON *)obj;
    memcpy((RGB *)data, &tmp->texture[AWE_PUSH_BUTTON_TEXTURE_FOCUSED].font_sdw, sizeof(RGB));
}


//sets the focused font color
static void _push_button_set_font_shadow_focused(AWE_OBJECT *obj, void *data)
{
    AWE_PUSH_BUTTON *tmp = (AWE_PUSH_BUTTON *)obj;
    memcpy(&tmp->texture[AWE_PUSH_BUTTON_TEXTURE_FOCUSED].font_sdw, (RGB *)data, sizeof(RGB));
    awe_set_widget_dirty(&tmp->widget);
}


//awe push_button properties
static AWE_CLASS_PROPERTY _push_button_properties[] = {
    { AWE_ID_TEXT, "const char *", sizeof(const char *), _push_button_get_text, _push_button_set_text, 0 },
    { AWE_ID_FONT, "FONT *", sizeof(FONT *), _push_button_get_font, _push_button_set_font, 0 },
    { AWE_ID_BITMAP, "BITMAP *", sizeof(BITMAP *), _push_button_get_bitmap, _push_button_set_bitmap, 0 },
    { AWE_ID_BITMAP_DIR, "int", sizeof(int), _push_button_get_bitmap_dir, _push_button_set_bitmap_dir, 0 },
    { AWE_ID_MARGIN_TOP, "int", sizeof(int), _push_button_get_margin_top, _push_button_set_margin_top, 0 },
    { AWE_ID_MARGIN_LEFT, "int", sizeof(int), _push_button_get_margin_left, _push_button_set_margin_left, 0 },
    { AWE_ID_MARGIN_RIGHT, "int", sizeof(int), _push_button_get_margin_right, _push_button_set_margin_right, 0 },
    { AWE_ID_MARGIN_BOTTOM, "int", sizeof(int), _push_button_get_margin_bottom, _push_button_set_margin_bottom, 0 },
    { AWE_ID_FACE_COLOR_TOP_LEFT_ENABLED, "RGB", sizeof(RGB), _push_button_get_face_color_top_left_enabled, _push_button_set_face_color_top_left_enabled, 0 },
    { AWE_ID_FACE_COLOR_TOP_RIGHT_ENABLED, "RGB", sizeof(RGB), _push_button_get_face_color_top_right_enabled, _push_button_set_face_color_top_right_enabled, 0 },
    { AWE_ID_FACE_COLOR_BOTTOM_LEFT_ENABLED, "RGB", sizeof(RGB), _push_button_get_face_color_bottom_left_enabled, _push_button_set_face_color_bottom_left_enabled, 0 },
    { AWE_ID_FACE_COLOR_BOTTOM_RIGHT_ENABLED, "RGB", sizeof(RGB), _push_button_get_face_color_bottom_right_enabled, _push_button_set_face_color_bottom_right_enabled, 0 },
    { AWE_ID_FACE_COLOR_TOP_LEFT_DISABLED, "RGB", sizeof(RGB), _push_button_get_face_color_top_left_disabled, _push_button_set_face_color_top_left_disabled, 0 },
    { AWE_ID_FACE_COLOR_TOP_RIGHT_DISABLED, "RGB", sizeof(RGB), _push_button_get_face_color_top_right_disabled, _push_button_set_face_color_top_right_disabled, 0 },
    { AWE_ID_FACE_COLOR_BOTTOM_LEFT_DISABLED, "RGB", sizeof(RGB), _push_button_get_face_color_bottom_left_disabled, _push_button_set_face_color_bottom_left_disabled, 0 },
    { AWE_ID_FACE_COLOR_BOTTOM_RIGHT_DISABLED, "RGB", sizeof(RGB), _push_button_get_face_color_bottom_right_disabled, _push_button_set_face_color_bottom_right_disabled, 0 },
    { AWE_ID_FACE_COLOR_TOP_LEFT_PRESSED, "RGB", sizeof(RGB), _push_button_get_face_color_top_left_pressed, _push_button_set_face_color_top_left_pressed, 0 },
    { AWE_ID_FACE_COLOR_TOP_RIGHT_PRESSED, "RGB", sizeof(RGB), _push_button_get_face_color_top_right_pressed, _push_button_set_face_color_top_right_pressed, 0 },
    { AWE_ID_FACE_COLOR_BOTTOM_LEFT_PRESSED, "RGB", sizeof(RGB), _push_button_get_face_color_bottom_left_pressed, _push_button_set_face_color_bottom_left_pressed, 0 },
    { AWE_ID_FACE_COLOR_BOTTOM_RIGHT_PRESSED, "RGB", sizeof(RGB), _push_button_get_face_color_bottom_right_pressed, _push_button_set_face_color_bottom_right_pressed, 0 },
    { AWE_ID_FACE_COLOR_TOP_LEFT_HIGHLIGHTED, "RGB", sizeof(RGB), _push_button_get_face_color_top_left_highlighted, _push_button_set_face_color_top_left_highlighted, 0 },
    { AWE_ID_FACE_COLOR_TOP_RIGHT_HIGHLIGHTED, "RGB", sizeof(RGB), _push_button_get_face_color_top_right_highlighted, _push_button_set_face_color_top_right_highlighted, 0 },
    { AWE_ID_FACE_COLOR_BOTTOM_LEFT_HIGHLIGHTED, "RGB", sizeof(RGB), _push_button_get_face_color_bottom_left_highlighted, _push_button_set_face_color_bottom_left_highlighted, 0 },
    { AWE_ID_FACE_COLOR_BOTTOM_RIGHT_HIGHLIGHTED, "RGB", sizeof(RGB), _push_button_get_face_color_bottom_right_highlighted, _push_button_set_face_color_bottom_right_highlighted, 0 },
    { AWE_ID_FACE_COLOR_TOP_LEFT_FOCUSED, "RGB", sizeof(RGB), _push_button_get_face_color_top_left_focused, _push_button_set_face_color_top_left_focused, 0 },
    { AWE_ID_FACE_COLOR_TOP_RIGHT_FOCUSED, "RGB", sizeof(RGB), _push_button_get_face_color_top_right_focused, _push_button_set_face_color_top_right_focused, 0 },
    { AWE_ID_FACE_COLOR_BOTTOM_LEFT_FOCUSED, "RGB", sizeof(RGB), _push_button_get_face_color_bottom_left_focused, _push_button_set_face_color_bottom_left_focused, 0 },
    { AWE_ID_FACE_COLOR_BOTTOM_RIGHT_FOCUSED, "RGB", sizeof(RGB), _push_button_get_face_color_bottom_right_focused, _push_button_set_face_color_bottom_right_focused, 0 },
    { AWE_ID_EDGE_COLOR_TOP_LEFT_ENABLED, "RGB", sizeof(RGB), _push_button_get_edge_color_top_left_enabled, _push_button_set_edge_color_top_left_enabled, 0 },
    { AWE_ID_EDGE_COLOR_BOTTOM_RIGHT_ENABLED, "RGB", sizeof(RGB), _push_button_get_edge_color_bottom_right_enabled, _push_button_set_edge_color_bottom_right_enabled, 0 },
    { AWE_ID_EDGE_COLOR_TOP_LEFT_DISABLED, "RGB", sizeof(RGB), _push_button_get_edge_color_top_left_disabled, _push_button_set_edge_color_top_left_disabled, 0 },
    { AWE_ID_EDGE_COLOR_BOTTOM_RIGHT_DISABLED, "RGB", sizeof(RGB), _push_button_get_edge_color_bottom_right_disabled, _push_button_set_edge_color_bottom_right_disabled, 0 },    
    { AWE_ID_EDGE_COLOR_TOP_LEFT_PRESSED, "RGB", sizeof(RGB), _push_button_get_edge_color_top_left_pressed, _push_button_set_edge_color_top_left_pressed, 0 },
    { AWE_ID_EDGE_COLOR_BOTTOM_RIGHT_PRESSED, "RGB", sizeof(RGB), _push_button_get_edge_color_bottom_right_pressed, _push_button_set_edge_color_bottom_right_pressed, 0 },
    { AWE_ID_EDGE_COLOR_TOP_LEFT_HIGHLIGHTED, "RGB", sizeof(RGB), _push_button_get_edge_color_top_left_highlighted, _push_button_set_edge_color_top_left_highlighted, 0 },
    { AWE_ID_EDGE_COLOR_BOTTOM_RIGHT_HIGHLIGHTED, "RGB", sizeof(RGB), _push_button_get_edge_color_bottom_right_highlighted, _push_button_set_edge_color_bottom_right_highlighted, 0 },
    { AWE_ID_EDGE_COLOR_TOP_LEFT_FOCUSED, "RGB", sizeof(RGB), _push_button_get_edge_color_top_left_focused, _push_button_set_edge_color_top_left_focused, 0 },
    { AWE_ID_EDGE_COLOR_BOTTOM_RIGHT_FOCUSED, "RGB", sizeof(RGB), _push_button_get_edge_color_bottom_right_focused, _push_button_set_edge_color_bottom_right_focused, 0 },
    { AWE_ID_FONT_COLOR_ENABLED, "RGB", sizeof(RGB), _push_button_get_font_color_enabled, _push_button_set_font_color_enabled, 0 },
    { AWE_ID_FONT_COLOR_DISABLED, "RGB", sizeof(RGB), _push_button_get_font_color_disabled, _push_button_set_font_color_disabled, 0 },
    { AWE_ID_FONT_COLOR_PRESSED, "RGB", sizeof(RGB), _push_button_get_font_color_pressed, _push_button_set_font_color_pressed, 0 },
    { AWE_ID_FONT_COLOR_HIGHLIGHTED, "RGB", sizeof(RGB), _push_button_get_font_color_highlighted, _push_button_set_font_color_highlighted, 0 },
    { AWE_ID_FONT_COLOR_FOCUSED, "RGB", sizeof(RGB), _push_button_get_font_color_focused, _push_button_set_font_color_focused, 0 },
    { AWE_ID_SHADOW_COLOR_ENABLED, "RGB", sizeof(RGB), _push_button_get_font_shadow_enabled, _push_button_set_font_shadow_enabled, 0 },
    { AWE_ID_SHADOW_COLOR_DISABLED, "RGB", sizeof(RGB), _push_button_get_font_shadow_disabled, _push_button_set_font_shadow_disabled, 0 },
    { AWE_ID_SHADOW_COLOR_PRESSED, "RGB", sizeof(RGB), _push_button_get_font_shadow_pressed, _push_button_set_font_shadow_pressed, 0 },
    { AWE_ID_SHADOW_COLOR_HIGHLIGHTED, "RGB", sizeof(RGB), _push_button_get_font_shadow_highlighted, _push_button_set_font_shadow_highlighted, 0 },
    { AWE_ID_SHADOW_COLOR_FOCUSED, "RGB", sizeof(RGB), _push_button_get_font_shadow_focused, _push_button_set_font_shadow_focused, 0 }, 

    // Add texture and anim

    { 0 }
};


/*****************************************************************************
    PUBLIC
 *****************************************************************************/


//push_button vtable
AWE_PUSH_BUTTON_VTABLE awe_push_button_vtable = {
    //widget
    {
        //object
        {
            awe_push_button_get_interface,
            awe_widget_properties_changed,
            awe_push_button_clone
        },
        awe_push_button_paint,
        awe_push_button_down,
        awe_push_button_up,
        awe_push_button_mouse_enter,
        awe_widget_mouse_move,
        awe_push_button_mouse_leave,
        awe_widget_mouse_wheel,
        awe_widget_key_down,
        awe_widget_key_up,
        awe_push_button_timer,
        awe_widget_get_focus,
        awe_widget_lose_focus,
        awe_widget_begin_display,
        awe_widget_end_display,
        awe_widget_insert_widget,
        awe_widget_remove_widget,
        awe_geometry_manager_geometry_changed,
        awe_geometry_manager_z_order_changed,
        awe_geometry_manager_visible_changed,
        0
    }
    ,
    //control
    {
        awe_push_button_key_press
    }
    ,
    //geometry
    {
        0,
        awe_push_button_set_geometry,
        0
    }
};


//push_button class
AWE_CLASS awe_push_button_class = {
    AWE_ID_PUSH_BUTTON,
    AWE_ID_AWE,
    &awe_widget_class,
    sizeof(AWE_PUSH_BUTTON),
    _push_button_properties,
    _push_button_events,
    &awe_push_button_vtable.widget.object,
    _push_button_constructor,
    _push_button_destructor
};


void *awe_push_button_get_interface(AWE_OBJECT *obj, const char *name, const char *pnamespace)
{
    if (!strcmp(pnamespace, AWE_ID_AWE)) {
        if (!strcmp(name, AWE_ID_PUSH_BUTTON)) {
            return (AWE_PUSH_BUTTON_VTABLE *)obj->pclass->vtable;
        }
        if (!strcmp(name, AWE_ID_CONTROL)) {
            return &((AWE_PUSH_BUTTON_VTABLE *)obj->pclass->vtable)->control;
        }
        if (!strcmp(name, AWE_ID_GEOMETRY_MANAGER)) {
            return &((AWE_PUSH_BUTTON_VTABLE *)obj->pclass->vtable)->geometry_manager;
        }
    }
    return awe_widget_get_interface(obj, name, pnamespace);
}


//clone widget
AWE_OBJECT *awe_push_button_clone(AWE_OBJECT *wgt)
{
    return awe_create_object(&awe_push_button_class, 
        AWE_ID_X                                  , ((AWE_WIDGET *)wgt)->x                                                                                            ,
        AWE_ID_Y                                  , ((AWE_WIDGET *)wgt)->y                                                                                            ,
        AWE_ID_WIDTH                              , ((AWE_WIDGET *)wgt)->width                                                                                        ,
        AWE_ID_HEIGHT                             , ((AWE_WIDGET *)wgt)->height                                                                                       ,
        AWE_ID_VISIBLE                            , ((AWE_WIDGET *)wgt)->visible                                                                                      ,
        AWE_ID_ENABLED                            , ((AWE_WIDGET *)wgt)->enabled                                                                                      ,
        AWE_ID_OPAQUE                             , ((AWE_WIDGET *)wgt)->opaque                                                                                       , 
        AWE_ID_TRANSLUCENCY                       , ((AWE_WIDGET *)wgt)->translucency                                                                                 ,
        AWE_ID_OUTPUT_TYPE                        , ((AWE_WIDGET *)wgt)->output_type                                                                                  ,
        AWE_ID_TEXT                               , ((AWE_PUSH_BUTTON *)wgt)->text                                                                                    ,
        AWE_ID_FONT                               , ((AWE_PUSH_BUTTON *)wgt)->font                                                                                    ,
        AWE_ID_BITMAP                             , ((AWE_PUSH_BUTTON *)wgt)->bitmap                                                                                    ,
        AWE_ID_BITMAP_DIR                         , ((AWE_PUSH_BUTTON *)wgt)->bitmap_dir                                                                                ,
        AWE_ID_MARGIN_TOP                         , ((AWE_PUSH_BUTTON *)wgt)->margin.top                                                                              ,
        AWE_ID_MARGIN_LEFT                        , ((AWE_PUSH_BUTTON *)wgt)->margin.left                                                                             ,
        AWE_ID_MARGIN_RIGHT                       , ((AWE_PUSH_BUTTON *)wgt)->margin.right                                                                            ,
        AWE_ID_MARGIN_BOTTOM                      , ((AWE_PUSH_BUTTON *)wgt)->margin.bottom                                                                           ,                                                                         
        AWE_ID_FONT_COLOR_ENABLED                 , ((AWE_PUSH_BUTTON *)wgt)->texture[AWE_PUSH_BUTTON_TEXTURE_ENABLED].font_col                                       ,
        AWE_ID_FONT_COLOR_DISABLED                , ((AWE_PUSH_BUTTON *)wgt)->texture[AWE_PUSH_BUTTON_TEXTURE_DISABLED].font_col                                      ,
        AWE_ID_FONT_COLOR_PRESSED                 , ((AWE_PUSH_BUTTON *)wgt)->texture[AWE_PUSH_BUTTON_TEXTURE_PRESSED].font_col                                       ,
        AWE_ID_FONT_COLOR_HIGHLIGHTED             , ((AWE_PUSH_BUTTON *)wgt)->texture[AWE_PUSH_BUTTON_TEXTURE_HIGHLIGHTED].font_col                                   ,
        AWE_ID_FONT_COLOR_FOCUSED                 , ((AWE_PUSH_BUTTON *)wgt)->texture[AWE_PUSH_BUTTON_TEXTURE_FOCUSED].font_col                                       ,
        AWE_ID_SHADOW_COLOR_ENABLED               , ((AWE_PUSH_BUTTON *)wgt)->texture[AWE_PUSH_BUTTON_TEXTURE_ENABLED].font_sdw                                       ,
        AWE_ID_SHADOW_COLOR_DISABLED              , ((AWE_PUSH_BUTTON *)wgt)->texture[AWE_PUSH_BUTTON_TEXTURE_DISABLED].font_sdw                                      ,
        AWE_ID_SHADOW_COLOR_PRESSED               , ((AWE_PUSH_BUTTON *)wgt)->texture[AWE_PUSH_BUTTON_TEXTURE_PRESSED].font_sdw                                       ,
        AWE_ID_SHADOW_COLOR_HIGHLIGHTED           , ((AWE_PUSH_BUTTON *)wgt)->texture[AWE_PUSH_BUTTON_TEXTURE_HIGHLIGHTED].font_sdw                                   ,
        AWE_ID_SHADOW_COLOR_FOCUSED               , ((AWE_PUSH_BUTTON *)wgt)->texture[AWE_PUSH_BUTTON_TEXTURE_FOCUSED].font_sdw                                       ,
        AWE_ID_FACE_COLOR_TOP_LEFT_ENABLED        , ((AWE_PUSH_BUTTON *)wgt)->texture[AWE_PUSH_BUTTON_TEXTURE_ENABLED].face_col[AWE_PUSH_BUTTON_FACE_TOP_LEFT]        ,
        AWE_ID_FACE_COLOR_TOP_LEFT_DISABLED       , ((AWE_PUSH_BUTTON *)wgt)->texture[AWE_PUSH_BUTTON_TEXTURE_DISABLED].face_col[AWE_PUSH_BUTTON_FACE_TOP_LEFT]       ,
        AWE_ID_FACE_COLOR_TOP_LEFT_PRESSED        , ((AWE_PUSH_BUTTON *)wgt)->texture[AWE_PUSH_BUTTON_TEXTURE_PRESSED].face_col[AWE_PUSH_BUTTON_FACE_TOP_LEFT]        ,
        AWE_ID_FACE_COLOR_TOP_LEFT_HIGHLIGHTED    , ((AWE_PUSH_BUTTON *)wgt)->texture[AWE_PUSH_BUTTON_TEXTURE_HIGHLIGHTED].face_col[AWE_PUSH_BUTTON_FACE_TOP_LEFT]    ,
        AWE_ID_FACE_COLOR_TOP_LEFT_FOCUSED        , ((AWE_PUSH_BUTTON *)wgt)->texture[AWE_PUSH_BUTTON_TEXTURE_FOCUSED].face_col[AWE_PUSH_BUTTON_FACE_TOP_LEFT]        ,
        AWE_ID_FACE_COLOR_TOP_RIGHT_ENABLED       , ((AWE_PUSH_BUTTON *)wgt)->texture[AWE_PUSH_BUTTON_TEXTURE_ENABLED].face_col[AWE_PUSH_BUTTON_FACE_TOP_RIGHT]       ,
        AWE_ID_FACE_COLOR_TOP_RIGHT_DISABLED      , ((AWE_PUSH_BUTTON *)wgt)->texture[AWE_PUSH_BUTTON_TEXTURE_DISABLED].face_col[AWE_PUSH_BUTTON_FACE_TOP_RIGHT]      ,
        AWE_ID_FACE_COLOR_TOP_RIGHT_PRESSED       , ((AWE_PUSH_BUTTON *)wgt)->texture[AWE_PUSH_BUTTON_TEXTURE_PRESSED].face_col[AWE_PUSH_BUTTON_FACE_TOP_RIGHT]       ,
        AWE_ID_FACE_COLOR_TOP_RIGHT_HIGHLIGHTED   , ((AWE_PUSH_BUTTON *)wgt)->texture[AWE_PUSH_BUTTON_TEXTURE_HIGHLIGHTED].face_col[AWE_PUSH_BUTTON_FACE_TOP_RIGHT]   ,
        AWE_ID_FACE_COLOR_TOP_RIGHT_FOCUSED       , ((AWE_PUSH_BUTTON *)wgt)->texture[AWE_PUSH_BUTTON_TEXTURE_FOCUSED].face_col[AWE_PUSH_BUTTON_FACE_TOP_RIGHT]       ,
        AWE_ID_FACE_COLOR_BOTTOM_LEFT_ENABLED     , ((AWE_PUSH_BUTTON *)wgt)->texture[AWE_PUSH_BUTTON_TEXTURE_ENABLED].face_col[AWE_PUSH_BUTTON_FACE_BOTTOM_LEFT]     ,
        AWE_ID_FACE_COLOR_BOTTOM_LEFT_DISABLED    , ((AWE_PUSH_BUTTON *)wgt)->texture[AWE_PUSH_BUTTON_TEXTURE_DISABLED].face_col[AWE_PUSH_BUTTON_FACE_BOTTOM_LEFT]    ,
        AWE_ID_FACE_COLOR_BOTTOM_LEFT_PRESSED     , ((AWE_PUSH_BUTTON *)wgt)->texture[AWE_PUSH_BUTTON_TEXTURE_PRESSED].face_col[AWE_PUSH_BUTTON_FACE_BOTTOM_LEFT]     ,
        AWE_ID_FACE_COLOR_BOTTOM_LEFT_HIGHLIGHTED , ((AWE_PUSH_BUTTON *)wgt)->texture[AWE_PUSH_BUTTON_TEXTURE_HIGHLIGHTED].face_col[AWE_PUSH_BUTTON_FACE_BOTTOM_LEFT] ,
        AWE_ID_FACE_COLOR_BOTTOM_LEFT_FOCUSED     , ((AWE_PUSH_BUTTON *)wgt)->texture[AWE_PUSH_BUTTON_TEXTURE_FOCUSED].face_col[AWE_PUSH_BUTTON_FACE_BOTTOM_LEFT]     ,
        AWE_ID_FACE_COLOR_BOTTOM_RIGHT_ENABLED    , ((AWE_PUSH_BUTTON *)wgt)->texture[AWE_PUSH_BUTTON_TEXTURE_ENABLED].face_col[AWE_PUSH_BUTTON_FACE_BOTTOM_RIGHT]    ,
        AWE_ID_FACE_COLOR_BOTTOM_RIGHT_DISABLED   , ((AWE_PUSH_BUTTON *)wgt)->texture[AWE_PUSH_BUTTON_TEXTURE_DISABLED].face_col[AWE_PUSH_BUTTON_FACE_BOTTOM_RIGHT]   ,
        AWE_ID_FACE_COLOR_BOTTOM_RIGHT_PRESSED    , ((AWE_PUSH_BUTTON *)wgt)->texture[AWE_PUSH_BUTTON_TEXTURE_PRESSED].face_col[AWE_PUSH_BUTTON_FACE_BOTTOM_RIGHT]    ,
        AWE_ID_FACE_COLOR_BOTTOM_RIGHT_HIGHLIGHTED, ((AWE_PUSH_BUTTON *)wgt)->texture[AWE_PUSH_BUTTON_TEXTURE_HIGHLIGHTED].face_col[AWE_PUSH_BUTTON_FACE_BOTTOM_RIGHT],
        AWE_ID_FACE_COLOR_BOTTOM_RIGHT_FOCUSED    , ((AWE_PUSH_BUTTON *)wgt)->texture[AWE_PUSH_BUTTON_TEXTURE_FOCUSED].face_col[AWE_PUSH_BUTTON_FACE_BOTTOM_RIGHT]    ,
        AWE_ID_EDGE_COLOR_TOP_LEFT_ENABLED        , ((AWE_PUSH_BUTTON *)wgt)->texture[AWE_PUSH_BUTTON_TEXTURE_ENABLED].edge_col[AWE_PUSH_BUTTON_EDGE_TOP_LEFT]        ,
        AWE_ID_EDGE_COLOR_TOP_LEFT_DISABLED       , ((AWE_PUSH_BUTTON *)wgt)->texture[AWE_PUSH_BUTTON_TEXTURE_DISABLED].edge_col[AWE_PUSH_BUTTON_EDGE_TOP_LEFT]       ,
        AWE_ID_EDGE_COLOR_TOP_LEFT_PRESSED        , ((AWE_PUSH_BUTTON *)wgt)->texture[AWE_PUSH_BUTTON_TEXTURE_PRESSED].edge_col[AWE_PUSH_BUTTON_EDGE_TOP_LEFT]        ,
        AWE_ID_EDGE_COLOR_TOP_LEFT_HIGHLIGHTED    , ((AWE_PUSH_BUTTON *)wgt)->texture[AWE_PUSH_BUTTON_TEXTURE_HIGHLIGHTED].edge_col[AWE_PUSH_BUTTON_EDGE_TOP_LEFT]    ,
        AWE_ID_EDGE_COLOR_TOP_LEFT_FOCUSED        , ((AWE_PUSH_BUTTON *)wgt)->texture[AWE_PUSH_BUTTON_TEXTURE_FOCUSED].edge_col[AWE_PUSH_BUTTON_EDGE_TOP_LEFT]        ,
        AWE_ID_EDGE_COLOR_BOTTOM_RIGHT_ENABLED    , ((AWE_PUSH_BUTTON *)wgt)->texture[AWE_PUSH_BUTTON_TEXTURE_ENABLED].edge_col[AWE_PUSH_BUTTON_EDGE_BOTTOM_RIGHT]    ,
        AWE_ID_EDGE_COLOR_BOTTOM_RIGHT_DISABLED   , ((AWE_PUSH_BUTTON *)wgt)->texture[AWE_PUSH_BUTTON_TEXTURE_DISABLED].edge_col[AWE_PUSH_BUTTON_EDGE_BOTTOM_RIGHT]   ,
        AWE_ID_EDGE_COLOR_BOTTOM_RIGHT_PRESSED    , ((AWE_PUSH_BUTTON *)wgt)->texture[AWE_PUSH_BUTTON_TEXTURE_PRESSED].edge_col[AWE_PUSH_BUTTON_EDGE_BOTTOM_RIGHT]    ,
        AWE_ID_EDGE_COLOR_BOTTOM_RIGHT_HIGHLIGHTED, ((AWE_PUSH_BUTTON *)wgt)->texture[AWE_PUSH_BUTTON_TEXTURE_HIGHLIGHTED].edge_col[AWE_PUSH_BUTTON_EDGE_BOTTOM_RIGHT],
        AWE_ID_EDGE_COLOR_BOTTOM_RIGHT_FOCUSED    , ((AWE_PUSH_BUTTON *)wgt)->texture[AWE_PUSH_BUTTON_TEXTURE_FOCUSED].edge_col[AWE_PUSH_BUTTON_EDGE_BOTTOM_RIGHT]    ,
        AWE_ID_TEXTURE_ENABLED                    , ((AWE_PUSH_BUTTON *)wgt)->texture[AWE_PUSH_BUTTON_TEXTURE_ENABLED].texture                                        ,
        AWE_ID_TEXTURE_DISABLED                   , ((AWE_PUSH_BUTTON *)wgt)->texture[AWE_PUSH_BUTTON_TEXTURE_DISABLED].texture                                       ,
        AWE_ID_TEXTURE_PRESSED                    , ((AWE_PUSH_BUTTON *)wgt)->texture[AWE_PUSH_BUTTON_TEXTURE_PRESSED].texture                                        ,
        AWE_ID_TEXTURE_HIGHLIGHTED                , ((AWE_PUSH_BUTTON *)wgt)->texture[AWE_PUSH_BUTTON_TEXTURE_HIGHLIGHTED].texture                                    ,
        AWE_ID_TEXTURE_FOCUSED                    , ((AWE_PUSH_BUTTON *)wgt)->texture[AWE_PUSH_BUTTON_TEXTURE_FOCUSED].texture                                        ,
        0);
}


//push_button paint
void awe_push_button_paint(AWE_WIDGET *wgt, AWE_CANVAS *canvas, const AWE_RECT *clip)
{
    AWE_PUSH_BUTTON *btn = (AWE_PUSH_BUTTON *)wgt;
    int tx = (wgt->width - gui_strlen(btn->text)) >> 1;
    int ty = (wgt->height - text_height(btn->font)) >> 1;
    int state;

    solid_mode();

    if(!awe_is_enabled_widget_tree(wgt))
        state = AWE_PUSH_BUTTON_TEXTURE_DISABLED;
    else if(btn->pressed)
        state = AWE_PUSH_BUTTON_TEXTURE_PRESSED;
    else if(awe_widget_has_mouse(wgt))
        state = AWE_PUSH_BUTTON_TEXTURE_HIGHLIGHTED;
    else if(awe_get_focus_widget() == wgt)
        state = AWE_PUSH_BUTTON_TEXTURE_FOCUSED;
    else
        state = AWE_PUSH_BUTTON_TEXTURE_ENABLED;

    awe_fill_gradient_s(canvas, 3, 3, wgt->width - 6, wgt->height - 6, 
        AWE_MAKE_COLOR(btn->texture[state].face_col[AWE_PUSH_BUTTON_FACE_TOP_LEFT]),
        AWE_MAKE_COLOR(btn->texture[state].face_col[AWE_PUSH_BUTTON_FACE_BOTTOM_LEFT]),
        AWE_MAKE_COLOR(btn->texture[state].face_col[AWE_PUSH_BUTTON_FACE_BOTTOM_RIGHT]),
        AWE_MAKE_COLOR(btn->texture[state].face_col[AWE_PUSH_BUTTON_FACE_TOP_RIGHT]));

    awe_draw_3d_rect_s(canvas, 0, 0, wgt->width, wgt->height, 
        AWE_MAKE_COLOR(btn->texture[state].edge_col[AWE_PUSH_BUTTON_EDGE_TOP_LEFT]),
        AWE_MAKE_COLOR(btn->texture[state].edge_col[AWE_PUSH_BUTTON_EDGE_BOTTOM_RIGHT]), 
        3);

    if(state == AWE_PUSH_BUTTON_TEXTURE_PRESSED){
        tx += 1;
        ty += 1;
    }

    if(AWE_MAKE_COLOR(btn->texture[state].font_sdw) != makecol(255, 0, 255))
        awe_draw_gui_text(canvas, btn->font, btn->text, tx + 1, ty + 1, AWE_MAKE_COLOR(btn->texture[state].font_sdw), -1);
    
    awe_draw_gui_text(canvas, btn->font, btn->text, tx, ty, AWE_MAKE_COLOR(btn->texture[state].font_col), -1);
    
    if (awe_get_focus_widget() == wgt && state != AWE_PUSH_BUTTON_TEXTURE_DISABLED)
        awe_draw_rect_pattern_s(canvas, 4, 4, wgt->width - 9, wgt->height - 9, AWE_MAKE_COLOR(btn->texture[state].font_col), AWE_PATTERN_DOT_DOT);
}


void awe_push_button_down(AWE_WIDGET *wgt, const AWE_EVENT *event)
{
    AWE_PUSH_BUTTON *btn = (AWE_PUSH_BUTTON *)wgt;
    if (!awe_set_focus_widget(wgt)) return;
    awe_enter_event_mode(awe_grab_event_proc, wgt);
    btn->pressed ^= 1;
    awe_do_widget_event0(wgt, AWE_ID_PUSH_BUTTON_PRESSED);
    awe_add_widget_timer(wgt, 0, 100);
    awe_set_widget_dirty(wgt);
}


void awe_push_button_up(AWE_WIDGET *wgt, const AWE_EVENT *event)
{
    AWE_PUSH_BUTTON *btn = (AWE_PUSH_BUTTON *)wgt;
    awe_leave_event_mode();
    btn->pressed ^= 1;
    if(awe_widget_has_mouse(wgt)){
        awe_do_widget_event0(wgt, AWE_ID_PUSH_BUTTON_ACTIVATED);
    }
    else{
        awe_do_widget_event0(wgt, AWE_ID_PUSH_BUTTON_RELEASED);
    }
    awe_remove_widget_timer(wgt, 0);
    awe_set_widget_dirty(wgt);
}


void awe_push_button_mouse_enter(AWE_WIDGET *wgt, const AWE_EVENT *event)
{
    awe_set_widget_dirty(wgt);
}


void awe_push_button_mouse_leave(AWE_WIDGET *wgt, const AWE_EVENT *event)
{
    awe_set_widget_dirty(wgt);
}


int awe_push_button_key_press(AWE_WIDGET *wgt, const AWE_EVENT *event)
{
    AWE_PUSH_BUTTON *btn = (AWE_PUSH_BUTTON *)wgt;
    const char *t = ustrchr(btn->text, '&');

    if (!t) return 0;
    t += ucwidth('&');
    if (utolower(ugetc(t)) != utolower(event->key.key)) return 0;
    if (!awe_set_focus_widget(wgt)) return 0;
    return 1;
}


void awe_push_button_timer(AWE_WIDGET *wgt, const AWE_EVENT *event)
{
    awe_do_widget_event0(wgt, AWE_ID_PUSH_BUTTON_HELD_DOWN);
}


void awe_push_button_set_geometry(AWE_WIDGET *wgt)
{
    AWE_PUSH_BUTTON *btn = (AWE_PUSH_BUTTON *)wgt;
    awe_override_widget_size(
        wgt, 
        text_length(btn->font,
        btn->text) + btn->margin.left + btn->margin.right, 
        text_height(btn->font) + btn->margin.top + btn->margin.bottom
    );
}

