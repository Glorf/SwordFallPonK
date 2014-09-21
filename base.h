#ifndef BASE_H

#include "gui.h"
#include "mapa.h"
#include "config.h"
#include "engine.h"

/// \brief Singleton class that helps classes to communicate with each other.
class Base {
public:
    static Base *instance(); //SINGLETON
    Base();
    ~Base();
    GUI *getGuiPtr();
    ///< Returns a pointer to the GUI instance.

    Map *getMapPtr();
    ///< @return a pointer to the Map instance.
    
    Config *getConfigPtr();
    ///< @return a pointer to the Config instance.

    int *getFPSRate();
    ///< @return a pointer to the int that stores current FPS rate 

    Engine *getEngine();
    ///< @return a pointer to the Engine instance.

    std::pair<int, int>* getCoords();
    ///< @return a pointer to the pair, that stores current user-look coords.

    ALLEGRO_DISPLAY *getDisplay();
    ///< @return a pointer to the bare allegro display

    void setGUI(GUI *gui);
    void setMap(Map *map);
    void setConfig(Config *config);
    void setCoords(std::pair<int, int> coords);
    void setDisplay(ALLEGRO_DISPLAY *disp);
    void setEngine(Engine *engine);


    bool endGame;
    ///< When true, will break the main loop in the end of this iteration.

private:
    static Base *mBaseInstance;
    GUI *mGuiPtr;
    Map *mMapPtr;
    Config *mConfigPtr;
    Engine *mEngine;
    std::pair<int, int> coords;
    std::map<std::string, ALLEGRO_FONT> fonts;
    ALLEGRO_FONT *defaultFont;
};

#endif // BASE_H
