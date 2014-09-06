#include "base.h"

Base *Base::mBaseInstance = 0;

Base::Base():
    endGame(false)
{
    mBaseInstance = this;
}

Base::~Base()
{
    mBaseInstance = 0;
}

GUI* Base::getGuiPtr()
{
    return mGuiPtr;
}

Map* Base::getMapPtr()
{
    return mMapPtr;
}

Config* Base::getConfigPtr()
{
    return mConfigPtr;
}

Engine* Base::getEngine()
{
    return mEngine;
}

Base* Base::instance()
{
    return mBaseInstance;
}

std::pair<int, int> *Base::getCoords()
{
    return &coords;
}

void Base::setMap(Map *map)
{
    mMapPtr = map;
}

void Base::setGUI(GUI *gui)
{
    mGuiPtr = gui;
}

void Base::setEngine(Engine *engine)
{
    mEngine = engine;
}

void Base::setCoords(std::pair<int, int> outcoords)
{
    coords = outcoords;
}

void Base::setConfig(Config *config)
{
    mConfigPtr = config;
}
