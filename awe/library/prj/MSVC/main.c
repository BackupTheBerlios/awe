#include "widget.h"
#include "button.h"


int main()
{
    AWE_WIDGET *root, *button;

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

    button = awe_create_widget(&awe_button_class, root,
        AWE_ID_TEXT, "SomeButton",
        AWE_ID_X, 100,
        AWE_ID_Y, 50,
        AWE_ID_WIDTH, 250,
        AWE_ID_HEIGHT, 100,
        0);

    awe_set_root_widget(root);
    awe_update_gui();

    while (1) {
        if (keypressed () && (readkey() >> 8) == KEY_ESC) break;
        awe_do_events();
        awe_update_gui();
        textprintf(screen, font, 0, SCREEN_H - 8, 0xFFFFFF, "%i", button->has_mouse);
    }

    //readkey();
    allegro_exit();
    return 0;
}
END_OF_MAIN();
