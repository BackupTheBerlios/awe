#include "awe.h"

void button1_activated(AWE_OBJECT *obj)
{
    TRACE("Button 1 activated\n");
}


void button1_pressed(AWE_OBJECT *obj)
{
    TRACE("Button 1 pressed\n");
}


void button1_held(AWE_OBJECT *obj)
{
    TRACE("Button 1 held down\n");
}


void button2_pressed(AWE_OBJECT *obj)
{
    TRACE("Button 2 pressed\n");
}


void button2_held(AWE_OBJECT *obj)
{
    TRACE("Button 2 held down\n");
}


void button2_released(AWE_OBJECT *obj)
{
    TRACE("Button 2 released\n");
}


RGB mk_rgb(int r, int g, int b)
{
    RGB s = {r, g, b};
    return s;
}


int main()
{
    AWE_WIDGET *root, *button1, *button2, *label;
    allegro_init();
    install_keyboard();
    install_timer();
    install_mouse();

    awe_install_input();

    set_color_depth(32);
    if (set_gfx_mode(GFX_AUTODETECT_WINDOWED, 640, 480, 0, 0) < 0) return 0;
    show_mouse(screen);

    awe_set_gui_screen(screen);

    root = awe_create_widget(&awe_widget_class, 0,
        AWE_ID_WIDTH, SCREEN_W,
        AWE_ID_HEIGHT, SCREEN_H,
        0);

    button1 = awe_create_widget(&awe_push_button_class, root,
        AWE_ID_TEXT, "Button",
        AWE_ID_X, 100,
        AWE_ID_Y, 50,
        AWE_ID_WIDTH, 250,
        AWE_ID_HEIGHT, 100,
        AWE_ID_FACE_COLOR_TOP_LEFT_ENABLED, mk_rgb(255, 255, 255),
        AWE_ID_FACE_COLOR_TOP_RIGHT_ENABLED, mk_rgb(255, 0, 255),
        AWE_ID_FACE_COLOR_BOTTOM_LEFT_ENABLED, mk_rgb(0, 255, 255),
        AWE_ID_FACE_COLOR_BOTTOM_RIGHT_ENABLED, mk_rgb(0, 0, 255),
	AWE_ID_FACE_COLOR_TOP_LEFT_FOCUSED, mk_rgb(255, 255, 255),
        AWE_ID_FACE_COLOR_TOP_RIGHT_FOCUSED, mk_rgb(255, 0, 255),
        AWE_ID_FACE_COLOR_BOTTOM_LEFT_FOCUSED, mk_rgb(0, 255, 255),
        AWE_ID_FACE_COLOR_BOTTOM_RIGHT_FOCUSED, mk_rgb(0, 0, 255),
	AWE_ID_FACE_COLOR_TOP_LEFT_HIGHLIGHTED, mk_rgb(255, 255, 255),
        AWE_ID_FACE_COLOR_TOP_RIGHT_HIGHLIGHTED, mk_rgb(255, 0, 255),
        AWE_ID_FACE_COLOR_BOTTOM_LEFT_HIGHLIGHTED, mk_rgb(0, 255, 255),
        AWE_ID_FACE_COLOR_BOTTOM_RIGHT_HIGHLIGHTED, mk_rgb(0, 0, 255),
	AWE_ID_FACE_COLOR_TOP_LEFT_PRESSED, mk_rgb(255, 255, 255),
        AWE_ID_FACE_COLOR_TOP_RIGHT_PRESSED, mk_rgb(255, 0, 255),
        AWE_ID_FACE_COLOR_BOTTOM_LEFT_PRESSED, mk_rgb(0, 255, 255),
        AWE_ID_FACE_COLOR_BOTTOM_RIGHT_PRESSED, mk_rgb(0, 0, 255),
        0);

    button2 = awe_create_widget(&awe_toggle_button_class, root,
        AWE_ID_TEXT, "Toggle Button",
        AWE_ID_X, 200,
        AWE_ID_Y, 300,
        AWE_ID_WIDTH, 250,
        AWE_ID_HEIGHT, 100,
        0);

    label = awe_create_widget(&awe_label_class, root,
        AWE_ID_TEXT, "Label",
        AWE_ID_X, 100,
        AWE_ID_Y, 200,
        AWE_ID_WIDTH, 40,
        AWE_ID_HEIGHT, 12,
        0);

    awe_add_widget_event(button1,
                         AWE_ID_PUSH_BUTTON_ACTIVATED, 
                         0,
                         button1_activated);
    awe_add_widget_event(button1,
                         AWE_ID_PUSH_BUTTON_PRESSED, 
                         0,
                         button1_pressed);
    awe_add_widget_event(button1,
                         AWE_ID_PUSH_BUTTON_HELD_DOWN, 
                         0,
                         button1_held);

    awe_add_widget_event(button2,
                         AWE_ID_TOGGLE_BUTTON_PRESSED, 
                         0,
                         button2_pressed);
    awe_add_widget_event(button2,
                         AWE_ID_TOGGLE_BUTTON_HELD_DOWN, 
                         0,
                         button2_held);
    awe_add_widget_event(button2,
                         AWE_ID_TOGGLE_BUTTON_RELEASED,
                         0,
                         button2_released);

    awe_set_root_widget(root);
    awe_update_gui();

    while (1) {
        if (keypressed () && (readkey() >> 8) == KEY_ESC) break;
        awe_do_events();
        awe_update_gui();
    }

    allegro_exit();
    return 0;
}
END_OF_MAIN();
