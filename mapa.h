#ifndef MAPA_H
#define MAPA_H

#include <iostream>
#include "allegro5.h"
#include "allegro_image.h"
#include "allegro_primitives.h"
#include "allegro_ttf.h"
#include <vector>
#include <fstream>
#include <string>
#include <ios>
#include "gui.h"

class GUI;

struct Realm;

enum eMovementType
{
    ATTACK,
    MOVE,
    NOTHING
};

enum eDiplomaticState
{
    ALLIANCE,
    WAR,
    TRADE_AGREEMENT,
    NEUTRAL
};

class Region
{
public:
    int x;
    int y;
    int w;
    int h;
    int garnizon;
    int incomeAmount; //TAX AMOUNT WITH MEDIUM TAXES AND HAPPY POPULATION
    std::string regionname;
    Realm *owner;
    std::vector<Region*> regionsConnected;

    void changeRegionOwnership(std::string ownername);
    void changeRegionGarnizon(int garnizon);
};

struct Player
{
    int numberId;
    std::string name;
    Realm *realm;
    bool isHuman;
};

struct DiplomaticState
{
    Realm *realm;
    eDiplomaticState state;
};

struct Realm
{
    std::string name;
    std::string signDir;
    ALLEGRO_COLOR color;
    int money;
    std::vector<Region*> regions;

    std::list<DiplomaticState> diplomacy;
};

class Map {
public:
    Map();
    ~Map();
    bool loadMap(std::string mapDirectory);
    void destroyMap();
    void redrawRegions();
    bool checkRegionDrawInfo(int x, int y);
    void fight(std::string regionAttacking, std::string regionDefending);
    bool selectRegion(int x, int y); // LPM Click
    void activateRegion(int x, int y); //PPM Click
    void setMapName(std::string name);
    void centerOnCoords(int x, int y) const;
    std::string getMapName();
    ALLEGRO_BITMAP *getMapaPtr();
    std::pair<int*,int*> getMapaWH();
    void setGUIPtr(GUI *GUIPtr);

    void swapTurn();
    void moveSelection();
    void move(int quantity);
    void attackSelection();
    void attack(int quantity);
    Region* findRegionByName(std::string regionName);
    bool changeDiplomaticState(eDiplomaticState state, std::string first, std::string second);

    void addPlayer(std::string name, std::string realmName, bool isHuman);
    int calculateIncome(Realm *realm);

    std::string mapaDirectory;
    Player *playerTurn;
    std::list<Realm> realms;
    std::vector<Player> players;
    eMovementType movType;
    int turnNumber;
    Region *selectedRegion;
    Region *activeRegion;

private:
    bool addRegion(int x, int y, std::string regionname, std::string ownername, int garnizon, int income);
    bool addRealm(std::string name, ALLEGRO_COLOR color, std::string signDir);
    bool addRegionsConnector(std::string firstRegion, std::string secondRegion);

    void fillDiplomaticStates();
    void fillRealmsRegions();

    std::string nazwaMapy;
    std::list<Region> regions;
    int playerNextNumber;

    ALLEGRO_BITMAP *mapaPtr;
    int mapaWidth;
    int mapaHeight;
};

#endif
