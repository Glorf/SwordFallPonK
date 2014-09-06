#include "gui.h"
#include <dirent.h>
#include <fstream>
#include <sstream>
#include "base.h"

/*****************************
 *          EVENTS           *
 *****************************/

void GUI::sliderLeftButton(std::string arg)
{
    std::list<Menu>::iterator it;
    for(it=menus.begin();it!=menus.end();it++)
    {
        if(arg!=it->nameId)
            continue;

        std::list<std::list<Layout> * > layTPtr;
        layTPtr.push_back(&it->layouts);
        std::list<std::list<Layout> * > layTPtrNew;

        while(layTPtr.size()!=0)
        {
            std::list<std::list<Layout> * >::iterator it2;
            for(it2=layTPtr.begin();it2!=layTPtr.end();it2++)
            {
                std::list<Layout> *layoutsPtr = *it2;

                std::list<Layout>::iterator it3;
                for(it3 = layoutsPtr->begin(); it3!=layoutsPtr->end(); it3++)
                {
                    Layout *layout = &*it3;

                    if(layout->sliders.size() != 0)
                    {

                        Slider *slider = &layout->sliders.at(0);

                        slider->value -= slider->step;
                        if(slider->value<slider->min)
                            slider->value=slider->min;

                        Layout *labelLayout = layout->parent->at(2);
                        std::string str = std::to_string(slider->value);
                        str.erase(str.find_last_not_of('0') + 1, std::string::npos);
                        str.erase(str.find_last_not_of('.') + 1, std::string::npos);
                        labelLayout->labels.at(0).text = str;

                        quantityReturnValue = slider->value;

                    }

                    else if(it3->layouts.size()!=0)
                    {
                        layTPtrNew.push_back(&it3->layouts);
                    }
                }
            }

            layTPtr = layTPtrNew;
            layTPtrNew.clear();
        }
    }
}

void GUI::sliderRightButton(std::string arg)
{
    std::list<Menu>::iterator it;
    for(it=menus.begin();it!=menus.end();it++)
    {
        if(arg!=it->nameId)
            continue;

        std::list<std::list<Layout> * > layTPtr;
        layTPtr.push_back(&it->layouts);
        std::list<std::list<Layout> * > layTPtrNew;

        while(layTPtr.size()!=0)
        {
            std::list<std::list<Layout> * >::iterator it2;
            for(it2=layTPtr.begin();it2!=layTPtr.end();it2++)
            {
                std::list<Layout> *layoutsPtr = *it2;

                std::list<Layout>::iterator it3;
                for(it3 = layoutsPtr->begin(); it3!=layoutsPtr->end(); it3++)
                {
                    Layout *layout = &*it3;

                    if(layout->sliders.size() != 0)
                    {

                        Slider *slider = &layout->sliders.at(0);

                        slider->value += slider->step;
                        if(slider->value>slider->max)
                            slider->value=slider->max;

                        std::string str = std::to_string(slider->value);
                        str.erase(str.find_last_not_of('0') + 1, std::string::npos);
                        str.erase(str.find_last_not_of('.') + 1, std::string::npos);

                        Layout *labelLayout = layout->parent->at(2);
                        labelLayout->labels.at(0).text = str;

                        quantityReturnValue = slider->value;

                    }

                    else if(it3->layouts.size()!=0)
                    {
                        layTPtrNew.push_back(&it3->layouts);
                    }
                }
            }

            layTPtr = layTPtrNew;
            layTPtrNew.clear();
        }
    }
}

void GUI::doPauzaReturn(std::string arg)
{
    enableAllMenus();
    removeMenu(arg);
    initPauseMenu();
}

void GUI::doMainMenuReturn(std::string arg)
{
    enableAllMenus();
    removeMenu(arg);
    initMainMenu();
}

void GUI::resumeToGame(std::string arg)
{
    enableAllMenus();
    //findMenuByName("dolnyPasek")->findLayoutByVector({1,3})->setButtonBlock(false);
    removeMenu(arg);
}

void GUI::endGame(std::string arg)
{
    Base::instance()->endGame = true;
}

void GUI::loadMap(std::string arg)
{
    std::string dir = "./mapy/";
    dir.append(arg);

    Map *mapPtr = Base::instance()->getMapPtr();

    mapPtr->destroyMap();
    if(!mapPtr->loadMap(dir))
    {
        mapPtr->destroyMap();
        return;
    }

    Menu *gameMenu = findMenuByName("gameMenu");

    gameMenu->findLayoutByVector({2,2})->removeButton();
    gameMenu->findLayoutByVector({2,2})->addButton(mapPtr->getMapName(), &GUI::initMapMenu, "");

    gameMenu->findLayoutByVector({3,2})->removeButton();
    gameMenu->findLayoutByVector({3,2})->addButton("Wybierz", &GUI::initFractionMenu, "");
    gameMenu->findLayoutByVector({10,2})->setButtonBlock(true);
}

void GUI::loadFraction(std::string arg)
{
    fractionName = arg;

    Menu *gameMenu = findMenuByName("gameMenu");
    gameMenu->findLayoutByVector({3,2})->removeButton();
    gameMenu->findLayoutByVector({3,2})->addButton(arg, &GUI::initFractionMenu, "");
    gameMenu->findLayoutByVector({10,2})->setButtonBlock(false);
}

void GUI::loadWorld(std::string arg)
{
    Map *mapPtr = Base::instance()->getMapPtr();
    mapPtr->addPlayer("Gracz1", fractionName, true);
    std::list<Realm>::iterator it;
    int i=0;
    for(it=mapPtr->realms.begin();it!=mapPtr->realms.end(); it++)
    {
        bool used=false;
        std::vector<Player>::iterator it2;
        for(it2=mapPtr->players.begin();it2!=mapPtr->players.end();it2++)
        {
            if(it->name == it2->realm->name)
                used=true;
        }

        if(used==false)
        {
            i+=1;
            std::string name;
            name.append("PC");
            name.append(std::to_string(i));
            mapPtr->addPlayer(name, it->name, false);
        }
    }

    mapPtr->swapTurn();
    mapPtr->centerOnCoords(mapPtr->playerTurn->realm->regions.at(0)->x, mapPtr->playerTurn->realm->regions.at(0)->y);

    destroyGameMenu();

    initUpMenu();
    initUpMenuBar();
    initDownMenu();

    findMenuByName("upMenu")->findLayoutByVector({1,2,4})->highlightElement();
    Base::instance()->getEngine()->setWindowTitle(mapPtr->getMapName());

    findMenuByName("dolnyPasek")->findLayoutByVector({2,3})->changeLabel(mapPtr->playerTurn->realm->name);
}

void GUI::showPauseMenu(std::string arg)
{
    initPauseMenu();
}

void GUI::showDiplomaticMenu(std::string arg)
{
    initDiplomacyMenu();
}

void GUI::showTargetMenu(std::string arg)
{

}

void GUI::showMoneyMenu(std::string arg)
{

}

void GUI::toggleXYInd(std::string arg)
{
    if(arg=="")
    {
        std::vector<std::string> sv;
        sv.push_back("Tak");
        sv.push_back("Nie");
        questionPrompt("Włączyć wskaźnik x/y?", sv, &GUI::toggleXYInd);
    }
    else
    {
        Config *configPtr = Base::instance()->getConfigPtr();
        removeQuestionPrompt();
        std::vector<std::string> v;
        if(arg=="Tak")
        {
            v.push_back("1");
            configPtr->saveValues("xyindicator", v);
        }
        else
        {
            v.push_back("0");
            configPtr->saveValues("xyindicator", v);
        }
    }

    findMenuByName("opcje")->findLayoutByVector({1,2})->removeButton();
    findMenuByName("opcje")->findLayoutByVector({1,2})->addButton(arg, &GUI::toggleXYInd, "");
}

void GUI::toggleFullScreen(std::string eventReturn)
{
    if(eventReturn=="")
    {
        std::vector<std::string> sv;
        sv.push_back("Tak");
        sv.push_back("Nie");
        questionPrompt("Włączyć tryb pełnoekranowy?", sv, &GUI::toggleFullScreen);
    }
    else
    {
        Config *configPtr = Base::instance()->getConfigPtr();
        removeQuestionPrompt();
        std::vector<std::string> v;
        if(eventReturn=="Tak")
        {
            v.push_back("1");
            configPtr->saveValues("fullscreen", v);
            showNotification("Zrestartuj grę aby zastosować");
        }
        else if(eventReturn=="Nie")
        {
            v.push_back("0");
            configPtr->saveValues("fullscreen", v);
            showNotification("Zrestartuj grę aby zastosować");
        }
    }

    findMenuByName("opcje")->findLayoutByVector({2,2})->removeButton();
    findMenuByName("opcje")->findLayoutByVector({2,2})->addButton(eventReturn, &GUI::toggleFullScreen, "");
}

void GUI::toggleAntialiasing(std::string arg)
{
    if(arg=="")
    {
        std::vector<std::string> sv;
        sv.push_back("Tak");
        sv.push_back("Nie");
        questionPrompt("Włączyć antyaliasing?", sv, &GUI::toggleAntialiasing);
    }
    else
    {
        Config *configPtr = Base::instance()->getConfigPtr();
        removeQuestionPrompt();
        std::vector<std::string> v;
        if(arg=="Tak")
        {
            v.push_back("1");
            configPtr->saveValues("antialiasing", v);
        }
        else if(arg=="Nie")
        {
            v.push_back("0");
            configPtr->saveValues("antialiasing", v);
        }

        showNotification("Zrestartuj grę aby zastosować");
    }

    findMenuByName("opcje")->findLayoutByVector({5,2})->removeButton();
    findMenuByName("opcje")->findLayoutByVector({5,2})->addButton(arg, &GUI::toggleAntialiasing, "");
}

void GUI::changeResolution(std::string eventReturn)
{
    if(eventReturn=="")
    {
        std::vector<std::string> sv;
        sv.push_back("640x480");
        sv.push_back("800x600");
        sv.push_back("1024x768");
        sv.push_back("1440x900");
        questionPrompt("Wybierz rozdzielczość:", sv, &GUI::changeResolution);
    }
    else
    {
        Config *configPtr = Base::instance()->getConfigPtr();
        removeQuestionPrompt();
        std::vector<std::string> v;
        std::string r1;
        std::string r2;
        std::string::iterator it;
        bool first=true;
        for(it=eventReturn.begin(); it!=eventReturn.end(); it++)
        {
            if(first)
            {
                if(*it=='x')
                {
                    first=false;
                }
                else
                {
                    r1+=*it;
                }
            }
            else
            {
                r2+=*it;
            }
        }

        v.push_back(r1);
        v.push_back(r2);

        configPtr->saveValues("resolution", v);
        showNotification("Zrestartuj grę aby zastosować");

        findMenuByName("opcje")->findLayoutByVector({3,2})->removeButton();
        findMenuByName("opcje")->findLayoutByVector({3,2})->addButton(eventReturn, &GUI::changeResolution, "");
    }
}

void GUI::changeFPS(std::string eventReturn)
{
    if(eventReturn=="")
    {
        quantityQuestionPrompt("Prędkość odświeżania: ", 1, 200, 1, &GUI::changeFPS);
    }
    else
    {
        std::string value = std::to_string((int)quantityReturnValue);
        removeQuantityQuestionPrompt();
        std::vector<std::string> v;
        v.push_back(value);
        Base::instance()->getConfigPtr()->saveValues("fpsrate", v);
        showNotification("Zrestartuj grę aby zastosować");

        findMenuByName("opcje")->findLayoutByVector({4,2})->removeButton();
        findMenuByName("opcje")->findLayoutByVector({4,2})->addButton(value+" FPS", &GUI::changeFPS, "");
    }
}

void GUI::endTurn(std::string arg)
{
    while(true)
    {
        Map *mapPtr = Base::instance()->getMapPtr();
        mapPtr->swapTurn();
        findMenuByName("dolnyPasek")->visible=false;
        refreshUpMenu();
        findMenuByName("upMenu")->findLayoutByVector({1,2,4})->highlightElement();
        findMenuByName("upMenuBar")->findLayoutByVector({1,3})->changeLabel(mapPtr->playerTurn->realm->name);

        if(mapPtr->playerTurn->isHuman)
        {
            callRest(0.5); //Computer's last move

            refreshDownMenu();
            findMenuByName("dolnyPasek")->visible = true;
            findMenuByName("dolnyPasek")->findLayoutByVector({2,3})->changeLabel(mapPtr->playerTurn->realm->name);
            std::string text1;
            text1.append("Tura: ");
            text1.append(std::to_string(mapPtr->turnNumber));
            findMenuByName("dolnyPasek")->findLayoutByVector({2,4,1})->changeLabel(text1);
            std::string text2;
            text2.append("Rok: ");
            text2.append(std::to_string(2000+mapPtr->turnNumber-1));
            text2.append(" ne");
            findMenuByName("dolnyPasek")->findLayoutByVector({2,4,2})->changeLabel(text2);

            //Set XY position to player's first region
            Region *reg = mapPtr->playerTurn->realm->regions.at(0);
            mapPtr->centerOnCoords(reg->x, reg->y);

            movingBlocked=false;
            break;
        }
        else
        {
            movingBlocked=true;
            callRest(0.5); //RUCH KOMPUTERA
        }
    }
}

void GUI::moveArmy(std::string arg)
{
    int q=quantityReturnValue;
    removeQuantityQuestionPrompt();
    Base::instance()->getMapPtr()->move(q);
}

void GUI::selectMoveArmy(std::string arg)
{
    Base::instance()->getMapPtr()->moveSelection();
}

void GUI::attackRegion(std::string arg)
{
    Base::instance()->getMapPtr()->attack(1); //DOROBIC!!!
}

void GUI::selectAttackRegion(std::string arg)
{
    Base::instance()->getMapPtr()->attackSelection();
}

void GUI::activateRegion(Region *region)
{
    Map *mapPtr = Base::instance()->getMapPtr();

    std::string notification;
    notification.append("Czy chcesz ");

    if(mapPtr->movType==ATTACK)
    {
        if(region->owner->name == mapPtr->playerTurn->realm->name)
            return;

        if(mapPtr->selectedRegion->owner->name != mapPtr->playerTurn->realm->name)
            return;

        std::list<DiplomaticState>::iterator it;
        for(it = mapPtr->playerTurn->realm->diplomacy.begin();it != mapPtr->playerTurn->realm->diplomacy.end();it++)
        {
            if(it->realm->name == region->owner->name)
            {
                if(it->state == ALLIANCE)
                {
                    showNotification("Najpierw rozwiąż sojusz i wypowiedz wojnę!");
                    return;
                }
                else if(it->state == TRADE_AGREEMENT || it->state == NEUTRAL)
                {
                    showNotification("Najpierw wypowiedz wojnę temu państwu!");
                    return;
                }
            }
        }

        notification.append("zaatakować kraj ");
        notification.append(region->owner->name);
        notification.append(" region ");
        notification.append(region->regionname);
        notification.append(" z ");
        notification.append(mapPtr->selectedRegion->regionname);
        notification.append("?");
    }
    else if(mapPtr->movType==MOVE)
    {
        if(mapPtr->selectedRegion->owner->name != region->owner->name)
            return;

        if(mapPtr->selectedRegion->owner->name != mapPtr->playerTurn->realm->name)
            return;

        notification.append("przenieść armię z ");
        notification.append(mapPtr->selectedRegion->regionname);
        notification.append(" do ");
        notification.append(region->regionname);
        notification.append("?");
    }

    quantityQuestionPrompt(notification, 0, mapPtr->selectedRegion->garnizon, 1, &GUI::moveArmy);
}

void GUI::showNewGameMenu(std::string arg)
{
    destroyMainMenu();
    initGameMenu();
}

void GUI::returnToMainMenu(std::string arg)
{
    enableAllMenus();
    Map *mapPtr = Base::instance()->getMapPtr();
    initMainMenu();
    movingBlocked = false;
    mapPtr->destroyMap();
    destroyPauseMenu();
    destroyDownMenu();
    destroyUpMenu();
    destroyUpMenuBar();
    Base::instance()->getEngine()->setWindowTitle("SwordFallPonK");
}

void GUI::toggleMenuVisibility(std::string arg)
{
    Menu *menu = findMenuByName(arg);
    menu->visible = !menu->visible;
}

/*****************************
 *          MENUS            *
 *****************************/

//downMenu
void GUI::initDownMenu()
{
    Map *mapPtr = Base::instance()->getMapPtr();
    Menu *downMenu = addMenu(0, 90, 100, 10,Y_LAYOUT, "dolnyPasek");

    downMenu->fillWithLayouts(2, 0, X_LAYOUT);

    downMenu->findLayoutByVector({1})->fillWithLayouts(5, 0, X_LAYOUT);
    downMenu->findLayoutByVector({2})->fillWithLayouts(5, 0, X_LAYOUT);

    downMenu->findLayoutByVector({1,5})->fillWithLayouts(5, 0, X_LAYOUT);

    downMenu->findLayoutByVector({1,5})->connectLayouts({2,3});
    downMenu->findLayoutByVector({1,5})->connectLayouts({3,4});

    downMenu->findLayoutByVector({1,5,2})->addLabel(std::to_string(mapPtr->playerTurn->realm->money));
    downMenu->findLayoutByVector({1,5,1})->addImage("./res/coin.png", "coin");
    downMenu->findLayoutByVector({1,5,3})->
            addLabel("(+"+std::to_string(mapPtr->calculateIncome(mapPtr->playerTurn->realm))+")");

    downMenu->findLayoutByVector({1,4})->addButton("Dyplomacja", &GUI::showDiplomaticMenu, "");
    downMenu->findLayoutByVector({1,2})->addButton("Cele", &GUI::showTargetMenu, "");
    downMenu->findLayoutByVector({1,3})->addButton("Menu", &GUI::showPauseMenu, "");
    downMenu->findLayoutByVector({1,1})->addButton("Przemieść", &GUI::selectMoveArmy, "");
    downMenu->findLayoutByVector({2,1})->addButton("Atakuj", &GUI::selectAttackRegion, "");
    downMenu->findLayoutByVector({2,5})->addButton("Koniec tury", &GUI::endTurn, "");
    downMenu->findLayoutByVector({2,3})->addLabel("Kraj");
    downMenu->findLayoutByVector({2,2})->addButton("Finanse", &GUI::showMoneyMenu, "");

    downMenu->findLayoutByVector({2,4})->fillWithLayouts(2, 0, X_LAYOUT);
    downMenu->findLayoutByVector({2,4,1})->addLabel("Tura: 1");
    downMenu->findLayoutByVector({2,4,2})->addLabel("Rok: 2000 ne");
}

void GUI::refreshDownMenu()
{
    Map *mapPtr = Base::instance()->getMapPtr();
    findMenuByName("dolnyPasek")->findLayoutByVector({1,5,2})->
            changeLabel(std::to_string(mapPtr->playerTurn->realm->money));
    findMenuByName("dolnyPasek")->
            findLayoutByVector({1,5,3})->
            changeLabel("(+"+std::to_string(mapPtr->calculateIncome(mapPtr->playerTurn->realm))+")");
}

void GUI::destroyDownMenu()
{
    removeMenu("dolnyPasek");
}

//mainMenu
void GUI::initMainMenu()
{
    addCustomMenu(0, 0, 100, 90, Y_LAYOUT, "mainMenuBackground", "./res/mainmenu-next.jpg", al_map_rgb(255,255,255));

    Menu *mainMenu = addMenu(0, 90, 100, 10, X_LAYOUT, "mainMenu");
    mainMenu->fillWithLayouts(5, 0, X_LAYOUT);

    mainMenu->findLayoutByVector({1})->addButton("Nowa gra", &GUI::showNewGameMenu, "");
    mainMenu->findLayoutByVector({2})->addButton("Opcje", &GUI::showOpcjeMenu, "");
    mainMenu->findLayoutByVector({5})->addButton("Wyjdz", &GUI::endGame, "");
}

void GUI::destroyMainMenu()
{
    removeMenu("mainMenu");
    removeMenu("mainMenuBackground");
}

//pauseMenu
void GUI::initPauseMenu()
{
    Menu *pauseMenu = addMenu(40,6.5,19.5,65, Y_LAYOUT, "pauza");
    disableAllMenusExcept("pauza");
    pauseMenu->fillWithLayouts(10, 5, X_LAYOUT);

    pauseMenu->findLayoutByVector({1})->addButton("Przywroc", &GUI::resumeToGame, "pauza");
    pauseMenu->findLayoutByVector({2})->addButton("Opcje", &GUI::showOpcjeMenu, "");

    if(!Base::instance()->getMapPtr()->getMapName().empty())
    {
        pauseMenu->findLayoutByVector({9})->addButton("Wyjdz do menu", &GUI::returnToMainMenu, "");
    }

    pauseMenu->findLayoutByVector({10})->addButton("Wyjdz z gry", &GUI::endGame, "");
    //findMenuByName("dolnyPasek")->findLayoutByVector({1,3})->setButtonBlock(true);
}

void GUI::destroyPauseMenu()
{
    enableAllMenus();
    removeMenu("pauza");
}

//mapLoadMenu
void GUI::initMapMenu(std::string eventReturn)
{   
    int i=1;

    DIR *dp;
    struct dirent *dirp;
    if((dp  = opendir("mapy")) == NULL) {
        showNotification("Error opening maps folder!");
        return;
    }

    std::string mapToLoad;
    std::vector<std::string> nazwyMap;

    while ((dirp = readdir(dp)) != NULL)
    {
        std::string dirway = "./mapy/";
        dirway.append(dirp->d_name);


        std::ifstream fileStream;

        if (std::string(dirp->d_name).length()<=4)
            continue;

        fileStream.open(dirway);

        if(!fileStream.good())
            continue;

        while(!fileStream.eof())
        {
            std::string line;
            std::vector<std::string> translatedLine;

            //line decryption
            std::getline(fileStream, line);

            if(line.size()==0)
                break;

            std::string::iterator it;
            std::string temporaryCommandContainer;
            bool nameEnclosure = false;
            for(it=line.begin();it!=line.end();it++)
            {
                if(*it == '#')
                    break;
                else if(*it == '\"')
                {
                    nameEnclosure = !nameEnclosure;
                }
                else if(*it == ' ' && !nameEnclosure)
                {
                    translatedLine.push_back(temporaryCommandContainer);
                    temporaryCommandContainer = "";
                }
                else
                {
                    temporaryCommandContainer += *it;
                }
            }

            if(temporaryCommandContainer.size()>0)
            {
                translatedLine.push_back(temporaryCommandContainer);
                temporaryCommandContainer = "";
            }

            else
                continue;

            //command decryption
            if(translatedLine[0]=="nazwa")
            {
                if(eventReturn==translatedLine[1])
                {
                    mapToLoad = dirp->d_name;
                }

                nazwyMap.push_back(translatedLine[1]);
            }

            fileStream.close();
        }

        i+=1;
    }

    if(closedir(dp)<0)
        showNotification("Error closing map folder!");

    if(eventReturn=="")
    {
        questionPrompt("Wybierz mapę: ", nazwyMap, &GUI::initMapMenu);
    }

    else
    {
        removeQuestionPrompt();
        loadMap(mapToLoad);
    }

}

//fractionMenu

void GUI::initFractionMenu(std::string eventReturn)
{
    Map *mapPtr = Base::instance()->getMapPtr();

    if(eventReturn=="")
    {
        std::vector<std::string> frakcje;
        std::list<Realm>::iterator it;
        for(it=mapPtr->realms.begin();it!=mapPtr->realms.end();it++)
        {
            frakcje.push_back(it->name);
        }

        questionPrompt("Wybierz frakcję:", frakcje, &GUI::initFractionMenu);
    }
    else
    {
        removeQuestionPrompt();
        loadFraction(eventReturn);
    }
}

//gameMenu
void GUI::initGameMenu()
{
    Menu *gameMenu = addMenu(0, 0, 100, 100, Y_LAYOUT, "gameMenu");
    gameMenu->fillWithLayouts(10, 0, X_LAYOUT);
    gameMenu->findLayoutByVector({1})->addLabel("Wybierz opcje gry:");
    gameMenu->findLayoutByVector({2})->fillWithLayouts(2, 0, X_LAYOUT);

    gameMenu->findLayoutByVector({2,1})->addLabel("Mapa:");
    gameMenu->findLayoutByVector({2,2})->addButton("Wybierz", &GUI::initMapMenu, "");

    gameMenu->findLayoutByVector({3})->fillWithLayouts(2, 0, X_LAYOUT);
    gameMenu->findLayoutByVector({3,1})->addLabel("Frakcja:");

    gameMenu->findLayoutByVector({10})->fillWithLayouts(2, 0, X_LAYOUT);
    gameMenu->findLayoutByVector({10,1})->addButton("Powrót", &GUI::doMainMenuReturn, "gameMenu");
    gameMenu->findLayoutByVector({10,2})->addButton("Zagraj", &GUI::loadWorld, "");
    gameMenu->findLayoutByVector({10,2})->setButtonBlock(true);
}

void GUI::destroyGameMenu()
{
    removeMenu("gameMenu");
}

//upMenu

void GUI::initUpMenu()
{
    Map *mapPtr = Base::instance()->getMapPtr();

    Menu *upMenu = addMenu(35,2, 30, 10, Y_LAYOUT, "upMenu");
    upMenu->fillWithLayouts(1, 0, Y_LAYOUT);
    upMenu->findLayoutByVector({1})->fillWithLayouts(4, 0, X_LAYOUT);

    upMenu->findLayoutByVector({1})->connectLayouts({2,3});

    upMenu->findLayoutByVector({1,2})->fillWithLayouts(10, 2, X_LAYOUT);

    Realm *realm;

    upMenu->findLayoutByVector({1,2})->connectLayouts({2, 3});

    if(mapPtr->playerTurn->numberId == 1)
        realm = mapPtr->players.at(mapPtr->players.size()-1).realm;
    else
        realm = mapPtr->players.at(mapPtr->playerTurn->numberId-2).realm;

    upMenu->findLayoutByVector({1,2,2})->addImage(realm->signDir, realm->name);


    upMenu->findLayoutByVector({1,2})->connectLayouts({4, 5});

    realm = mapPtr->playerTurn->realm;

    upMenu->findLayoutByVector({1,2,4})->addImage(realm->signDir, realm->name);


    upMenu->findLayoutByVector({1,2})->connectLayouts({6, 7});

    if(mapPtr->playerTurn->numberId==mapPtr->players.size())
        realm = mapPtr->players.at(0).realm;
    else
        realm = mapPtr->players.at(mapPtr->playerTurn->numberId).realm;

    upMenu->findLayoutByVector({1,2,6})->addImage(realm->signDir, realm->name);
}

void GUI::refreshUpMenu()
{
    Map *mapPtr = Base::instance()->getMapPtr();

    Menu *upMenu = findMenuByName("upMenu");

    Realm *realm;

    if(mapPtr->playerTurn->numberId == 1)
        realm = mapPtr->players.at(mapPtr->players.size()-1).realm;
    else
        realm = mapPtr->players.at(mapPtr->playerTurn->numberId-2).realm;

    upMenu->findLayoutByVector({1,2,2})->removeImage();
    upMenu->findLayoutByVector({1,2,2})->addImage(realm->signDir, realm->name);

    realm = mapPtr->players.at(mapPtr->playerTurn->numberId-1).realm;

    upMenu->findLayoutByVector({1,2,4})->removeImage();
    upMenu->findLayoutByVector({1,2,4})->addImage(realm->signDir, realm->name);

    if(mapPtr->playerTurn->numberId==mapPtr->players.size())
        realm = mapPtr->players.at(0).realm;
    else
        realm = mapPtr->players.at(mapPtr->playerTurn->numberId).realm;

    upMenu->findLayoutByVector({1,2,6})->removeImage();
    upMenu->findLayoutByVector({1,2,6})->addImage(realm->signDir, realm->name);
}

void GUI::destroyUpMenu()
{
    removeMenu("upMenu");
}

//upMenuBar

void GUI::initUpMenuBar()
{
    Menu *upMenuBar = addMenu(35,0, 30, 2,X_LAYOUT, "upMenuBar");
    upMenuBar->fillWithLayouts(1, 0, X_LAYOUT);
    upMenuBar->findLayoutByVector({1})->fillWithLayouts(10, 0, X_LAYOUT);
    upMenuBar->findLayoutByVector({1})->connectLayouts({3,4,5,6});
    upMenuBar->findLayoutByVector({1,3})->addLabel(Base::instance()->getMapPtr()->playerTurn->realm->name);
    upMenuBar->findLayoutByVector({1,7})->addButton("X", &GUI::toggleMenuVisibility, "upMenu");
}

void GUI::destroyUpMenuBar()
{
    removeMenu("upMenuBar");
}

//opcjeMenu
void GUI::showOpcjeMenu(std::string arg)
{
    removeMenu("pauza");
    initOpcjeMenu();
}

void GUI::initOpcjeMenu()
{
    bool fullscreen;
    if(findMenuByName("mainMenu")!=nullptr)
        fullscreen = true;
    else
        fullscreen = false;

    Menu *opcjeMenu;
    if(fullscreen)
    {
        removeMenu("mainMenu");
        opcjeMenu = addMenu(0,0,100,100,Y_LAYOUT, "opcje");
    }
    else
        opcjeMenu = addMenu(37,6.5,30,65,Y_LAYOUT, "opcje");

    disableAllMenusExcept("opcje");
    opcjeMenu->fillWithLayouts(10, 0, X_LAYOUT);

    if(fullscreen)
        opcjeMenu->findLayoutByVector({10})->addButton("Powrot", &GUI::doMainMenuReturn,"opcje");
    else
        opcjeMenu->findLayoutByVector({10})->addButton("Powrot", &GUI::doPauzaReturn,"opcje");

    opcjeMenu->findLayoutByVector({1})->fillWithLayouts(2, 0, X_LAYOUT);
    opcjeMenu->findLayoutByVector({1,1})->addLabel("Wskaźnik X Y: ");

    std::string ind;
    if(std::stoi(Base::instance()->getConfigPtr()->getConfigValues("xyindicator").at(0))==1)
        ind="Tak";
    else
        ind="Nie";
    opcjeMenu->findLayoutByVector({1,2})->addButton(ind, &GUI::toggleXYInd, "");

    opcjeMenu->findLayoutByVector({2})->fillWithLayouts(2, 0, X_LAYOUT);
    opcjeMenu->findLayoutByVector({2,1})->addLabel("Pełny ekran:");
    if(std::stoi(Base::instance()->getConfigPtr()->getConfigValues("fullscreen").at(0))==1)
        ind="Tak";
    else
        ind="Nie";
    opcjeMenu->findLayoutByVector({2,2})->addButton(ind, &GUI::toggleFullScreen, "");

    opcjeMenu->findLayoutByVector({3})->fillWithLayouts(2, 0, X_LAYOUT);
    opcjeMenu->findLayoutByVector({3,1})->addLabel("Rozdzielczość:");
    ind=(Base::instance()->getConfigPtr()->getConfigValues("resolution").at(0) + "x" +
        Base::instance()->getConfigPtr()->getConfigValues("resolution").at(1));
    opcjeMenu->findLayoutByVector({3,2})->addButton(ind, &GUI::changeResolution, "");

    opcjeMenu->findLayoutByVector({4})->fillWithLayouts(2, 0, X_LAYOUT);
    opcjeMenu->findLayoutByVector({4,1})->addLabel("Prędkość odświeżania: ");
    opcjeMenu->findLayoutByVector({4,2})->addButton(Base::instance()->getConfigPtr()->
                                                   getConfigValues("fpsrate").at(0) + " FPS", &GUI::changeFPS,"");

    opcjeMenu->findLayoutByVector({5})->fillWithLayouts(2, 0, X_LAYOUT);
    opcjeMenu->findLayoutByVector({5,1})->addLabel("Antyaliasing: ");
    if(std::stoi(Base::instance()->getConfigPtr()->getConfigValues("antialiasing").at(0))==1)
        ind="Tak";
    else
        ind="Nie";
    opcjeMenu->findLayoutByVector({5,2})->addButton(ind, &GUI::toggleAntialiasing,"");
}

void GUI::initDiplomacyMenu()
{
    Menu *diplomacyMenu = addMenu(15, 15, 50, 50, Y_LAYOUT, "diplomacyMenu");
    disableAllMenusExcept("diplomacyMenu");
    diplomacyMenu->fillWithLayouts(1, 0 ,Y_LAYOUT);
    diplomacyMenu->findLayoutByVector({1})->fillWithLayouts(10, 0, X_LAYOUT);
    diplomacyMenu->findLayoutByVector({1})->connectLayouts({1,2,3,4,5,6,7,8,9});
    diplomacyMenu->findLayoutByVector({1,2})->addButton("Wyjdź", &GUI::resumeToGame, "diplomacyMenu");
    diplomacyMenu->findLayoutByVector({1,1})->fillWithLayouts(3, 0, Y_LAYOUT);
    diplomacyMenu->findLayoutByVector({1,1})->connectLayouts({2,3});
    diplomacyMenu->findLayoutByVector({1,1,1})->fillWithLayouts(Base::instance()->getMapPtr()->realms.size()-1, 0, X_LAYOUT);

    Map *mapPtr = Base::instance()->getMapPtr();

    int i=1;
    std::list<Realm>::iterator it;
    for(it=mapPtr->realms.begin();it!=mapPtr->realms.end();it++)
    {
        if(it->name == mapPtr->playerTurn->realm->name)
            continue;
        diplomacyMenu->findLayoutByVector({1,1,1,i})->addButton(it->name, &GUI::showDiplomacyMenuOptions, it->name);
        i+=1;
    }

    diplomacyMenu->findLayoutByVector({1,1,2})->fillWithLayouts(10, 0, X_LAYOUT);
    diplomacyMenu->findLayoutByVector({1,1,2})->connectLayouts({6,7,8,9,10});
    diplomacyMenu->findLayoutByVector({1,1,2,1})->addLabel("");
}

void GUI::showDiplomacyMenuOptions(std::string arg)
{
    Map *mapPtr = Base::instance()->getMapPtr();
    Menu *diplomacyMenu = findMenuByName("diplomacyMenu");
    diplomacyMenu->findLayoutByVector({1,1,2,1})->changeLabel("Stosunki dyplomatyczne z " + arg + ":");
    diplomacyMenu->findLayoutByVector({1,1,2,2})->removeButton();
    diplomacyMenu->findLayoutByVector({1,1,2,2})->clearHighlight();
    diplomacyMenu->findLayoutByVector({1,1,2,2})->addButton("Sojusz", &GUI::setDiplomaticState, "s"+arg);
    diplomacyMenu->findLayoutByVector({1,1,2,3})->removeButton();
    diplomacyMenu->findLayoutByVector({1,1,2,3})->clearHighlight();
    diplomacyMenu->findLayoutByVector({1,1,2,3})->addButton("Umowa handlowa", &GUI::setDiplomaticState, "t"+arg);
    diplomacyMenu->findLayoutByVector({1,1,2,4})->removeButton();
    diplomacyMenu->findLayoutByVector({1,1,2,4})->clearHighlight();
    diplomacyMenu->findLayoutByVector({1,1,2,4})->addButton("Neutralny", &GUI::setDiplomaticState, "n"+arg);
    diplomacyMenu->findLayoutByVector({1,1,2,5})->removeButton();
    diplomacyMenu->findLayoutByVector({1,1,2,5})->clearHighlight();
    diplomacyMenu->findLayoutByVector({1,1,2,5})->addButton("Wrogi", &GUI::setDiplomaticState, "w"+arg);

    eDiplomaticState state;
    std::list<DiplomaticState>::iterator it;
    for(it=mapPtr->playerTurn->realm->diplomacy.begin(); it!=mapPtr->playerTurn->realm->diplomacy.end(); it++)
    {
        if(it->realm->name == arg)
        {
            state = it->state;
            break;
        }
    }

    switch(state)
    {
    case ALLIANCE:
        diplomacyMenu->findLayoutByVector({1,1,2,2})->highlightElement();
        break;
    case TRADE_AGREEMENT:
        diplomacyMenu->findLayoutByVector({1,1,2,3})->highlightElement();
        break;
    case NEUTRAL:
        diplomacyMenu->findLayoutByVector({1,1,2,4})->highlightElement();
        break;
    case WAR:
        diplomacyMenu->findLayoutByVector({1,1,2,5})->highlightElement();
        break;
    }
}

void GUI::setDiplomaticState(std::string eventReturn)
{
    Map *mapPtr = Base::instance()->getMapPtr();
    std::string realmName = eventReturn.substr(1, eventReturn.size()-1);
    if(eventReturn.at(0)=='s')
        mapPtr->changeDiplomaticState(ALLIANCE, mapPtr->playerTurn->realm->name, realmName);
    else if(eventReturn.at(0)=='t')
        mapPtr->changeDiplomaticState(TRADE_AGREEMENT, mapPtr->playerTurn->realm->name, realmName);
    else if(eventReturn.at(0)=='n')
        mapPtr->changeDiplomaticState(NEUTRAL, mapPtr->playerTurn->realm->name, realmName);
    else if(eventReturn.at(0)=='w')
        mapPtr->changeDiplomaticState(WAR, mapPtr->playerTurn->realm->name, realmName);

    showDiplomacyMenuOptions(realmName);
}

void GUI::destroyDiplomacyMenu()
{
    enableAllMenus();
    removeMenu("diplomacyMenu");
}
