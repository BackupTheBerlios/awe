#include "geomman.h"
#include "dataobjects.h"
#include "event.h"
#include <string.h>


//ids
#define AWE_ID_TEST          "Test"
#define AWE_ID_LAYOUT_TYPE   "LayoutType"


//layout type of test class
typedef enum LAYOUT_TYPE {
    LAYOUT_NONE,
    LAYOUT_WRAP,
    LAYOUT_HOR,
    LAYOUT_VER
} LAYOUT_TYPE;


//test struct
typedef struct _TEST {
    AWE_WIDGET widget;
    char *text;
    LAYOUT_TYPE layout;
    int layout_changed:1;
} _TEST;


//test vtable
typedef struct _TEST_VTABLE {
    AWE_WIDGET_VTABLE widget;
    AWE_GEOMETRY_MANAGER_VTABLE geometry_manager;
} _TEST_VTABLE;


extern AWE_CLASS test_class;


//constructor
void _test_constructor(AWE_OBJECT *obj)
{
    _TEST *test = (_TEST *)obj;
    test->text = ustrdup(empty_string);
    test->layout = LAYOUT_NONE;
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


//gets the layout type
void _test_get_layout_type(AWE_OBJECT *obj, void *data)
{
    _TEST *test = (_TEST *)obj;
    *(LAYOUT_TYPE *)data = test->layout;
}


//sets the layout type
void _test_set_layout_type(AWE_OBJECT *obj, void *data)
{
    _TEST *test = (_TEST *)obj;
    test->layout = *(LAYOUT_TYPE *)data;
    test->layout_changed = 1;
}


//gets an interface
void *_test_get_interface(AWE_OBJECT *obj, const char *name, const char *pnamespace)
{
    if (!strcmp(name, AWE_ID_TEST)) return &obj->pclass->vtable;
    if (!strcmp(name, AWE_ID_GEOMETRY_MANAGER)) return &((_TEST_VTABLE *)obj->pclass->vtable)->geometry_manager;
    return awe_widget_get_interface(obj, name, pnamespace);
}


//properties changed
void _test_properties_changed(AWE_OBJECT *obj)
{
    _TEST *test = (_TEST *)obj;

    awe_widget_properties_changed(obj);
    if (test->layout_changed) {
        test->layout_changed = 0;
        awe_manage_geometry(&test->widget);
    }
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
        AWE_ID_LAYOUT_TYPE , test->layout             ,
        0);
}


//paints the object
void _test_paint(AWE_WIDGET *wgt, AWE_CANVAS *canvas, const AWE_RECT *clip)
{
    _TEST *test;

    //if root, paint otherwise
    if (!wgt->parent) {
        awe_widget_paint(wgt, canvas, clip);
        return;
    }

    test = (_TEST *)wgt;

    solid_mode();
    awe_fill_rect_s(canvas, 0, 0, wgt->width, wgt->height, makecol(232, 232, 232));
    awe_draw_text(canvas, font, test->text, 0, 0, makecol(0, 0, 0), -1);
    awe_draw_rect_s(canvas, 0, 0, wgt->width, wgt->height, makecol(0, 0, 0));
}


static int _dx, _dy, _move = 0;


//move widget until button up
void _test_button_down(AWE_WIDGET *wgt, const AWE_EVENT *event)
{
    _dx = event->mouse.x;
    _dy = event->mouse.y;
    _move = 1;
    awe_enter_event_mode(awe_grab_event_proc, wgt);
}


//button up
void _test_button_up(AWE_WIDGET *wgt, const AWE_EVENT *event)
{
    awe_leave_event_mode();
    _move = 0;
}


//mouse move
void _test_mouse_move(AWE_WIDGET *wgt, const AWE_EVENT *event)
{
    int x, y;

    if (!_move) return;
    awe_get_widget_properties(wgt,
        AWE_ID_X, &x,
        AWE_ID_Y, &y,
        0);

    x += event->mouse.x - _dx;
    y += event->mouse.y - _dy;

    awe_set_widget_properties(wgt,
        AWE_ID_X, x,
        AWE_ID_Y, y,
        0);
}


//set the preferred geometry
static void _test_set_geometry(AWE_WIDGET *wgt)
{
    _TEST *test = (_TEST *)wgt;
    int max_w = 0, max_h = 0, v, x, y, w, h;
    AWE_WIDGET *child;

    //position children
    awe_geometry_manager_set_geometry(wgt);

    //we are interested in wrapping around children only
    if (test->layout != LAYOUT_WRAP) return;

    //find max width, height from visible children
    for(child = awe_get_first_child_widget(wgt);
        child;
        child = awe_get_next_sibling_widget(child)) {
        awe_get_widget_properties(child, 
            AWE_ID_VISIBLE, &v,
            AWE_ID_X, &x,
            AWE_ID_Y, &y,
            AWE_ID_WIDTH, &w,
            AWE_ID_HEIGHT, &h,
            0);
        if (!v) continue;
        max_w = MAX(max_w, x + w);
        max_h = MAX(max_h, y + h);
    }

    //specify geometry
    awe_override_widget_size(wgt, max_w, max_h);
}


//does layout
static void _test_do_layout(AWE_WIDGET *wgt)
{
    _TEST *test = (_TEST *)wgt;
    AWE_WIDGET *child;
    int x, y, w, h, v;

    switch (test->layout) {
        case LAYOUT_HOR:
            x = 0;
            awe_get_widget_properties(wgt, 
                AWE_ID_HEIGHT, &h,
                0);
            for(child = awe_get_first_child_widget(wgt);
                child;
                child = awe_get_next_sibling_widget(child)) {
                awe_get_widget_properties(child,
                    AWE_ID_WIDTH, &w,
                    AWE_ID_VISIBLE, &v,
                    0);
                if (!v) continue;
                awe_override_widget_geometry(child, x, 0, w, h);
                x += w;
            }
            break;

        case LAYOUT_VER:
            y = 0;
            awe_get_widget_properties(wgt, 
                AWE_ID_WIDTH, &w,
                0);
            for(child = awe_get_first_child_widget(wgt);
                child;
                child = awe_get_next_sibling_widget(child)) {
                awe_get_widget_properties(child,
                    AWE_ID_HEIGHT, &h,
                    AWE_ID_VISIBLE, &v,
                    0);
                if (!v) continue;
                awe_override_widget_geometry(child, 0, y, w, h);
                y += h;
            }
            break;
    }

    awe_geometry_manager_do_layout(wgt);
}


//layout enum
AWE_CLASS_ENUMERATION _layout_type_enum[] = {
    {"LAYOUT_NONE", LAYOUT_NONE},
    {"LAYOUT_WRAP", LAYOUT_WRAP},
    {"LAYOUT_HOR", LAYOUT_HOR},
    {"LAYOUT_VER", LAYOUT_VER},
    {0}
};


//properties
AWE_CLASS_PROPERTY _test_properties[] = {
    {AWE_ID_TEXT, "const char *", sizeof(const char *), _test_get_text, _test_set_text, 0},
    {AWE_ID_LAYOUT_TYPE, "LAYOUT_TYPE", sizeof(int), _test_get_layout_type, _test_set_layout_type, _layout_type_enum},
    {0}
};


//vtable
_TEST_VTABLE _test_vtable = {
    //widget
    {
        //object
        {
            _test_get_interface,
            _test_properties_changed,
            _test_clone
        },

        //widget methods
        _test_paint,
        _test_button_down,
        _test_button_up,
        0,
        _test_mouse_move,
        0,
        0,
        0,
        0,
        0,
        awe_widget_get_focus,
        awe_widget_loose_focus,
        awe_geometry_manager_begin_display,
        awe_widget_end_display,
        awe_geometry_manager_insert_widget,
        awe_geometry_manager_remove_widget,
        awe_geometry_manager_geometry_changed,
        awe_geometry_manager_z_order_changed,
        awe_geometry_manager_visible_changed,
        0,
    },

    //geometry manager
    {
        awe_geometry_manager_manage_child,
        _test_set_geometry,
        _test_do_layout,
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
    AWE_WIDGET *root, *child1, *child11, *child12, *child13;

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
    root = awe_create_widget(&test_class, 0,
        AWE_ID_WIDTH, SCREEN_W,
        AWE_ID_HEIGHT, SCREEN_H,
        0);

    //create a child
    child1 = awe_create_widget(&test_class, root,
        AWE_ID_X, 100,
        AWE_ID_Y, 50,
        AWE_ID_WIDTH, 500,
        AWE_ID_HEIGHT, 250,
        AWE_ID_TEXT, "child1",
        AWE_ID_LAYOUT_TYPE, LAYOUT_WRAP,
        0);

    //create grand children
    child11 = awe_create_widget(&test_class, child1,
        AWE_ID_X, 50, 
        AWE_ID_Y, 40,
        AWE_ID_WIDTH, 150,
        AWE_ID_HEIGHT, 140,
        AWE_ID_TEXT, "child11",
        0);
    child12 = awe_create_widget(&test_class, child1,
        AWE_ID_X, 70, 
        AWE_ID_Y, 60,
        AWE_ID_WIDTH, 150,
        AWE_ID_HEIGHT, 140,
        AWE_ID_TEXT, "child12",
        0);
    child13 = awe_create_widget(&test_class, child1,
        AWE_ID_X, 90, 
        AWE_ID_Y, 80,
        AWE_ID_WIDTH, 150,
        AWE_ID_HEIGHT, 140,
        AWE_ID_TEXT, "child13",
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
