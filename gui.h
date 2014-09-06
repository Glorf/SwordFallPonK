#ifndef GUI_H
#define GUI_H

#include <string>
#include "allegro5.h"
#include "allegro_image.h"
#include "allegro_primitives.h"
#include "allegro_ttf.h"
#include "allegro_font.h"
#include <vector>
#include <list>
#include <iostream>
#include "mapa.h"
#include "config.h"

struct Region;

class World;

class Config;

class GUI;

class Menu;

enum eLayout{
    X_LAYOUT,
    Y_LAYOUT
};

//DO PRZENIESIENIA
class Slider{
public:
    double min;
    double max;
    double step;
    double value;
};

class Label{
public:
    std::string text;
    std::string font;
};

class Image{
public:
    std::string nameId;
    ALLEGRO_BITMAP *image;
};

class ButtonBase{
public:
    void (GUI::*function)(std::string);
    std::string functionArg;
    bool blocked;

    void activate();
};

class Button : public ButtonBase
{
public:
    std::string label;
    std::string font;
};

class ImageButton : public ButtonBase
{
public:
    ALLEGRO_BITMAP *image;
};

class Layout{
public:
    std::list<Layout> layouts;
    std::vector<Button> buttons;
    std::vector<ImageButton> imageButtons;
    std::vector<Label> labels;
    std::vector<Image> images;
    std::vector<Slider> sliders;
    Layout *parent;
    Menu *parentMenu;
    eLayout layoutType;
    int marginSize;
    float relX;
    float relY;
    float height;
    float width;
    bool highlight;

    Layout* at(int num);
    void fillWithLayouts(int layoutsNumber, int marginSize, eLayout layoutsType);
    void addLabel(std::string text, std::string font="");
    void changeLabel(std::string label);
    void highlightElement();
    void clearHighlight();
    void addButton(std::string label, void (GUI::*function)(std::string), std::string functionArg="", std::string font="");
    void removeButton();
    void addImageButton(std::string imageDir, void (GUI::*function)(std::string), std::string functionArg="");
    void addImage(std::string imageDir, std::string nameId);
    void removeImage();
    void addSlider(double min, double max, double step);
    void connectLayouts(std::vector<int> layoutsToConnect);
    void setButtonBlock(bool block);
};

class Menu{
public:
    std::list<ALLEGRO_BITMAP*> imagesInMenuList;
    bool isCustom;
    float x;
    float y;
    float width;
    float height;
    ALLEGRO_BITMAP *background;
    ALLEGRO_COLOR fontColors;
    eLayout layout;
    std::string nameId;
    std::list<Layout> layouts;
    bool visible;
    bool disabled;

    Layout* at(int num);
    Layout *findLayoutByVector(std::vector<int> v);
    void fillWithLayouts(int layoutsNumber, int marginSize, eLayout layoutsType);
};

//DO PRZENIESIENIA

class GUI
{

public:
    GUI(int displayx, int displayy, ALLEGRO_FONT *czcionkaPtr);
    ~GUI();
    void redrawGui();
    void setWorldXY(int *x, int *y);
    void selectButton(int x, int y);
    bool isGui(int x, int y);
    void callRest(double secTime);

    Menu *addMenu(float mx, float my, float width, float height, eLayout layout, std::string nameId); //PROCENTOWO!
    Menu *addCustomMenu(float mx, float my, float width, float height, eLayout layout, std::string nameId, std::string bgDir,
                       ALLEGRO_COLOR fontColors);
    void removeMenu(std::string nameId);

    Menu *findMenuByName(std::string name);
    void disableAllMenusExcept(std::string menu);
    void enableAllMenus();
    bool mapMovingBlocked();


    //WIDGETS
    void questionPrompt(std::string question, std::vector<std::string> options, void (GUI::*caller)(std::string));
    void removeQuestionPrompt();
    void showNotification(std::string notification);
    void removeNotification(std::string arg);
    void quantityQuestionPrompt(std::string question, double min, double max, double step, void (GUI::*caller)(std::string));
    void removeQuantityQuestionPrompt();
    //END WIDGETS


    //EVENTS
    void sliderLeftButton(std::string arg);
    void sliderRightButton(std::string arg);

    void showPauseMenu(std::string arg);
    void showDiplomaticMenu(std::string arg);
    void showTargetMenu(std::string arg);
    void showMoneyMenu(std::string arg);
    void showNewGameMenu(std::string arg);
    void showOpcjeMenu(std::string arg);

    void resumeToGame(std::string arg);
    void endGame(std::string arg);
    void doPauzaReturn(std::string arg);
    void doMainMenuReturn(std::string arg);
    void loadMap(std::string arg);
    void loadFraction(std::string arg);
    void returnToMainMenu(std::string arg);
    void loadWorld(std::string arg);
    void endTurn(std::string arg);
    void moveArmy(std::string arg);
    void selectMoveArmy(std::string arg);
    void attackRegion(std::string arg);
    void selectAttackRegion(std::string arg);
    void activateRegion(Region *region);
    void toggleMenuVisibility(std::string arg);
    void toggleAntialiasing(std::string arg);
    //END EVENTS

    //MENUS INIT&DESTROYS
    void initDownMenu();
    void refreshDownMenu();
    void destroyDownMenu();

    void initUpMenu();
    void refreshUpMenu();
    void destroyUpMenu();

    void initUpMenuBar();
    void destroyUpMenuBar();

    void initMainMenu();
    void destroyMainMenu();

    void initPauseMenu();
    void destroyPauseMenu();

    void initGameMenu();
    void destroyGameMenu();

    void initOpcjeMenu();
    void destroyOpcjeMenu();

    void initDiplomacyMenu();
    void showDiplomacyMenuOptions(std::string arg);
    void destroyDiplomacyMenu();
    //END MENUS INIT&DESTROYS

    //EVENT MENUS
    void initMapMenu(std::string eventReturn);
    void initFractionMenu(std::string eventReturn);
    void toggleXYInd(std::string eventReturn);
    void changeResolution(std::string eventReturn);
    void toggleFullScreen(std::string eventReturn);
    void changeFPS(std::string eventReturn);
    void setDiplomaticState(std::string eventReturn);
    //EVENT MENUS

    //RUN VARIABLES
    std::string fractionName;
    double quantityReturnValue; //FOR QUESTION PROMPT
    //RUN VARIABLES

    int dispx;
    int dispy;
    int restTime;
    std::list<Menu> menus;
    bool movingBlocked;

    ALLEGRO_FONT *czcionka;
    ALLEGRO_BITMAP *background;
};


#endif
