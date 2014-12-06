#include "mapa.h"
#include "base.h"
#include "engine.h"

Map::Map():
    mapaPtr(al_create_bitmap(0,0)),
    mapaWidth(0),
    mapaHeight(0),
    playerNextNumber(1),
    playerTurn(NULL),
    turnNumber(1),
    movType(NOTHING),
    selectedRegion(NULL)
{
    al_clear_to_color(al_map_rgb(0,0,0));
}
Map::~Map(){}

void Map::swapTurn()
{
    movType=NOTHING;
    selectedRegion = NULL;

    if(playerTurn==NULL || playerTurn==nullptr)
    {
        std::vector<Player>::iterator it;
        for(it = players.begin(); it!=players.end(); it++)
        {
            if(it->numberId == 1)
                playerTurn = &*it;
        }
    }
    else
    {
        playerTurn->realm->money += calculateIncome(playerTurn->realm);

        int num=playerTurn->numberId+1;
        std::vector<Player>::iterator it;
        for(it = players.begin(); it!=players.end(); it++)
        {
            if(it->numberId == num)
                playerTurn = &*it;
        }

        if(playerTurn->numberId>=players.size() && playerTurn->numberId!=num)
        {
            turnNumber+=1;

            std::vector<Player>::iterator it2;
            for(it2 = players.begin(); it2!=players.end(); it2++)
            {
                if(it2->numberId == 1)
                {
                    playerTurn = &*it2;
                }
            }
        }
    }
}

bool Map::loadMap(std::string mapDirectory)
{
    GUI *guiPtr = Base::instance()->getGuiPtr();

    std::ifstream fileStream;
    fileStream.open(mapDirectory);
    if(!fileStream.good())
        return false;

    int linei = 0;
    bool error = false;
    while(!fileStream.eof())
    {
        linei+=1;
        std::string line;
        std::vector<std::string> translatedLine;

        //line decryption
        std::getline(fileStream, line);

        if(line.size()==0)
            continue;

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

        if(translatedLine.size()==0)
            continue;

        //command decryption
        if(translatedLine[0]=="nazwa")
        {
            if(translatedLine.size()!=2)
                error=true;
            else
                setMapName(translatedLine[1]);
        }
        else if(translatedLine[0]=="mapa")
        {
            if(translatedLine.size()!=2)
                error=true;
            else
            {
                al_destroy_bitmap(mapaPtr);
                mapaPtr = al_load_bitmap(translatedLine[1].c_str());
                al_clear_to_color(al_map_rgb(0, 0, 255));
                mapaWidth = al_get_bitmap_width(mapaPtr);
                mapaHeight = al_get_bitmap_height(mapaPtr);
            }
        }
        else if(translatedLine[0]=="kraj")
        {
            if(translatedLine.size()!=6)
                error=true;
            else
                addRealm(translatedLine[1],al_map_rgb(std::stoi(translatedLine[2]),
                        std::stoi(translatedLine[3]),std::stoi(translatedLine[4])), translatedLine[5]);
        }
        else if(translatedLine[0]=="koniec")
        {
            if(translatedLine.size()!=2)
                error=true;
            else if(translatedLine[1]=="krajow")
                fillDiplomaticStates();
            else
                error=true;
        }
        else if(translatedLine[0]=="region")
        {
            if(translatedLine.size()!=7)
                error=true;
            else if(!addRegion(std::stoi(translatedLine[1]),std::stoi(translatedLine[2]),
                    translatedLine[3],translatedLine[4],std::stoi(translatedLine[5]), std::stoi(translatedLine[6])))
                error=true;
        }
        else if(translatedLine[0]=="droga")
        {
            if(translatedLine.size()!=3)
                error=true;
            else if(!addRegionsConnector(translatedLine[1], translatedLine[2]))
                error=true;
        }
        else if(translatedLine[0]=="wojna")
        {
            if(translatedLine.size()!=3)
                error=true;
            else if(!changeDiplomaticState(WAR, translatedLine[1], translatedLine[2]))
                error=true;
        }
        else if(translatedLine[0]=="handel")
        {
            if(translatedLine.size()!=3)
                error=true;
            else if(!changeDiplomaticState(TRADE_AGREEMENT, translatedLine[1], translatedLine[2]))
                error=true;
        }
        else if(translatedLine[0]=="sojusz")
        {
            if(translatedLine.size()!=3)
                error=true;
            else if(!changeDiplomaticState(ALLIANCE, translatedLine[1], translatedLine[2]))
                error=true;
        }

        if(error)
        {
            guiPtr->showNotification("Błąd w mapie w linii: " + std::to_string(linei));
            return false;
        }
    }

    fillRealmsRegions();
    return true;
}

void Map::destroyMap()
{
    al_clear_to_color(al_map_rgb(0,0,0));
    al_destroy_bitmap(mapaPtr);
    mapaPtr = al_create_bitmap(0,0);
    realms.clear();
    regions.clear();
    players.clear();
    playerTurn = NULL;
    playerNextNumber = 1;
    selectedRegion = NULL;
    nazwaMapy.clear();
    mapaHeight = 0;
    mapaWidth = 0;
    movType = NOTHING;
    Base::instance()->getCoords()->first = 0;
    Base::instance()->getCoords()->second = 0;
}

bool Map::addRegion(int x, int y, std::string regionname, std::string ownername, int garnizon, int income)
{
    Region region;
    region.x = x;
    region.y = y;
    region.w = 10;
    region.h = 10;
    region.garnizon = garnizon;
    region.regionname = regionname;
    region.incomeAmount = income;

    bool got = false;
    std::list<Realm>::iterator it;
    for(it=realms.begin();it!=realms.end();it++)
    {
        if(it->name == ownername)
        {
            got=true;
            region.owner = &*it;
            break;
        }
    }

    if(!got)
        return false;

    regions.push_back(region);

    return true;
}

void Map::fillDiplomaticStates()
{
    std::list<Realm>::iterator it;
    for(it=realms.begin();it!=realms.end();it++)
    {
        std::list<Realm>::iterator it2;
        for(it2=realms.begin();it2!=realms.end();it2++)
        {
            if(it->name==it2->name)
                continue;

            DiplomaticState state;
            state.state = NEUTRAL;
            state.realm = &*it2;

            it->diplomacy.push_back(state);
        }

    }
}

bool Map::changeDiplomaticState(eDiplomaticState state, std::string first, std::string second)
{
    std::vector<Realm*> realmPtrs;

    std::list<Realm>::iterator it;
    for(it=realms.begin();it!=realms.end();it++)
    {
        if((it->name == first) || (it->name == second))
        {
            realmPtrs.push_back(&*it);
        }
    }

    if(realmPtrs.size()!=2)
        return false;

    bool c1 = false;
    bool c2 = false;

    std::list<DiplomaticState>::iterator it1;
    for(it1=realmPtrs.at(0)->diplomacy.begin();it1!=realmPtrs.at(0)->diplomacy.end();it1++)
    {
        if(it1->realm->name == realmPtrs.at(1)->name)
        {
            it1->state = state;
            c1 = true;
        }
    }

    std::list<DiplomaticState>::iterator it2;
    for(it2=realmPtrs.at(1)->diplomacy.begin();it2!=realmPtrs.at(1)->diplomacy.end();it2++)
    {
        if(it2->realm->name == realmPtrs.at(0)->name)
        {
            it2->state = state;
            c2=true;
        }
    }

    if(!c1 || !c2)
        return false;

    return true;
}

void Map::redrawRegions()
{
    if(playerTurn == NULL)
        return;

    int screenW = Base::instance()->getEngine()->getWindowSize().x;
    int screenH = Base::instance()->getEngine()->getWindowSize().y;

    int worldx = Base::instance()->getCoords()->first;
    int worldy = Base::instance()->getCoords()->second;

    al_clear_to_color(al_map_rgb(0, 0, 255));
    al_draw_bitmap_region(mapaPtr,-worldx,-worldy,screenW, screenH,0,0,0);

    std::list<Region>::iterator it0;
    for(it0=regions.begin(); it0!=regions.end(); it0++)
    {
        std::vector<Region*>::iterator it2;
        for(it2=it0->regionsConnected.begin(); it2!=it0->regionsConnected.end(); it2++)
        {
            ALLEGRO_COLOR color = al_map_rgb(0,0,0);
            Region *region2 = *it2;

            if(selectedRegion!=NULL)
            {

                if(movType==MOVE &&
                ((it0->regionname==selectedRegion->regionname ||
                    region2->regionname==selectedRegion->regionname) && //BREAKS SOMETIMES
                    (it0->owner->name == region2->owner->name)) &&
                    (it0->owner->name == playerTurn->realm->name))
                        color = al_map_rgb(0,255,0);
                else if(movType==ATTACK&&
                    ((it0->regionname==selectedRegion->regionname ||
                    region2->regionname==selectedRegion->regionname) &&
                    (it0->owner->name != region2->owner->name)) &&
                    (it0->owner->name != playerTurn->realm->name))
                        color = al_map_rgb(255,0,0);
            }

            al_draw_line(it0->x+worldx+5, it0->y+worldy+5,(*it2)->x+worldx+5, (*it2)->y+worldy+5,color,2);
        }
    }


    std::list<Region>::iterator it;
    for(it=regions.begin(); it!=regions.end(); it++)
    {
        std::list<DiplomaticState>::iterator stateIt;
        for(stateIt=playerTurn->realm->diplomacy.begin(); stateIt!=playerTurn->realm->diplomacy.end(); stateIt++)
        {
            if(playerTurn->realm->name == it->owner->name)
            {
                al_draw_rectangle(it->x+worldx-1, it->y+worldy-1,
                                  it->x+it->w+worldx+1, it->y+it->h+worldy+1, al_map_rgb(255, 255, 0), 3);
                break;
            }

            else if(stateIt->realm->name == it->owner->name)
            {
                if(stateIt->state == WAR)
                    al_draw_rectangle(it->x+worldx-1, it->y+worldy-1,
                                      it->x+it->w+worldx+1, it->y+it->h+worldy+1, al_map_rgb(255, 0, 0), 3);
                else if(stateIt->state == ALLIANCE)
                    al_draw_rectangle(it->x+worldx-1, it->y+worldy-1,
                                      it->x+it->w+worldx+1, it->y+it->h+worldy+1, al_map_rgb(0, 255, 0), 3);
                else if(stateIt->state == TRADE_AGREEMENT)
                    al_draw_rectangle(it->x+worldx-1, it->y+worldy-1,
                                      it->x+it->w+worldx+1, it->y+it->h+worldy+1, al_map_rgb(0, 0, 255), 3);
                else
                    al_draw_rectangle(it->x+worldx-1, it->y+worldy-1,
                                      it->x+it->w+worldx+1, it->y+it->h+worldy+1, al_map_rgb(255, 255, 255), 3);
            }
        }

        al_draw_filled_rectangle(it->x+worldx, it->y+worldy, it->x+it->w+worldx, it->y+it->h+worldy, it->owner->color);
        al_draw_rectangle(it->x+worldx, it->y+worldy, it->x+it->w+worldx, it->y+it->h+worldy, al_map_rgb(0, 0, 0), 2);

        if(&*it==selectedRegion)
        {
            al_draw_circle(it->x+worldx+5, it->y+worldy+5, 10, al_map_rgb(0,0,0), 4);
        }
    }
}

bool Map::checkRegionDrawInfo(int x, int y)
{
    int worldx = Base::instance()->getCoords()->first;
    int worldy = Base::instance()->getCoords()->second;

    std::list<Region>::iterator it;
    for(it=regions.begin(); it!=regions.end(); it++)
    {
        if((x>=it->x+worldx && x<=it->x+it->w+worldx) &&
                (y>=it->y+worldy && y<=it->y+it->h+worldy))
        {
            std::string garnizon;
            garnizon.append("Garnizon: ");
            garnizon.append(std::to_string(it->garnizon));
            garnizon.append(" jednostek");

            std::string dyplomacja;
            std::list<DiplomaticState>::iterator it2;
            for(it2=playerTurn->realm->diplomacy.begin(); it2!=playerTurn->realm->diplomacy.end(); it2++)
            {
                if(playerTurn->realm->name == it->owner->name)
                {
                    dyplomacja = "Własny";
                    break;
                }
                else if(it2->realm->name == it->owner->name)
                {
                    if(it2->state == WAR)
                        dyplomacja = "Wojna";
                    else if(it2->state == ALLIANCE)
                        dyplomacja = "Sojusz";
                    else if(it2->state == TRADE_AGREEMENT)
                        dyplomacja = "Umowa handlowa";
                    else
                        dyplomacja = "Neutralny";
                    break;
                }
            }

            Engine *engine = Base::instance()->getEngine();

            std::string dochod("Dochód: " + std::to_string(it->incomeAmount) + " sztuk złota");
            int namelen = engine->getTextWH(it->regionname).width;
            int ownerlen = engine->getTextWH(it->owner->name).width;
            int garnizonlen = engine->getTextWH(garnizon).width;
            int dochodlen = engine->getTextWH(dochod).width;
            int dyplomacjalen = engine->getTextWH(dyplomacja).width;
            int rectanglesize = max(namelen, ownerlen);
            rectanglesize = max(rectanglesize, garnizonlen);
            rectanglesize = max(rectanglesize, dochodlen);
            rectanglesize = max(rectanglesize, dyplomacjalen);

            int screenW = Base::instance()->getEngine()->getWindowSize().x;
            int screenH = Base::instance()->getEngine()->getWindowSize().y;

            al_draw_filled_rectangle(it->x+(0.0097*screenW)+worldx, it->y+worldy,
                                     it->x+rectanglesize + (0.013*screenW) +worldx,it->y-(0.120*screenH)+worldy,
                                     al_map_rgb(0, 0, 0));
            al_draw_rectangle(it->x+(0.0097*screenW)+worldx, it->y+worldy,
                              it->x+rectanglesize + (0.013*screenW) +worldx,it->y-(0.120*screenH)+worldy,
                              al_map_rgb(255, 127, 0), 2);

            engine->writeText(it->regionname, {it->x+(0.012*screenW)+worldx, it->y-(0.120*screenH)+worldy}, it->owner->color);
            engine->writeText(it->owner->name, {it->x+(0.012*screenW)+worldx, it->y-(0.100*screenH)+worldy}, it->owner->color);
            engine->writeText(garnizon, {it->x+(0.012*screenW)+worldx, it->y-(0.065*screenH)+worldy}, it->owner->color);
            engine->writeText(dochod, {it->x+(0.012*screenW)+worldx, it->y-(0.045*screenH)+worldy}, it->owner->color);
            engine->writeText(dochod, {it->x+(0.012*screenW)+worldx, it->y-(0.025*screenH)+worldy}, it->owner->color);

            return true;
        }
    }

    return false;
}

bool Map::addRealm(std::string name, ALLEGRO_COLOR color, std::string signDir)
{
    Realm realm;
    realm.color = color;
    realm.name = name;
    realm.signDir = signDir;
    realm.money = 1000; //FIXED
    realm.regions.clear();

    realms.push_back(realm);

    return true;
}

void Map::fillRealmsRegions()
{
    std::list<Realm>::iterator it;
    for(it=realms.begin();it!=realms.end();it++)
    {
        std::list<Region>::iterator it2;
        for(it2=regions.begin();it2!=regions.end();it2++)
        {
            if(it->name == it2->owner->name)
            {
                it->regions.push_back(&*it2);
            }
        }
    }
}

int Map::calculateIncome(Realm *realm)
{
    int income = 0;

    std::vector<Region*>::iterator it;
    for(it = realm->regions.begin(); it!= realm->regions.end(); it++)
    {
        income += (*it)->incomeAmount;
    }

    return income;
}

bool Map::addRegionsConnector(std::string firstRegion, std::string secondRegion)
{
    Region *first;
    Region *second;

    bool fc=false;
    bool sc=false;
    std::list<Region>::iterator it;
    for(it = regions.begin(); it!=regions.end(); it++)
    {
        if(it->regionname == firstRegion)
        {
            first = &*it;
            fc=true;
        }
        else if(it->regionname == secondRegion)
        {
            second = &*it;
            sc=true;
        }
    }

    if(!fc || !sc)
        return false;

    first->regionsConnected.push_back(second);
    second->regionsConnected.push_back(first);

    return true;
}

void Map::addPlayer(std::string name, std::string realmName, bool isHuman)
{
    Player player;
    player.numberId = playerNextNumber;
    playerNextNumber+=1;
    player.isHuman = isHuman;
    player.name = name;

    std::list<Realm>::iterator it;
    for(it = realms.begin(); it!=realms.end(); it++)
    {
        if(it->name == realmName)
        {
            player.realm = &*it;
            break;
        }
    }


    players.push_back(player);
}

bool Map::selectRegion(int x, int y)
{
    int worldx = Base::instance()->getCoords()->first;
    int worldy = Base::instance()->getCoords()->second;

    movType = NOTHING;

    std::list<Region>::iterator it;
    for(it = regions.begin(); it!=regions.end(); it++)
    {
        if((x>=it->x+worldx && x<=it->x+it->w+worldx) && (y>=it->y+worldy && y<=it->y+it->h+worldy))
        {
            selectedRegion = &*it;
            return true;
        }
    }

    selectedRegion = NULL;
    return false;
}

void Map::activateRegion(int x, int y)
{
    int worldx = Base::instance()->getCoords()->first;
    int worldy = Base::instance()->getCoords()->second;

    if(selectedRegion==NULL || movType==NOTHING)
        return;

    Region *region;
    std::list<Region>::iterator it;
    for(it = regions.begin(); it!=regions.end(); it++)
    {
        if((x>=it->x+worldx && x<=it->x+it->w+worldx) && (y>=it->y+worldy && y<=it->y+it->h+worldy))
        {
            region = &*it;
            break;
        }
    }

    if(region==NULL || selectedRegion==region)
        return;

    bool connected=false;
    std::vector<Region*>::iterator it2;
    for(it2=selectedRegion->regionsConnected.begin();it2!=selectedRegion->regionsConnected.end();it2++)
    {
        Region *r2 = *it2;
        if(r2 == region)
            connected=true;
    }

    if(!connected)
        return;

    activeRegion = region;
    Base::instance()->getGuiPtr()->activateRegion(region);
}

void Map::fight(std::string regionAttacking, std::string regionDefending)
{

}

void Map::setMapName(std::string name)
{
    nazwaMapy = name;
}

std::string Map::getMapName()
{
    return nazwaMapy;
}

ALLEGRO_BITMAP* Map::getMapaPtr()
{
    return mapaPtr;
}

std::pair<int*,int*> Map::getMapaWH()
{
    std::pair<int*,int*> para;
    para.first = &mapaWidth;
    para.second = &mapaHeight;
    return para;
}

void Map::moveSelection()
{
    if(selectedRegion == NULL)
        return;

    if(movType==MOVE)
        movType=NOTHING;
    else
        movType=MOVE;
}

void Map::move(int quantity)
{
    findRegionByName(selectedRegion->regionname)->changeRegionGarnizon(selectedRegion->garnizon-quantity);
    findRegionByName(activeRegion->regionname)->changeRegionGarnizon(activeRegion->garnizon+quantity);
    activeRegion = NULL;
    movType = NOTHING;
}

void Map::attackSelection()
{
    if(selectedRegion == NULL)
        return;

    if(movType==ATTACK)
        movType=NOTHING;
    else
        movType=ATTACK;
}

void Map::attack(int quantity)
{

}

void Map::centerOnCoords(int x, int y) const
{
    int screenW = Base::instance()->getEngine()->getWindowSize().x;
    int screenH = Base::instance()->getEngine()->getWindowSize().y;

    int calcX = -x + (screenW/2);
    int calcY = -y + (screenH/2);

    if(calcX<(-mapaWidth+screenW+8))
        Base::instance()->getCoords()->first = (-mapaWidth+screenW+8);
    else if(calcX>0)
        Base::instance()->getCoords()->first = 0;
    else
        Base::instance()->getCoords()->first = calcX;

    if(calcY<(-mapaHeight+screenH+8-(0.1*screenH)))
        Base::instance()->getCoords()->second = (-mapaHeight+screenH+8-(0.1*screenH));
    else if(calcY>0)
        Base::instance()->getCoords()->second = 0;
    else
        Base::instance()->getCoords()->second = calcY;
}

Region* Map::findRegionByName(std::string regionName)
{
    std::list<Region>::iterator it;
    for(it = regions.begin(); it!=regions.end(); it++)
    {
        if(it->regionname == regionName)
        {
            return &*it;
        }
    }
}

void Region::changeRegionGarnizon(int garnizon)
{
    this->garnizon = garnizon;
}

void Region::changeRegionOwnership(std::string ownername)
{
    std::vector<Region*>::iterator it;
    for(it = this->owner->regions.begin();it!=this->owner->regions.end();it++)
    {
        if((*it)->regionname == this->regionname)
            this->owner->regions.erase(it);
    }

    Map *mapPtr = Base::instance()->getMapPtr();

    std::list<Realm>::iterator it2;
    for(it2=mapPtr->realms.begin();it2!=mapPtr->realms.end();it2++)
    {
        if(it2->name == ownername)
            this->owner = &*it2;
    }
}
