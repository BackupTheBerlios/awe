#include <awe.h>

BITMAP *buffer;
volatile int my_timer;

void timer_proc(void){
    my_timer++;
}END_OF_FUNCTION(timer_proc);

void _drs_proc(AL_CONST AWE_RECT *r)
{
    blit(buffer, screen, r->left, r->top, r->left, r->top, AWE_RECT_WIDTH(*r), AWE_RECT_HEIGHT(*r));
}

int main()
{
    AWE_RECT rect;
    AWE_SKIN *skn;
    BITMAP *user_cursor;
    AWE_CURSOR *hand;
    AWE_CANVAS canvas;
    AWE_TEXTURE *texture1, *texture2, *texture3, *texture4, *texture5;
    AWE_WIDGET *root, *button, *checkbox, *radio1, *radio2, *slider1, *slider2;
    RGB *rgb;
    int test_int, flag;
    FONT *font1, *font2;
    allegro_init();
    install_keyboard();
    install_timer();
    install_mouse();
    awe_install_input();
    awe_install_mouse();
    awe_install_font();
    install_int_ex(timer_proc, BPS_TO_TIMER(60));
    awe_set_font_alias(TRUE);
    set_color_depth(32);
    if (set_gfx_mode(GFX_AUTODETECT_WINDOWED, 800, 600, 0, 0) < 0) return -1;
    buffer = create_bitmap(SCREEN_W, SCREEN_H);
    if (!buffer){
        allegro_message("Unable to create buffer");
        exit(1);
    }
    awe_set_gui_screen(buffer);
    awe_set_gui_update_mode(AWE_GUI_UPDATE_CHANGES, _drs_proc);
    text_mode(-1);
    user_cursor = load_bitmap("./image/hand.bmp", NULL);
    if (!user_cursor){
        allegro_message("Unable to load hand.bmp");
        exit(1);
    }
    skn = awe_load_skin("../themes/test.skn");
    if (!skn){
        allegro_message("Unable to load skin");
        exit(1);
    }
    hand = awe_create_mouse_cursor(user_cursor, 1, 1, AWE_MOUSE_SHOW_SHADOW, AWE_MOUSE_SINGLE_SHADOW, 1, 0, AWE_MOUSE_LOOP_ONCE, 128);
    awe_show_mouse(buffer);
    texture1 = awe_get_skin_texture(skn, "AWE.Pushbutton.Background.Enabled");
    if(!texture1){
        allegro_message("Unable to find texture");
        exit(1);
    }
    texture2 = awe_get_skin_texture(skn, "AWE.Pushbutton.Background.Disabled");
    if(!texture2){
        allegro_message("Unable to find linked texture");
        exit(1);
    }
    texture3 = awe_get_skin_texture(skn, "AWE.Pushbutton.Background.Pressed");
    if(!texture3){
        allegro_message("Unable to find linked texture");
        exit(1);
    }
    texture4 = awe_get_skin_texture(skn, "AWE.Pushbutton.Background.Highlighted");
    if(!texture4){
        allegro_message("Unable to find linked texture");
        exit(1);
    }
    texture5 = awe_get_skin_texture(skn, "AWE.Pushbutton.Background.Focused");
    if(!texture5){
        allegro_message("Unable to find linked texture");
        exit(1);
    }
    rgb = awe_get_skin_color(skn, "AWE.Pushbutton.Font.Color.Enabled");
    test_int = awe_get_skin_int(skn, "AWE.Test.Integer", -1);
    font1 = awe_get_skin_font(skn, "AWE.Pushbutton.Font.Selected");
    font2 = awe_get_skin_font(skn, "AWE.Font.Bitmap");

    rect.left = 0;
    rect.top = 0;
    rect.right = SCREEN_W;
    rect.bottom = SCREEN_H;
    awe_set_canvas(&canvas, buffer, &rect);

    clear_to_color(buffer, makecol(200, 200, 200));

    textout(buffer, font1, "Hello World!", 10, 12, makecol(255, 255, 255));
    textout(buffer, font2, "Hello World!", 10, 32, makecol(rgb->r, rgb->g, rgb->b));
    textprintf(buffer, font1, 10, 57, makecol(255, 255, 255), "%d", test_int);

    awe_draw_texture_hor(&canvas, texture1, 100, 100, 160, 140);
    awe_draw_texture_hor(&canvas, texture2, 100, 150, 160, 190);
    awe_draw_texture_hor(&canvas, texture3, 100, 200, 160, 240);
    awe_draw_texture_hor(&canvas, texture4, 100, 250, 160, 290);
    awe_draw_texture_hor(&canvas, texture5, 100, 300, 160, 340);
    
    root = awe_create_widget(&awe_widget_class, 0,
        AWE_ID_WIDTH, SCREEN_W,
        AWE_ID_HEIGHT, SCREEN_H,
        AWE_ID_OPAQUE, 0,
        0);
            
    button = awe_create_widget(&awe_push_button_class, root,
        AWE_ID_X, 300,
        AWE_ID_Y, 50,
        AWE_ID_WIDTH, 160,
        AWE_ID_HEIGHT, 40,
        AWE_ID_FONT, font2,
        AWE_ID_TEXT, "Push Button",
        AWE_ID_TEXTURE_ENABLED, texture1,
        AWE_ID_TEXTURE_DISABLED, texture2,
        AWE_ID_TEXTURE_PRESSED, texture3,
        AWE_ID_TEXTURE_HIGHLIGHTED, texture4,
        AWE_ID_TEXTURE_FOCUSED, texture5, 
        0);

    checkbox = awe_create_widget(&awe_checkbox_class, root,
        AWE_ID_X, 300,
        AWE_ID_Y, 100,
        AWE_ID_HEIGHT, 13,
        AWE_ID_WIDTH, 160,
        AWE_ID_TEXT, "Checkbox",
        0);

    radio1 = awe_create_widget(&awe_radio_class, root,
        AWE_ID_X, 300,
        AWE_ID_Y, 120,
        AWE_ID_HEIGHT, 13,
        AWE_ID_WIDTH, 160,
        AWE_ID_TEXT, "Radio1",
        AWE_ID_GROUP, 1,
        AWE_ID_TOGGLED, TRUE,
        0);
            
    radio2 = awe_create_widget(&awe_radio_class, root,
        AWE_ID_X, 300,
        AWE_ID_Y, 140,
        AWE_ID_HEIGHT, 13,
        AWE_ID_WIDTH, 160,
        AWE_ID_TEXT, "Radio2",
        AWE_ID_GROUP, 1,
        0);
    slider1 = awe_create_widget(&awe_slider_class, root,
        AWE_ID_X, 300,
        AWE_ID_Y, 160,
        AWE_ID_HEIGHT, 16,
        AWE_ID_WIDTH, 160,
        0);
    slider2 = awe_create_widget(&awe_slider_class, root,
        AWE_ID_X, 300,
        AWE_ID_Y, 190,
        AWE_ID_WIDTH, 16,
        AWE_ID_HEIGHT, 100,
        AWE_ID_ORIENTATION, AWE_SLIDER_VERTICAL,
        AWE_ID_HANDLE_TYPE, AWE_SLIDER_HANDLE_RIGHT,
        AWE_ID_STEP, 10,
        0);
            
    awe_set_root_widget(root);

    do{
        AWE_RECT rect;
        if(!flag && mouse_x >= 10 && mouse_x < text_length(font1, "Hello World!") + 10 && mouse_y >= 12 && mouse_y < text_height(font1) + 12){
            awe_scare_mouse();
            rectfill(buffer, 10, 12, text_length(font1, "Hello World!") + 10, text_height(font1) + 12,  makecol(200, 200, 200));
            textout(buffer, font1, "Hello World!", 10, 12, makecol(255, 0, 0));
            awe_unscare_mouse();
            awe_set_mouse_cursor(hand);
            AWE_RECT_SET_S(rect, 10, 12, text_length(font1, "Hello World!") + 1, text_height(font1) + 1);
            _drs_proc(&rect);
            flag = 1;
        }
        else if(flag && !(mouse_x >= 10 && mouse_x < text_length(font1, "Hello World!") + 10 && mouse_y >= 12 && mouse_y < text_height(font1) + 12)){
            awe_scare_mouse();
            rectfill(buffer, 10, 12, text_length(font1, "Hello World!") + 10, text_height(font1) + 12,  makecol(200, 200, 200));
            textout(buffer, font1, "Hello World!", 10, 12, makecol(255, 255, 255));
            awe_unscare_mouse();
            awe_set_mouse_cursor(awe_get_system_cursor(AWE_MOUSE_ARROW));
            AWE_RECT_SET_S(rect, 10, 12, text_length(font1, "Hello World!") + 1, text_height(font1) + 1);
            _drs_proc(&rect);
            flag = 0;
        }
        awe_do_events();
        awe_update_gui();
        awe_update_mouse();
    }while(!key[KEY_ESC]);
    awe_destroy_mouse_cursor(hand);
    awe_show_mouse(NULL);
    awe_font_exit();
    awe_mouse_exit();
    awe_unload_skin(skn);
    destroy_bitmap(user_cursor);
    destroy_bitmap(buffer);
    return 0;
}
END_OF_MAIN();
