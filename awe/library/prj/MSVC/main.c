#include "widget.h"


int main()
{
    AWE_WIDGET *root;

    allegro_init();
    install_keyboard();
    install_timer();
    install_mouse();

    set_color_depth(32);
    if (set_gfx_mode(GFX_AUTODETECT_WINDOWED, 640, 480, 0, 0) < 0) return 0;
    show_mouse(screen);

    awe_set_gui_screen(screen);

    root = awe_create_widget(&awe_widget_class, 0,
        AWE_ID_WIDTH, SCREEN_W,
        AWE_ID_HEIGHT, SCREEN_H,
        0);

    awe_set_root_widget(root);
    awe_update_gui();

    readkey();
    allegro_exit();
    return 0;
}
END_OF_MAIN();
