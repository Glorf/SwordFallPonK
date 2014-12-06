#ifndef BASE_H

#include "gui.h"
#include "mapa.h"
#include "config.h"
#include "engine.h"

class Base {
public:
    static Base *instance(); //SINGLETON
    Base();
    ~Base();
    GUI *getGuiPtr();
    Map *getMapPtr();
    Config *getConfigPtr();
    int *getFPSRate();
    Engine *getEngine();
    std::pair<int, int>* getCoords();
    ALLEGRO_DISPLAY *getDisplay();

    void setGUI(GUI *gui);
    void setMap(Map *map);
    void setConfig(Config *config);
    void setCoords(std::pair<int, int> coords);
    void setDisplay(ALLEGRO_DISPLAY *disp);
    void setEngine(Engine *engine);
    bool endGame; //BREAKS MAIN LOOP

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
