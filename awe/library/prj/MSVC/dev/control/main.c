#include "control.h"
#include "event.h"
#include "gdi.h"
#include "dataobjects.h"
#include <string.h>


/***************************************************************************** 
    DIALOG
 *****************************************************************************/


//dialog paint
void dialog_paint(AWE_WIDGET *wgt, AWE_CANVAS *canvas, const AWE_RECT *clip)
{
    solid_mode();
    awe_fill_rect_s(canvas, 0, 0, wgt->width, wgt->height, makecol(128, 128, 128));
}


//dialog vtable
AWE_WIDGET_VTABLE dialog_vtable = {
    {
        awe_widget_get_interface,
        awe_widget_properties_changed,
        awe_widget_clone
    },
    dialog_paint,
    awe_widget_button_down,
    awe_widget_button_up,
    awe_widget_mouse_enter,
    awe_widget_mouse_move,
    awe_widget_mouse_leave,
    awe_widget_mouse_wheel,
    awe_dialog_key_down,
    awe_widget_key_up,
    0,
    0,
    0,
    awe_widget_begin_display,
    awe_widget_end_display,
    awe_widget_insert_widget,
    awe_widget_remove_widget,
    0,
    0,
    0,
    0
};


//dialog class
AWE_CLASS dialog_class = {
    "Dialog",
    AWE_ID_AWE,
    &awe_widget_class,
    sizeof(AWE_WIDGET),
    awe_widget_properties,
    0,
    &dialog_vtable.object,
    0,
    0
};


/***************************************************************************** 
    PANEL
 *****************************************************************************/


//panel paint
void panel_paint(AWE_WIDGET *wgt, AWE_CANVAS *canvas, const AWE_RECT *clip)
{
    solid_mode();
    awe_fill_rect_s(canvas, 0, 0, wgt->width, wgt->height, makecol(212, 212, 212));
    awe_draw_rect_s(canvas, 0, 0, wgt->width, wgt->height, makecol(32, 32, 32));
}


//panel vtable
AWE_WIDGET_VTABLE panel_vtable = {
    {
        awe_widget_get_interface,
        awe_widget_properties_changed,
        awe_widget_clone
    },
    panel_paint,
    awe_widget_button_down,
    awe_widget_button_up,
    awe_widget_mouse_enter,
    awe_widget_mouse_move,
    awe_widget_mouse_leave,
    awe_widget_mouse_wheel,
    awe_widget_key_down,
    awe_widget_key_up,
    0,
    0,
    0,
    awe_widget_begin_display,
    awe_widget_end_display,
    awe_widget_insert_widget,
    awe_widget_remove_widget,
    0,
    0,
    0,
    0
};


//panel class
AWE_CLASS panel_class = {
    "Panel",
    AWE_ID_AWE,
    &awe_widget_class,
    sizeof(AWE_WIDGET),
    awe_widget_properties,
    0,
    &panel_vtable.object,
    0,
    0
};


/***************************************************************************** 
    BUTTON
 *****************************************************************************/


//button
typedef struct BUTTON {
    AWE_WIDGET widget;
    char *text;
} BUTTON;


//button vtable
typedef struct BUTTON_VTABLE {
    AWE_WIDGET_VTABLE widget;
    AWE_CONTROL_VTABLE control;
} BUTTON_VTABLE;


//constructor
void button_constructor(AWE_OBJECT *obj)
{
    BUTTON *test = (BUTTON *)obj;
    test->text = ustrdup(empty_string);
}


//destructor
void button_destructor(AWE_OBJECT *obj)
{
    BUTTON *test = (BUTTON *)obj;
    free(test->text);
}


//gets the text
void button_get_text(AWE_OBJECT *obj, void *data)
{
    BUTTON *test = (BUTTON *)obj;
    *(const char **)data = test->text;
}


//sets the text
void button_set_text(AWE_OBJECT *obj, void *data)
{
    const char *new_text = *(const char **)data;
    BUTTON *test = (BUTTON *)obj;
    free(test->text);
    test->text = ustrdup(new_text ? new_text : empty_string);
    awe_set_widget_dirty(&test->widget);
}


void *button_get_interface(AWE_OBJECT *obj, const char *name, const char *pnamespace)
{
    if (!strcmp(pnamespace, AWE_ID_AWE)) {
        if (!strcmp(name, "Button")) {
            return (BUTTON_VTABLE *)obj->pclass->vtable;
        }
        if (!strcmp(name, AWE_ID_CONTROL)) {
            return &((BUTTON_VTABLE *)obj->pclass->vtable)->control;
        }
    }
    return awe_widget_get_interface(obj, name, pnamespace);
}


//button paint
void button_paint(AWE_WIDGET *wgt, AWE_CANVAS *canvas, const AWE_RECT *clip)
{
    BUTTON *btn = (BUTTON *)wgt;
    int tx = (wgt->width - gui_strlen(btn->text))/2;
    int ty = (wgt->height - text_height(font))/2;

    solid_mode();
    awe_fill_rect_s(canvas, 0, 0, wgt->width, wgt->height, makecol(255, 255, 255));
    awe_draw_rect_s(canvas, 0, 0, wgt->width, wgt->height, makecol(0, 0, 0));
    awe_draw_gui_text(canvas, font, btn->text, tx, ty, makecol(0, 0, 0), -1);
    if (awe_get_focus_widget() == wgt) {
        awe_draw_rect_pattern_s(canvas, 4, 4, wgt->width - 9, wgt->height - 9, makecol(0, 0, 0), AWE_PATTERN_DOT_DOT);
    }
}


void button_button_down(AWE_WIDGET *wgt, const AWE_EVENT *event)
{
    if (!awe_set_focus_widget(wgt)) return;
    awe_enter_event_mode(awe_grab_event_proc, wgt);
}


void button_button_up(AWE_WIDGET *wgt, const AWE_EVENT *event)
{
    awe_leave_event_mode();
}


int button_key_press(AWE_WIDGET *wgt, const AWE_EVENT *event)
{
    BUTTON *btn = (BUTTON *)wgt;
    const char *t = ustrchr(btn->text, '&');

    if (!t) return 0;
    t += ucwidth('&');
    if (utolower(ugetc(t)) != utolower(event->key.key)) return 0;
    if (!awe_set_focus_widget(wgt)) return 0;
    return 1;
}


//button properties
AWE_CLASS_PROPERTY button_properties[] = {
    {AWE_ID_TEXT, "const char *", sizeof(const char *), button_get_text, button_set_text, 0},
    {0}
};


//button vtable
BUTTON_VTABLE button_vtable = {
    //widget
    {
        //object
        {
            button_get_interface,
            awe_widget_properties_changed,
            awe_widget_clone
        },
        button_paint,
        button_button_down,
        button_button_up,
        awe_widget_mouse_enter,
        awe_widget_mouse_move,
        awe_widget_mouse_leave,
        awe_widget_mouse_wheel,
        awe_widget_key_down,
        awe_widget_key_up,
        0,
        awe_widget_get_focus,
        awe_widget_lose_focus,
        awe_widget_begin_display,
        awe_widget_end_display,
        awe_widget_insert_widget,
        awe_widget_remove_widget,
        0,
        0,
        0,
        0
    }
    ,
    //control
    {
        button_key_press
    }
};


//button class
AWE_CLASS button_class = {
    "Button",
    AWE_ID_AWE,
    &awe_widget_class,
    sizeof(BUTTON),
    button_properties,
    0,
    &button_vtable.widget.object,
    0,
    0
};


/***************************************************************************** 
    MAIN
 *****************************************************************************/


AWE_WIDGET *root, *panel1, *panel2, *button11, *button12, *button21, *button22;


int main()
{
    allegro_init();
    install_keyboard();
    install_timer();
    install_mouse();

    awe_install_input();

    set_color_depth(32);
    if (set_gfx_mode(GFX_AUTODETECT_WINDOWED, 640, 480, 0, 0) < 0) return 0;
    show_mouse(screen);

    awe_set_gui_screen(screen);

    root = awe_create_widget(&dialog_class, 0,
        AWE_ID_WIDTH, SCREEN_W,
        AWE_ID_HEIGHT, SCREEN_H,
        0);

    panel1 = awe_create_widget(&panel_class, root,
        AWE_ID_X, 100, 
        AWE_ID_Y, 50,
        AWE_ID_WIDTH, 300, 
        AWE_ID_HEIGHT, 250,
        0);

    button11 = awe_create_widget(&button_class, panel1,
        AWE_ID_X, 50, 
        AWE_ID_Y, 40,
        AWE_ID_WIDTH, 100, 
        AWE_ID_HEIGHT, 50,
        AWE_ID_TEXT, "Button &1",
        0);

    button12 = awe_create_widget(&button_class, panel1,
        AWE_ID_X, 90, 
        AWE_ID_Y, 120,
        AWE_ID_WIDTH, 100, 
        AWE_ID_HEIGHT, 50,
        AWE_ID_TEXT, "Button &2",
        0);

    panel2 = awe_create_widget(&panel_class, root,
        AWE_ID_X, 250, 
        AWE_ID_Y, 200,
        AWE_ID_WIDTH, 300, 
        AWE_ID_HEIGHT, 250,
        0);

    button21 = awe_create_widget(&button_class, panel2,
        AWE_ID_X, 50, 
        AWE_ID_Y, 40,
        AWE_ID_WIDTH, 100, 
        AWE_ID_HEIGHT, 50,
        AWE_ID_TEXT, "Button &3",
        0);

    button22 = awe_create_widget(&button_class, panel2,
        AWE_ID_X, 90, 
        AWE_ID_Y, 120,
        AWE_ID_WIDTH, 100, 
        AWE_ID_HEIGHT, 50,
        AWE_ID_TEXT, "Button &4",
        0);

    awe_set_root_widget(root);
    
    while (1) {
        if (keypressed() && (readkey() >> 8) == KEY_ESC) break;
        yield_timeslice();
        awe_do_events();
        awe_update_gui();
    }

    allegro_exit();
    return 0;
}
END_OF_MAIN();
