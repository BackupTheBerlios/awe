#include "draganddrop.h"
#include "dataobjects.h"
#include <string.h>


//ids
#define AWE_ID_TEST          "Test"


//test struct
typedef struct _TEST {
    AWE_WIDGET widget;
    char *text;
} _TEST;


//test vtable
typedef struct _TEST_VTABLE {
    AWE_WIDGET_VTABLE widget;
    AWE_DRAG_AND_DROP_SOURCE_VTABLE source;
    AWE_DRAG_AND_DROP_TARGET_VTABLE target;
} _TEST_VTABLE;


extern AWE_CLASS test_class;


static AWE_RECT _drag_rect = {0, 0, 0, 0};
static int _last_x = 0;
static int _last_y = 0;


void _draw_drag_rect()
{
    xor_mode(1);
    scare_mouse();
    rect(screen, _drag_rect.left, _drag_rect.top, _drag_rect.right, _drag_rect.bottom, makecol(255, 255, 255));
    unscare_mouse();
    solid_mode();
}


//constructor
void _test_constructor(AWE_OBJECT *obj)
{
    _TEST *test = (_TEST *)obj;
    test->text = ustrdup(empty_string);
}


//destructor
void _test_destructor(AWE_OBJECT *obj)
{
    _TEST *test = (_TEST *)obj;
    free(test->text);
}


//gets the text
void _test_get_text(AWE_OBJECT *obj, void *data)
{
    _TEST *test = (_TEST *)obj;
    *(const char **)data = test->text;
}


//sets the text
void _test_set_text(AWE_OBJECT *obj, void *data)
{
    const char *new_text = *(const char **)data;
    _TEST *test = (_TEST *)obj;
    free(test->text);
    test->text = ustrdup(new_text ? new_text : empty_string);
    awe_set_widget_dirty(&test->widget);
}


//gets an interface
void *_test_get_interface(AWE_OBJECT *obj, const char *name, const char *pnamespace)
{
    if (!strcmp(name, AWE_ID_TEST)) return &obj->pclass->vtable;
    if (!strcmp(name, AWE_ID_DRAG_AND_DROP_SOURCE)) return &((_TEST_VTABLE *)obj->pclass->vtable)->source;
    if (!strcmp(name, AWE_ID_DRAG_AND_DROP_TARGET)) return &((_TEST_VTABLE *)obj->pclass->vtable)->target;
    return awe_widget_get_interface(obj, name, pnamespace);
}


//clones the object
AWE_OBJECT *_test_clone(AWE_OBJECT *obj)
{
    _TEST *test = (_TEST *)obj;

    return awe_create_object(&test_class, 
        AWE_ID_X           , test->widget.x           ,
        AWE_ID_Y           , test->widget.y           ,
        AWE_ID_WIDTH       , test->widget.width       ,
        AWE_ID_HEIGHT      , test->widget.height      ,
        AWE_ID_VISIBLE     , test->widget.visible     ,
        AWE_ID_ENABLED     , test->widget.enabled     ,
        AWE_ID_OPAQUE      , test->widget.opaque      ,
        AWE_ID_TRANSLUCENCY, test->widget.translucency,
        AWE_ID_OUTPUT_TYPE , test->widget.output_type ,
        AWE_ID_TEXT        , test->text               ,
        0);
}


//paints the object
void _test_paint(AWE_WIDGET *wgt, AL_CONST AWE_CANVAS *canvas, AL_CONST AWE_RECT *clip)
{
    _TEST *test = (_TEST *)wgt;

    solid_mode();
    awe_fill_rect_s(canvas, 0, 0, wgt->width, wgt->height, makecol(232, 232, 232));
    awe_draw_text(canvas, font, test->text, 0, 0, makecol(0, 0, 0), -1);
}


//button down
void _test_button_down(AWE_WIDGET *wgt, AL_CONST AWE_EVENT *ev)
{
    _TEST *test = (_TEST *)wgt;

    AWE_OBJECT *text_obj = awe_create_object(&awe_text_class,
        AWE_ID_TEXT, test->text,
        0);

    //init drag rect
    _drag_rect = *awe_get_widget_rect(wgt);
    _draw_drag_rect();
    _last_x = ev->mouse.x;
    _last_y = ev->mouse.y;

    //begin drag-n-drop
    awe_begin_drag_and_drop(wgt, text_obj);
}


//drag-n-drop begin
void _test_drag_and_drop_begin(AWE_WIDGET *wgt)
{
    TRACE("_test_drag_and_drop_begin\n");
}


//drag-n-drop end
void _test_drag_and_drop_end(AWE_WIDGET *wgt)
{
    const AWE_RECT *pos = awe_get_widget_rect(wgt);

    TRACE("_test_drag_and_drop_end\n");

    //erase drag-n-drop rect
    _draw_drag_rect();
}


//drag-n-drop clear
void _test_drag_and_drop_clear(AWE_WIDGET *wgt)
{
    awe_set_widget_properties(wgt,
        AWE_ID_TEXT, "cleared",
        0);

    TRACE("_test_drag_and_drop_clear\n");
}


//drag-n-drop source move
void _test_drag_and_drop_source_mouse_move(AWE_WIDGET *wgt, AL_CONST AWE_EVENT *ev, AWE_OBJECT *data)
{
    TRACE("_test_drag_and_drop_source_mouse_move\n");

    //erase and redraw the drag rect
    _draw_drag_rect();
    AWE_RECT_SHIFT(_drag_rect, ev->mouse.x - _last_x, ev->mouse.y - _last_y);
    _draw_drag_rect();
    _last_x = ev->mouse.x;
    _last_y = ev->mouse.y;
}


//drag-n-drop button up
void _test_drag_and_drop_button_up(AWE_WIDGET *wgt, AL_CONST AWE_EVENT *ev, AWE_OBJECT *data)
{
    const char *text;

    TRACE("_test_drag_and_drop_button_up\n");

    //the data object must be of type text
    if (!awe_object_is_class(data, AWE_ID_TEXT, AWE_ID_AWE)) return;

    //get text
    awe_get_object_properties(data, 
        AWE_ID_TEXT, &text,
        0);

    //set text of widget
    awe_set_widget_properties(wgt,
        AWE_ID_TEXT, text,
        0);

    //on shift, clear the source
    if (ev->mouse.shifts & KB_SHIFT_FLAG) awe_clear_drag_and_drop_source();
}


//drag-n-drop mouse enter
void _test_drag_and_drop_mouse_enter(AWE_WIDGET *wgt, AL_CONST AWE_EVENT *ev, AWE_OBJECT *data)
{
    TRACE("_test_drag_and_drop_mouse_enter\n");
}


//drag-n-drop mouse move
void _test_drag_and_drop_mouse_move(AWE_WIDGET *wgt, AL_CONST AWE_EVENT *ev, AWE_OBJECT *data)
{
    TRACE("_test_drag_and_drop_mouse_move\n");
}


//drag-n-drop mouse leave
void _test_drag_and_drop_mouse_leave(AWE_WIDGET *wgt, AL_CONST AWE_EVENT *ev, AWE_OBJECT *data)
{
    TRACE("_test_drag_and_drop_mouse_leave\n");
}


//drag-n-drop mouse wheel
void _test_drag_and_drop_mouse_wheel(AWE_WIDGET *wgt, AL_CONST AWE_EVENT *ev, AWE_OBJECT *data)
{
    TRACE("_test_drag_and_drop_mouse_wheel\n");
}


//key down
void _test_drag_and_drop_key_down(AWE_WIDGET *wgt, AL_CONST AWE_EVENT *ev, AWE_OBJECT *data)
{
    TRACE("_test_drag_and_drop_key_down\n");
}


//key up
void _test_drag_and_drop_key_up(AWE_WIDGET *wgt, AL_CONST AWE_EVENT *ev, AWE_OBJECT *data)
{
    TRACE("_test_drag_and_drop_key_up\n");
}


//properties
AWE_CLASS_PROPERTY _test_properties[] = {
    {AWE_ID_TEXT, "const char *", sizeof(const char *), _test_get_text, _test_set_text, 0},
    {0}
};


//vtable
_TEST_VTABLE _test_vtable = {
    //widget
    {
        //object
        {
            _test_get_interface,
            awe_widget_properties_changed,
            _test_clone
        },

        //widget methods
        _test_paint,
        _test_button_down,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        awe_widget_get_focus,
        awe_widget_loose_focus,
        awe_widget_begin_display,
        awe_widget_end_display,
        awe_widget_insert_widget,
        awe_widget_remove_widget,
        0,
        0,
        0,
        0,
    },

    //source
    {
        _test_drag_and_drop_begin,
        _test_drag_and_drop_end,
        _test_drag_and_drop_clear,
        _test_drag_and_drop_source_mouse_move
    },

    //target
    {
        _test_drag_and_drop_button_up,
        _test_drag_and_drop_mouse_enter,
        _test_drag_and_drop_mouse_move,
        _test_drag_and_drop_mouse_leave,
        _test_drag_and_drop_mouse_wheel,
        _test_drag_and_drop_key_down,
        _test_drag_and_drop_key_up
    }
};


//custom class that supports drag-and-drop of allegro strings
AWE_CLASS test_class = {
    AWE_ID_TEST,
    AWE_ID_AWE,
    &awe_widget_class,
    sizeof(_TEST),
    _test_properties,
    0,
    &_test_vtable.widget.object,
    _test_constructor,
    _test_destructor
};


int main()
{
    AWE_WIDGET *root, *test1, *test2;

    //install allegro
    allegro_init();
    install_keyboard();
    install_timer();
    install_mouse();

    //install AWE
    awe_install_input();

    //set a test video mode
    set_color_depth(32);
    if (set_gfx_mode(GFX_AUTODETECT_WINDOWED, 640, 480, 0, 0) < 0) return 0;
    show_mouse(screen);

    //set up the gui
    awe_set_gui_screen(screen);

    //create the root widget
    root = awe_create_widget(&awe_widget_class, 0,
        AWE_ID_WIDTH, SCREEN_W,
        AWE_ID_HEIGHT, SCREEN_H,
        0);

    //create the 1st test object
    test1 = awe_create_widget(&test_class, root,
        AWE_ID_X, 100,
        AWE_ID_Y, 50,
        AWE_ID_WIDTH, 200,
        AWE_ID_HEIGHT, 50,
        AWE_ID_TEXT, "test1",
        0);

    //create the 2nd test object
    test2 = awe_create_widget(&test_class, root,
        AWE_ID_X, 300,
        AWE_ID_Y, 250,
        AWE_ID_WIDTH, 200,
        AWE_ID_HEIGHT, 50,
        AWE_ID_TEXT, "test2",
        0);

    //show the root widget
    awe_set_root_widget(root);

    //event loop
    while (!keypressed()) {
        yield_timeslice();
        awe_do_events();
        awe_update_gui();        
    }

    readkey();
    allegro_exit();
    return 0;
}
END_OF_MAIN();
