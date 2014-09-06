#include "engine.h"
#include <iostream>

Engine::Engine():
    defaultFont(nullptr)
{
    al_init();
    al_init_image_addon();
    al_init_primitives_addon();
    al_init_ttf_addon();
}

Engine::~Engine()
{
    al_destroy_display(mDisplay);
    al_uninstall_mouse();
    al_uninstall_keyboard();
}

void Engine::initDisplay(bool fullscreen, bool antialiasing, sCoords size, sCoords position)
{
    if(fullscreen)
    {
        al_set_new_display_flags(ALLEGRO_FULLSCREEN);
    }

    al_set_new_display_option(ALLEGRO_VSYNC, 1 ,ALLEGRO_SUGGEST);

    if(antialiasing)
    {
        al_set_new_display_option(ALLEGRO_SAMPLE_BUFFERS, 1, ALLEGRO_SUGGEST);
        al_set_new_display_option(ALLEGRO_SAMPLES, 8, ALLEGRO_SUGGEST);
    }

    mDisplay = al_create_display(size.x, size.y);
    al_set_window_position(mDisplay, position.x, position.y);
}

ALLEGRO_DISPLAY *Engine::getDisplay()
{
    return mDisplay;
}

ALLEGRO_MOUSE_STATE *Engine::getMouse()
{
    return &mouse;
}

ALLEGRO_KEYBOARD_STATE *Engine::getKeyboard()
{
    return &keyboard;
}

sCoords Engine::getWindowSize()
{
    sCoords size;
    size.x = al_get_display_width(mDisplay);
    size.y = al_get_display_height(mDisplay);
    return size;
}

void Engine::initEventSystem()
{
    al_install_keyboard();
    al_install_mouse();
}

void Engine::updateInput()
{
    al_get_keyboard_state(&keyboard);
    al_get_mouse_state(&mouse);
}

void Engine::setWindowTitle(std::string title)
{
    al_set_window_title(mDisplay, title.c_str());
}

void Engine::addFont(std::string name, std::string dir, int factor)
{
    ALLEGRO_FONT *ptr = al_load_ttf_font(dir.c_str(), 0.013*getWindowSize().x*factor, 0);

    sFont font;
    font.dir = dir;
    font.name = name;
    font.ptr = ptr;

    fonts.push_back(font);
}

void Engine::setDefaultFont(std::string name)
{
    std::list<sFont>::iterator it;
    for(it=fonts.begin(); it!=fonts.end(); it++)
    {
        if(it->name == name)
            defaultFont = &*it;
        return;
    }
}

sWH Engine::getTextWH(std::string text, std::string font)
{
    sFont *fontPtr = findFont(font);

    sWH dimensions;
    dimensions.width = al_get_text_width(fontPtr->ptr, text.c_str());
    dimensions.height = al_get_font_line_height(fontPtr->ptr);

    return dimensions;
}

void Engine::writeText(std::string text, sCoords coord, ALLEGRO_COLOR color, std::string font)
{
    sFont *fontPtr = findFont(font);

    al_draw_text(fontPtr->ptr, color, coord.x, coord.y, 0, text.c_str());
}

sFont *Engine::findFont(std::string name)
{
    if(name=="")
        return defaultFont;
    else
    {
        std::list<sFont>::iterator it;
        for(it=fonts.begin(); it!=fonts.end(); it++)
        {
            if(it->name == name)
                return &*it;
        }
    }

    return nullptr;
}
