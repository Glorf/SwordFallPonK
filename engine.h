#ifndef ENGINE_H
#define ENGINE_H

#include "allegro5.h"
#include "allegro_image.h"
#include "allegro_primitives.h"
#include "allegro_ttf.h"
#include "allegro_font.h"
#include <string>
#include <list>

struct sCoords
{
    double x;
    ///< Part X of coordinates struct

    double y;
    ///< Part y of coordinates struct
};

struct sWH
{
    double width;
    ///< The width of screen/object/anything

    double height;
    ///< The height of screen/object/anything
};

struct sFont
{
    std::string name;
    ///< Font name

    std::string dir;
    ///< Relative path to font file

    ALLEGRO_FONT *ptr;
    ///< Pointer to real font instance
};

/// \brief Hides some of the bare allegro procedures, makes using its functions easier.
/// Also stores pointers to some of allegro classes instances
class Engine
{
public:
    Engine();
    ~Engine();
    void initDisplay(bool fullscreen, bool antialiasing, sCoords size, sCoords position);
    void initEventSystem();
    ALLEGRO_DISPLAY *getDisplay();
    sCoords getWindowSize();
    ALLEGRO_KEYBOARD_STATE *getKeyboard();
    ALLEGRO_MOUSE_STATE *getMouse();
    void updateInput();
    void setWindowTitle(std::string title);

    void addFont(std::string name, std::string dir, int factor=1);
    void setDefaultFont(std::string name);
    sWH getTextWH(std::string text, std::string font="");
    void writeText(std::string text, sCoords coord, ALLEGRO_COLOR color = al_map_rgb(0,0,0), std::string font="");

    bool isFullscreen();
private:
    ALLEGRO_DISPLAY *mDisplay;
    bool fullscreen;
    ALLEGRO_KEYBOARD_STATE keyboard;
    ALLEGRO_MOUSE_STATE mouse;
    std::list<sFont> fonts;
    sFont *defaultFont;

    sFont *findFont(std::string name);
};

#endif // ENGINE_H
